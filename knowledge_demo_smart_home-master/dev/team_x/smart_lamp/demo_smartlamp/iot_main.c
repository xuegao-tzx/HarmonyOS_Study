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
#include "ohos_init.h"
#include "E53_SC1.h"
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

#define    SOFTAP_NAME    "teamX-Lamp01"

// DEFAULT PARAM, AND MUST BE MODIFIED TO THE REAL PARAMS
#define CONFIG_CLOUD_DEFAULT_SERVERIP "121.36.42.100"    // replace it with iotplatform server ip
#define CONFIG_CLOUD_DEFAULT_SERVERPORT "1883"           // replace it with iotplatform server  port
#define CONFIG_SENSOR_SAMPLE_CYCLE 30000                  // unit:minisecond
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

typedef struct {
    int mainLedStatus;
    int pwmLedDutyCycle;
    int lightIntensity;
    int curDay;
    int curSecondsInDay;
    bool lightManual;

    osTimerId_t timeID;
    int executeTime;
    int timerFlag;

    osEventFlagsId_t lampEvent;
}ApplicationController;
static ApplicationController g_appController;

#define CN_LAMP_EVENT_SETSTATUS  (1<<0)
#define CN_LAMP_EVENT_SETDUTY    (1<<1)
#define CN_LAMP_EVENT_ALL   (CN_LAMP_EVENT_SETSTATUS | CN_LAMP_EVENT_SETDUTY)

static void RtcTimeUpdate(void)
{
    extern int SntpGetRtcTime(int localTimeZone, struct tm *rtcTime);
    struct tm rtcTime;
    SntpGetRtcTime(CONFIG_LOCAL_TIMEZONE, &rtcTime);

    RaiseLog(LOG_LEVEL_INFO," Year:%d Month:%d Mday:%d Wday:%d Hour:%d Min:%d Sec:%d", \
        rtcTime.tm_year + BASE_YEAR_OF_TIME_CALC, rtcTime.tm_mon + 1, rtcTime.tm_mday,\
            rtcTime.tm_wday, rtcTime.tm_hour, rtcTime.tm_min, rtcTime.tm_sec);

    if (rtcTime.tm_wday > 0) {
        g_appController.curDay = rtcTime.tm_wday - 1;
    } else {
        g_appController.curDay = 6;
    }

    g_appController.curSecondsInDay = rtcTime.tm_hour * CN_SECONDS_IN_HOUR + \
        rtcTime.tm_min * CN_SECONDS_IN_MINUTE + rtcTime.tm_sec + 8; // add 8 s offset
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
    if ((curSec) < (lastSec + CN_REACTION_TIME_SECONDS)) {
        RaiseLog(LOG_LEVEL_WARN, "Frequecy Pressed Button");
        return;
    }
    lastSec = curSec;

    g_appController.lightManual = true;
    g_appController.mainLedStatus = CN_BOARD_SWITCH_ON;
    g_appController.pwmLedDutyCycle = \
        g_appController.pwmLedDutyCycle > 0 ? g_appController.pwmLedDutyCycle:CONFIG_LED_DUTYCYCLEDEFAULT;

    osEventFlagsSet(g_appController.lampEvent, CN_LAMP_EVENT_SETSTATUS);


    return;
}

static void BoardLedButtonCallbackF2(char *arg)
{
    static uint32_t lastSec = 0;
    uint32_t curSec = 0;
    RaiseLog(LOG_LEVEL_INFO, "BUTTON PRESSED");

    curSec = g_appController.curSecondsInDay;
    if ((curSec) < (lastSec + CN_REACTION_TIME_SECONDS)) {
        RaiseLog(LOG_LEVEL_WARN, "Frequecy Pressed Button");
        return;
    }
    lastSec = curSec;

    g_appController.lightManual = true;
    g_appController.mainLedStatus = CN_BOARD_SWITCH_OFF;
    osEventFlagsSet(g_appController.lampEvent, CN_LAMP_EVENT_SETSTATUS);

    return;
}

/**
 * @brief Sensor data collection task entry and the main flow is as follows:
 *        1, Wait the for the event to trigger in specified time
 *        2, Collect Sensor Data (air humidity/temperature, soil moisture, etc.)
 *        3, Report the collected data to the Cloud Platform
 */
static void DataCollectAndReport(const void *arg)
{
    (void)arg;
    uint32_t lampEvent;
    uint32_t waitTicks;

    waitTicks = Time2Tick(CONFIG_SENSOR_SAMPLE_CYCLE);
    while (1) {

        lampEvent = osEventFlagsWait(g_appController.lampEvent, CN_LAMP_EVENT_ALL, \
            osFlagsWaitAny, waitTicks);
        if (lampEvent & CN_LAMP_EVENT_ALL) {
            RaiseLog(LOG_LEVEL_INFO, "GetEvent:%08x", lampEvent);
            if(lampEvent & CN_LAMP_EVENT_SETSTATUS) {
                E53SC1_SetMainLed(g_appController.mainLedStatus, g_appController.pwmLedDutyCycle);
            }
        }

        if (E53SC1_GetLightIntensity(&g_appController.lightIntensity) == 0) {
            (void) IotProfile_Report( g_appController.mainLedStatus, \
                g_appController.pwmLedDutyCycle, g_appController.lightIntensity);
            RaiseLog(LOG_LEVEL_INFO, "Get the sensor data:Led:%d DutyCycle:%d Intensity:%d",\
                g_appController.mainLedStatus, g_appController.pwmLedDutyCycle, g_appController.lightIntensity);
        } else {
            RaiseLog(LOG_LEVEL_ERR, "Get the sensor data error");
        }

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
                shedule->startMinute * CN_SECONDS_IN_MINUTE, shedule->duration * CN_SECONDS_IN_MINUTE, shedule->lampStatus.status, shedule->lampStatus.dutyCycle);
            break;
        case 'U':
            IOT_ScheduleUpdate(shedule->scheduleID, shedule->day, shedule->num, shedule->startHour * CN_SECONDS_IN_HOUR +\
                shedule->startMinute * CN_SECONDS_IN_MINUTE, shedule->duration * CN_SECONDS_IN_MINUTE, shedule->lampStatus.status, shedule->lampStatus.dutyCycle);
            break;
        case 'D':
            IOT_ScheduleDelete(shedule->scheduleID, shedule->day, shedule->num, shedule->startHour * CN_SECONDS_IN_HOUR +\
                shedule->startMinute * CN_SECONDS_IN_MINUTE, shedule->duration * CN_SECONDS_IN_MINUTE, shedule->lampStatus.status, shedule->lampStatus.dutyCycle);
            break;
        default:
            RaiseLog(LOG_LEVEL_ERR, "the schedule has no such option!\n");
            break;
    }

    return 0;
}

void TimeCalcTimerHander(void *args)
{
    (void)args;
    g_appController.curSecondsInDay++;
    if (g_appController.timerFlag) {
         if (g_appController.executeTime > 0) {
             g_appController.executeTime--;
         }
    }
}

#define TIMER_TICK_NUM 100

static void LampShedule(void)
{
    int startSecondInDay = 0;
    int endSecondInDay = 0;

    CommandParamSetLamp lampStatus, lastLampStatus;
    lastLampStatus.status = lampStatus.status = 0;
    lastLampStatus.dutyCycle = lampStatus.dutyCycle = 0;

    while (1) {
        osDelay(Time2Tick(CN_MINISECONDS_IN_SECOND));
        RaiseLog(LOG_LEVEL_INFO, "CurrentTime %02d:%02d:%02d\n", g_appController.curSecondsInDay/3600,\
            (g_appController.curSecondsInDay%3600)/60, g_appController.curSecondsInDay%60);
        if (g_appController.curSecondsInDay >= CN_SECONS_IN_DAY) {
            g_appController.curSecondsInDay = 0;
            g_appController.curDay++;
            g_appController.lightManual = false;   // clear it
            if (g_appController.curDay >= EN_DAYALL) {
                g_appController.curDay = EN_MONDAY;
            }
            IOT_ScheduleSetUpdate(1);
        }
        printf("[%s|%d] \n", __func__, __LINE__);
        
        printf("g_appController.curDay = %d \n", g_appController.curDay);
        if (IOT_ScheduleIsUpdate(g_appController.curDay, g_appController.curSecondsInDay)) {
            printf("[%s|%d] \n", __func__, __LINE__);
            if (g_appController.timerFlag > 0) {
                g_appController.timerFlag = 0;
                g_appController.executeTime = 0;
                if (g_appController.mainLedStatus != lastLampStatus.status) {
                    g_appController.mainLedStatus = lastLampStatus.status;
                    osEventFlagsSet(g_appController.lampEvent, CN_LAMP_EVENT_SETSTATUS);
                }
            }
            startSecondInDay = IOT_ScheduleGetStartTime();
            endSecondInDay = startSecondInDay + IOT_ScheduleGetDurationTime();
            IOT_ScheduleGetCommand(&(lampStatus.status), &(lampStatus.dutyCycle));
        }
        printf("[%s|%d] \n", __func__, __LINE__);
        if (g_appController.timerFlag == 0) {
            RaiseLog(LOG_LEVEL_INFO,"start:%d end:%d cur:%d",startSecondInDay, endSecondInDay,\
                g_appController.curSecondsInDay);
            if ((g_appController.curSecondsInDay  >= startSecondInDay) && \
                (g_appController.curSecondsInDay < endSecondInDay)) {    // the schedule time has coming
                lastLampStatus.status = g_appController.mainLedStatus;
                lastLampStatus.dutyCycle = g_appController.pwmLedDutyCycle;
                g_appController.mainLedStatus = lampStatus.status;
                g_appController.pwmLedDutyCycle = lampStatus.dutyCycle;

                if (g_appController.mainLedStatus != lastLampStatus.status) {
                    RaiseLog(LOG_LEVEL_INFO, "Triggering");
                    osEventFlagsSet(g_appController.lampEvent, CN_LAMP_EVENT_SETSTATUS);
                }
                g_appController.executeTime = endSecondInDay - g_appController.curSecondsInDay;
                g_appController.timerFlag = 1;
            }
        } else {
            RaiseLog(LOG_LEVEL_INFO,"Shedule works finished:start:%d end:%d cur:%d excutetimeLeft:%d",\
                startSecondInDay, endSecondInDay, g_appController.curSecondsInDay, g_appController.executeTime);
            if (g_appController.executeTime == 0) {
                g_appController.pwmLedDutyCycle = lampStatus.dutyCycle;
                if (g_appController.mainLedStatus != lastLampStatus.status) {
                    g_appController.mainLedStatus = lastLampStatus.status;
                    osEventFlagsSet(g_appController.lampEvent, CN_LAMP_EVENT_SETSTATUS);
                }
                IOT_ScheduleSetUpdate(1);
                startSecondInDay = 0;
                endSecondInDay = 0;
                g_appController.timerFlag = 0;
            }
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
    CommandParamSetLamp setLampParam;
    CommandParamSetDutyCycle setDutyCycleParam;
    CLOUD_CommandType cmd = (CLOUD_CommandType)command;

    /**
     * Here, we only process the command for controlling the motor issued by the Cloud Platform, and the
     * value should be CN_BOARD_SWITCH_ON or CN_BOARD_SWITCH_OFF
     */
    if (cmd == CLOUD_COMMAND_SETLED) {
        setLampParam = *(CommandParamSetLamp *)buf;
        g_appController.mainLedStatus = setLampParam.status;
        g_appController.pwmLedDutyCycle = setLampParam.dutyCycle;
        g_appController.lightManual = true;
        RaiseLog(LOG_LEVEL_INFO, "setled:%d\r\n", setLampParam.status);
        osEventFlagsSet(g_appController.lampEvent, CN_LAMP_EVENT_SETSTATUS);
        return 0;

    } else if (cmd == CLOUD_COMMAND_SETDUTCYCLE) {
        setDutyCycleParam = *(CommandParamSetDutyCycle *)buf;
        if ((setDutyCycleParam.dutyCycle >=  CONFIG_LED_DUTYMINIMUM) && \
            (setDutyCycleParam.dutyCycle <= CONFIG_LED_DUTYMAXMUN)) {
            g_appController.pwmLedDutyCycle = setDutyCycleParam.dutyCycle;
            RaiseLog(LOG_LEVEL_INFO, "setduty:%d\r\n", setDutyCycleParam.dutyCycle);
            osEventFlagsSet(g_appController.lampEvent, CN_LAMP_EVENT_SETSTATUS);
            return 0;
        } else {
            RaiseLog(LOG_LEVEL_WARN, "Received illegal dutyCycle:%d\r\n", setDutyCycleParam.dutyCycle);
            return -1;
        }

    } else if (cmd == CLOUD_COMMAND_SETSHEDULE) {
        setSheduleParam = *(CommandParamSetShedule *)buf;
        RaiseLog(LOG_LEVEL_INFO, "setshedule:day:%d hour:%d minute:%d duration:%d \r\n", \
            setSheduleParam.day,setSheduleParam.startHour, setSheduleParam.startMinute, setSheduleParam.duration);
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

    if (UtilsGetValue(PWD_KEY, pwd, pd_size) < 0) {
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
    int ret;

    g_appController.pwmLedDutyCycle = CONFIG_LED_DUTYCYCLEDEFAULT;
    E53SC1_InitMainLed();
    E53SC1_InitLightIntensity();
    BOARD_InitPwmLed();
    BOARD_InitWifi();

    IOT_ScheduleInit();
    ret = Board_IsButtonPressedF2();
    osDelay(100);
    Board_InitButtonF1(BoardLedButtonCallbackF1, NULL);
    Board_InitButtonF2(BoardLedButtonCallbackF2, NULL);

    // if any netconfiguration
    LedFlashFrequencySet(CONFIG_FLASHLED_FRENETCONFIG);

    nfcInfo.deviceID = "6128c7b60ad1ed0286680f19_Lamp01";
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
        osDelay(100);
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

    g_appController.timeID = osTimerNew(&TimeCalcTimerHander, osTimerPeriodic, NULL, NULL);
    osTimerStart(g_appController.timeID, TIMER_TICK_NUM);

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

    attr.name = "LedShedule";
    if (osThreadNew((osThreadFunc_t)LampShedule, NULL, (const osThreadAttr_t *)&attr) == NULL) {
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
    RaiseLog(LOG_LEVEL_INFO, "DATA:%s Time:%s \r\n", __FUNCTION__, __DATE__, __TIME__);
    memset(&g_appController, 0x00, sizeof(g_appController));
    g_appController.lampEvent = osEventFlagsNew(NULL);
    if (g_appController.lampEvent == NULL) {
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
