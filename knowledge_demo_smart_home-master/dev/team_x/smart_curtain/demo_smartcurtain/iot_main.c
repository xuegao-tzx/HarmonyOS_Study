/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "iot_cloud.h"
#include "iot_profile.h"
#include "iot_boardled.h"
#include "iot_boardbutton.h"
#include "iot_nfc.h"
#include "iot_wifi.h"
#include "iot_netcfg.h"
#include "iot_sntp.h"
#include "iot_gpio.h"
#include "ohos_init.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <cmsis_os2.h>
#include <stdbool.h>
#include "kv_store.h"
#include "iot_schedule.h"
#include "network_config_service.h"
#include "iot_netcfg_nan.h"

#include "hi_reset.h"

#define    SID_KEY        "ssid"
#define    PWD_KEY        "password"

#define    SOFTAP_NAME    "teamX-Curtain01"

// DEFAULT PARAM, AND MUST BE MODIFIED TO THE REAL PARAMS
#define CONFIG_CLOUD_DEFAULT_SERVERIP "121.36.42.100"    // replace it with iotplatform server ip
#define CONFIG_CLOUD_DEFAULT_SERVERPORT "1883"           // replace it with iotplatform server  port
#define CONFIG_SENSOR_SAMPLE_CYCLE 10000                  // unit:minisecond
#define CONFIG_TASK_DEFAULT_STACKSIZE 0x1000             // unit:bytes
#define CONFIG_TASK_DEFAULT_PRIOR 20                     // default task priority
#define CONFIG_TASK_MAIN_STACKSIZE 0x4000                // main task stacksize must be bigger
#define CONFIG_TASK_MAIN_PRIOR 25                        // default task priority
#define CONFIG_LOCAL_TIMEZONE            8
#define CONFIG_LED_DUTYCYCLEDEFAULT      20
#define CONFIG_LED_DUTYMAXMUN            99
#define CONFIG_LED_DUTYMINIMUM           1
#define CONFIG_FLASHLED_FRENETCONFIG     8
#define CONFIG_FLASHLED_FREWIFI          4
#define CONFIG_FLASHLED_FRECLOUD         1
#define CONFIG_FLASHLED_WORKSWELL        0.1
#define MAIN_TASK_DELAY_TICKS            100

typedef struct {
    int curtainStatus;
    int pwmLedDutyCycle;
    int lightIntensity;
    int curDay;
    int curSecondsInDay;
    osEventFlagsId_t curtainEvent;
}ApplicationController;
static ApplicationController g_appController;

#define CN_LAMP_EVENT_SETSTATUS  (1<<0)
#define CN_LAMP_EVENT_SETDUTY    (1<<1)curtainStatus
#define CN_LAMP_EVENT_ALL   (CN_LAMP_EVENT_SETSTATUS | CN_LAMP_EVENT_SETDUTY)

#define CN_CURTAIN_EVENT_SETSTATUS  (1<<0)

static void RtcTimeUpdate(void)
{
    extern int SntpGetRtcTime(int localTimeZone, struct tm *rtcTime);
    struct tm rtcTime;
    SntpGetRtcTime(CONFIG_LOCAL_TIMEZONE,&rtcTime);

    RaiseLog(LOG_LEVEL_INFO, "Year:%d Month:%d Mday:%d Wday:%d Hour:%d Min:%d Sec:%d", \
            rtcTime.tm_year + BASE_YEAR_OF_TIME_CALC, rtcTime.tm_mon + 1, rtcTime.tm_mday,\
            rtcTime.tm_wday, rtcTime.tm_hour, rtcTime.tm_min, rtcTime.tm_sec);

    if (rtcTime.tm_wday > 0) {
        g_appController.curDay = rtcTime.tm_wday - 1;
    } else {
        g_appController.curDay = EN_SUNDAY;
    }

    g_appController.curSecondsInDay = rtcTime.tm_hour * CN_SECONDS_IN_HOUR + \
            rtcTime.tm_min * CN_SECONDS_IN_MINUTE + rtcTime.tm_sec + 8; // add 8 ms offset
}

/**
 * @brief Convert miniseconds to system ticks
 * @param ms Indicates the mimiseconds to convert
 * @return Returns the corresponding ticks of specified time
 */
static uint32_t Time2Tick(uint32_t ms)
{
    uint64_t ret;
    ret = ((uint64_t)ms * osKernelGetTickFreq()) / CN_MINISECONDS_IN_SECOND;
    return (uint32_t)ret;
}

#define CN_REACTION_TIME_SECONDS   1
static void BoardLedButtonCallbackF1(char *arg)
{
    static uint32_t lastSec = 0;
    uint32_t curSec = 0;
    RaiseLog(LOG_LEVEL_INFO, "BUTTON PRESSED");

    curSec = g_appController.curSecondsInDay;
    if((curSec) < (lastSec + CN_REACTION_TIME_SECONDS)) {
        RaiseLog(LOG_LEVEL_WARN, "Frequecy Pressed Button");
        return;
    }
    lastSec = curSec;

    g_appController.curtainStatus = CN_BOARD_SWITCH_ON;
    g_appController.pwmLedDutyCycle = \
        g_appController.pwmLedDutyCycle > 0 ? g_appController.pwmLedDutyCycle:CONFIG_LED_DUTYCYCLEDEFAULT;

    osEventFlagsSet(g_appController.curtainEvent, CN_LAMP_EVENT_SETSTATUS);

    return;
}

static void BoardLedButtonCallbackF2(char *arg)
{
      uint32_t lastSec = 0;
    uint32_t curSec = 0;
    RaiseLog(LOG_LEVEL_INFO, "BUTTON PRESSED");

    curSec = g_appController.curSecondsInDay;
    if ((curSec) < (lastSec + CN_REACTION_TIME_SECONDS)) {
        RaiseLog(LOG_LEVEL_WARN, "Frequecy Pressed Button");
        return;
    }
    lastSec = curSec;

    g_appController.curtainStatus = CN_BOARD_SWITCH_OFF;
    osEventFlagsSet(g_appController.curtainEvent, CN_LAMP_EVENT_SETSTATUS);

    return;
}

#define CURTAIN_MOTOR_GPIO_IDX 8
#define WIFI_IOT_IO_FUNC_GPIO_8_GPIO 0
#define WIFI_IOT_IO_FUNC_GPIO_14_GPIO 4
#define MOTOR_WORK_SECOND 6

static void E53SC1_MotorInit(void)
{
    IoTGpioInit(CURTAIN_MOTOR_GPIO_IDX);
    IoTGpioSetFunc(CURTAIN_MOTOR_GPIO_IDX, WIFI_IOT_IO_FUNC_GPIO_8_GPIO);
    IoTGpioSetDir(CURTAIN_MOTOR_GPIO_IDX, IOT_GPIO_DIR_OUT); //设置GPIO_8为输出模式
    return;
}

static void E53SC1_SetCurtainStatus(int curtainStatus)
{
   if ((curtainStatus == CN_BOARD_CURTAIN_OPEN) || (curtainStatus == CN_BOARD_CURTAIN_CLOSE)) {
        IoTGpioSetOutputVal(CURTAIN_MOTOR_GPIO_IDX, 1); //设置GPIO_8输出高电平打开电机
        sleep(MOTOR_WORK_SECOND);
        IoTGpioSetOutputVal(CURTAIN_MOTOR_GPIO_IDX, 0); //设置GPIO_8输出低电平关闭电机
    }
    return;
}

/**
 * @brief Sensor data collection task entry and the main flow is as follows:
 *        1, Wait the for the event to tcurtainStatusrigger in specified time
 *        2, Collect Sensor Data (air humidity/temperature, soil moisture, etc.)
 *        3, Report the collected data to the Cloud Platform
 */
static void DataCollectAndReport(const void *arg)
{
    (void)arg;
    uint32_t curtainEvent;
    uint32_t waitTicks;

    waitTicks = Time2Tick(CONFIG_SENSOR_SAMPLE_CYCLE);
    while (1) {

        curtainEvent = osEventFlagsWait(g_appController.curtainEvent, CN_CURTAIN_EVENT_SETSTATUS, \
            osFlagsWaitAny, waitTicks);
        if (curtainEvent & CN_CURTAIN_EVENT_SETSTATUS) {
            RaiseLog(LOG_LEVEL_INFO, "GetEvent:%08x", curtainEvent);
            E53SC1_SetCurtainStatus(g_appController.curtainStatus);
        }

        (void) IotProfile_Report(g_appController.curtainStatus);

    }
    return;
}

static int UpdateShedule(CommandParamSetShedule *shedule)
{
    if (shedule->num == 1 && shedule->day[0] == 0) {        // set the one time schedule to current weekday
        shedule->day[0] = (g_appController.curDay + 1);
    }

    switch (shedule->option) {
        case 'A':
            IOT_ScheduleAdd(shedule->scheduleID, shedule->day, shedule->num, shedule->startHour * CN_SECONDS_IN_HOUR +\
                shedule->startMinute * CN_SECONDS_IN_MINUTE, shedule->duration, shedule->shedulecmd.cmd, 0);
            break;
        case 'U':
            IOT_ScheduleUpdate(shedule->scheduleID, shedule->day, shedule->num, shedule->startHour * CN_SECONDS_IN_HOUR +\
                shedule->startMinute * CN_SECONDS_IN_MINUTE, shedule->duration, shedule->shedulecmd.cmd, 0);
            break;
        case 'D':
            IOT_ScheduleDelete(shedule->scheduleID, shedule->day, shedule->num, shedule->startHour * CN_SECONDS_IN_HOUR +\
                shedule->startMinute * CN_SECONDS_IN_MINUTE, shedule->duration, shedule->shedulecmd.cmd, 0);
            break;
        default:
            RaiseLog(LOG_LEVEL_ERR, "the schedule has no such option!\n");
            break;
    }
    return 0;
}

void CurrentTimeCalcTimerHander(){
   g_appController.curSecondsInDay ++;
}

#define TimeCalcTicks_NUMBER 100
#define CN_MINISECONDS_IN_1000MS 1000

static void CurtainShedule(void)
{
    int startSecondInDay = 0;
    int endSecondInDay = 0;
    int settingCmd = 0;
    int executeTaskTime = 0;  // indicate the do something busy
    osTimerId_t CurrentTimeCalc_id;
    CurrentTimeCalc_id = osTimerNew(CurrentTimeCalcTimerHander, osTimerPeriodic, NULL, NULL);
    osTimerStart(CurrentTimeCalc_id, TimeCalcTicks_NUMBER);

    while (1) {
        osDelay(Time2Tick(CN_MINISECONDS_IN_1000MS));
    
        if (g_appController.curSecondsInDay >= CN_SECONS_IN_DAY) {
            g_appController.curSecondsInDay = 0;
            g_appController.curDay++;
            if (g_appController.curDay >= EN_DAYALL) {
                g_appController.curDay = EN_MONDAY;
            }
            IOT_ScheduleSetUpdate(1);
        }
        // check if we need do some task here
        if (IOT_ScheduleIsUpdate(g_appController.curDay, g_appController.curSecondsInDay)) {
            if (executeTaskTime > 0) {
                executeTaskTime = 0;
                if (g_appController.curtainStatus == CN_BOARD_CURTAIN_OPEN) {
                    g_appController.curtainStatus = CN_BOARD_CURTAIN_CLOSE;
                    osEventFlagsSet(g_appController.curtainEvent, CN_CURTAIN_EVENT_SETSTATUS);
                }
            }
            startSecondInDay = IOT_ScheduleGetStartTime();
            endSecondInDay = startSecondInDay + IOT_ScheduleGetDurationTime();

            IOT_ScheduleGetCommand(&settingCmd, NULL);
        }

        RaiseLog(LOG_LEVEL_INFO, "start:%d end:%d cur:%d",startSecondInDay, endSecondInDay, g_appController.curSecondsInDay);
        if ((endSecondInDay == startSecondInDay) && (g_appController.curSecondsInDay == endSecondInDay)) {
            if (g_appController.curtainStatus != settingCmd) {
                RaiseLog(LOG_LEVEL_INFO, "Triggering");
                g_appController.curtainStatus = settingCmd;
                osEventFlagsSet(g_appController.curtainEvent, CN_CURTAIN_EVENT_SETSTATUS);
            }
            IOT_ScheduleSetUpdate(1);
        }
    }
    return;
}

/**
 * @brief Callback function used to process commands delivered by the Cloud Platform
 * @param command Indicates the command delivered by the Cloud Platform
 * @param value Indicates the parameter corresponding to the command
 * @return Returns 0 success while -1 failed
 */
int IotProfile_CommandCallback(int command, void *buf)
{
    CommandParamSetShedule setSheduleParam;
    CommandParamSetCurtain setCurtainParam;
    //CommandParamSetDutyCycle setDutyCycleParam;
    CLOUD_CommandType cmd = (CLOUD_CommandType)command;

    /**
     * Here, we only process the command for controlling the motor issued by the Cloud Platform, and the
     * value should be CN_BOARD_CURTAIN_OPEN or CN_BOARD_CURTAIN_CLOSE
     */
    if (cmd == CLOUD_COMMAND_SETCURTAIN_STATUS) {
        setCurtainParam = *(CommandParamSetCurtain *)buf;
        g_appController.curtainStatus = setCurtainParam.status;
        //g_appController.lightManual = true;
        RaiseLog(LOG_LEVEL_INFO, "setCurtainParam.status:%d\r\n", setCurtainParam.status);
        osEventFlagsSet(g_appController.curtainEvent, CN_LAMP_EVENT_SETSTATUS);
        return 0;

    } else if (cmd == CLOUD_COMMAND_SETSHEDULE) {
        setSheduleParam = *(CommandParamSetShedule *)buf;
        RaiseLog(LOG_LEVEL_INFO, "setshedule:day:%d hour:%d minute:%d duration:%d \r\n", \
            setSheduleParam.day,setSheduleParam.startHour,setSheduleParam.startMinute, setSheduleParam.duration);
        return UpdateShedule(&setSheduleParam);
    }

    return -1;
}

static int IotWifiInfo_get(char *ssid, int id_size, char *pwd, int pd_size)
{
    int retval = UtilsGetValue(SID_KEY, ssid, id_size);
    if (retval <= 0) {
        RaiseLog(LOG_LEVEL_ERR, "no such ssid stored! \n");
        return 0;
    }

    if ( UtilsGetValue(PWD_KEY, pwd, pd_size) < 0) {
        RaiseLog(LOG_LEVEL_INFO, "ssid(%s) no password stored! \n", ssid);
    } else {
        RaiseLog(LOG_LEVEL_INFO, "ssid : %s, pwd : %s! \n", ssid, pwd);
    }

    return 1;
}

static void IotWifiInfo_set(char *ssid, char *pwd)
{
    if (UtilsSetValue(SID_KEY, ssid) != 0) {
        RaiseLog(LOG_LEVEL_ERR, "store ssid failed! \n");
        return;
    }
    if (UtilsSetValue(PWD_KEY, pwd) != 0) {
        RaiseLog(LOG_LEVEL_ERR, "store password failed! \n");
        UtilsDeleteValue(SID_KEY);
        return;
    }
    RaiseLog(LOG_LEVEL_INFO, "store password success! \n");
}

/**
 * @brief IoTMain task entry and the main flow is as follows:
 *        1, Initialize the necessary hardware and obtain the corresponding information from NFC
 *        2, Connecting to wifi
 *        3, Connecting to the HUAWEI CLOUD Platform
 *        4, Creating a Sensor Collection Task
 */
static void IotMainTaskEntry(const void *arg)
{
    osThreadAttr_t attr;
    NfcInfo nfcInfo;
    (void)arg;
    char ssid[BUFF_SIZE] = {0};
    char pwd[BUFF_SIZE] = {0};
    int ret = 0;

    g_appController.pwmLedDutyCycle = CONFIG_LED_DUTYCYCLEDEFAULT;
    BOARD_InitPwmLed();
    BOARD_InitWifi();
    E53SC1_MotorInit();

    IOT_ScheduleInit();
    ret = Board_IsButtonPressedF2();
    osDelay(MAIN_TASK_DELAY_TICKS);

    // if any netconfiguration
    LedFlashFrequencySet(CONFIG_FLASHLED_FRENETCONFIG);

    nfcInfo.deviceID = "6136ceba0ad1ed02866fa3b2_Curtain01";
    nfcInfo.devicePWD = "12345678";

    if (ret) {
        RaiseLog(LOG_LEVEL_INFO, "Netconfig Button has pressed! \n");
        if (BOARD_NAN_NetCfgStartConfig(SOFTAP_NAME, ssid, sizeof(ssid), pwd, sizeof(pwd)) < 0) {
            RaiseLog(LOG_LEVEL_ERR, "BOARD_NetCfgStartConfig failed! \n");
            return;
        } else {
            ret = AFTER_NETCFG_ACTION;
        }
    } else {
        ret = IotWifiInfo_get(ssid, sizeof(ssid), pwd, sizeof(pwd));
        if (ret == 0) {
            if (BOARD_NAN_NetCfgStartConfig(SOFTAP_NAME, ssid, sizeof(ssid), pwd, sizeof(pwd)) < 0) {
                RaiseLog(LOG_LEVEL_ERR, "BOARD_NetCfgStartConfig failed! \n");
                return;
            } else {
                ret = AFTER_NETCFG_ACTION;
            }
        } 
    }

    LedFlashFrequencySet(CONFIG_FLASHLED_FREWIFI);
    if (BOARD_ConnectWifi(ssid, pwd) != 0) {
        RaiseLog(LOG_LEVEL_ERR, "BOARD_ConnectWifi failed! \n");
        if (ret == AFTER_NETCFG_ACTION) {
            NotifyNetCfgResult(NETCFG_DEV_INFO_INVALID);
        }
        hi_hard_reboot(HI_SYS_REBOOT_CAUSE_CMD);
        return;
    }

    if (ret == AFTER_NETCFG_ACTION) {
        RaiseLog(LOG_LEVEL_DEBUG, "Connect wifi success ! \n");
        NotifyNetCfgResult(NETCFG_OK);
        osDelay(MAIN_TASK_DELAY_TICKS);
        RaiseLog(LOG_LEVEL_DEBUG, "StopNetCfg wifi success ! \n");
        StopNetCfg();
        IotWifiInfo_set(ssid, pwd);
    }

    // update the RTC time and connect to the cloud
    LedFlashFrequencySet(CONFIG_FLASHLED_FRECLOUD);
    RtcTimeUpdate();
    if (CLOUD_Init() != 0) {
        return;
    }

    if (CLOUD_Connect(nfcInfo.deviceID, nfcInfo.devicePWD, \
        CONFIG_CLOUD_DEFAULT_SERVERIP, CONFIG_CLOUD_DEFAULT_SERVERPORT) != 0) {
        return;
    }

    LedFlashFrequencySet(CONFIG_FLASHLED_WORKSWELL);
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = CONFIG_TASK_DEFAULT_STACKSIZE;
    attr.priority = CONFIG_TASK_DEFAULT_PRIOR;
    attr.name = "DataCollectAndReport";
    if (osThreadNew((osThreadFunc_t)DataCollectAndReport, NULL, (const osThreadAttr_t *)&attr) == NULL) {
        return;
    }

    attr.name = "CurtainShedule";
    if (osThreadNew((osThreadFunc_t)CurtainShedule, NULL, (const osThreadAttr_t *)&attr) == NULL) {
        return;
    }
    return;
}

/**
 * @brief IoT Main Entry of the IoT-Flower project
 */
static void IotMainEntry(void)
{
    osThreadAttr_t attr;
    RaiseLog(LOG_LEVEL_INFO, "DATA:%s Time:%s \r\n",__FUNCTION__, __DATE__, __TIME__);
    g_appController.curtainEvent = osEventFlagsNew(NULL);
    if ( g_appController.curtainEvent == NULL) {
        return;
    }
    // Create the IoT Main task
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = CONFIG_TASK_MAIN_STACKSIZE;
    attr.priority = CONFIG_TASK_MAIN_PRIOR;
    attr.name = "IoTMain";
    (void) osThreadNew((osThreadFunc_t)IotMainTaskEntry, NULL, (const osThreadAttr_t *)&attr);

    return;
}

// Add the IotMainEntry function to the System Startup Procedure
APP_FEATURE_INIT(IotMainEntry);
