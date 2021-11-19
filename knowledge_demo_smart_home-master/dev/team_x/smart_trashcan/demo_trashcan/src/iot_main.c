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
#include "iot_boardbutton.h"
#include "iot_nfc.h"
#include "iot_wifi.h"
#include "iot_netcfg.h"
#include "iot_sntp.h"
#include "common_init.h"
#include "ohos_init.h"
#include "oled.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <cmsis_os2.h>
#include <stdbool.h>
#include "kv_store.h"
#include "iot_schedule.h"
#include "iot_netcfg_nan.h"
#include "network_config_service.h"

#include "hi_reset.h"

#define    SID_KEY        "ssid"
#define    PWD_KEY        "password"

#define    SOFTAP_NAME    "teamX-Trashcan01"

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

typedef struct {
    int capacityUsed;
}ApplicationController;

static ApplicationController g_appController;

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

int32_t g_timecount = 0;

void TimeCalcTimerHander(void *args)
{
    (void)args;
    g_timecount--;
}

#define TIMER_TICK_NUM 100
#define WARNING_INTERVAL 10*60 // 10*60 (units:second)

/**
 * @brief Sensor data collection task entry and the main flow is as follows:
 *        1, Wait the for the event to trigger in specified time
 *        2, Collect Sensor Data (air humidity/temperature, soil moisture, etc.)
 *        3, Report the collected data to the Cloud Platform
 */
static void DataCollectAndReport(const void *arg)
{
    (void)arg;
    int ret = -1;
    uint32_t flag = 0;
    osTimerId_t timeID;
    timeID = osTimerNew(&TimeCalcTimerHander, osTimerPeriodic, NULL, NULL);

    while (1) {

        ret = CalcCapacityUsedPercentage(&g_appController.capacityUsed);
        if(ret != 0) {
            RaiseLog(LOG_LEVEL_ERR, "Get the sensor data error");
            osDelay(Time2Tick(CONFIG_SENSOR_SAMPLE_CYCLE));
            continue;
        }
        if (g_appController.capacityUsed  >= 95) {
            if ((g_timecount == 0) && (flag == 0)) {
               g_timecount = WARNING_INTERVAL;
               CLOUD_ReportNotification(NOTIFY_TYPE_URGENT, "Trashcan Full", "垃圾桶已满");
               osTimerStart(timeID, TIMER_TICK_NUM);
               flag = 1;
            } else if ((g_timecount == 0) && (flag != 0)) {
                g_timecount = WARNING_INTERVAL;
                CLOUD_ReportNotification(NOTIFY_TYPE_URGENT, "Trashcan Full", "垃圾桶已满");
            }
        } else if (flag == 1 ) {
            g_timecount = 0;
            osTimerStop(timeID);
            flag = 0;
        }
        (void) IotProfile_Report(g_appController.capacityUsed);

        RaiseLog(LOG_LEVEL_INFO, "Get the capacityUsed value :%d \n", g_appController.capacityUsed);
        osDelay(Time2Tick(CONFIG_SENSOR_SAMPLE_CYCLE));
    }
    return;
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

    BOARD_InitWifi();

    ret = IsPressUserButton();
    osDelay(100);

    // if any netconfiguration botton press
    HisparkLedFlashFrequencySet(CONFIG_FLASHLED_FRENETCONFIG);

    nfcInfo.deviceID = "614456cff74139027d2230ec_Trashcan01";
    nfcInfo.devicePWD = "12345678";

    if (ret) {
        RaiseLog(LOG_LEVEL_INFO, "Netconfig Button has pressed! \n");
        if(BOARD_NAN_NetCfgStartConfig(SOFTAP_NAME, ssid, sizeof(ssid), pwd, sizeof(pwd)) < 0) {
            RaiseLog(LOG_LEVEL_ERR, "BOARD_NetCfgStartConfig failed! \n");
            return;
        } else {
            ret = AFTER_NETCFG_ACTION;
        }
    } else {
        ret = IotWifiInfo_get(ssid, sizeof(ssid), pwd, sizeof(pwd));
        if (ret == 0) {
            if(BOARD_NAN_NetCfgStartConfig(SOFTAP_NAME, ssid, sizeof(ssid), pwd, sizeof(pwd)) < 0) {
                RaiseLog(LOG_LEVEL_ERR, "BOARD_NetCfgStartConfig failed! \n");
                return;
            } else {
                ret = AFTER_NETCFG_ACTION;
            }
        }
    }

    HisparkLedFlashFrequencySet(CONFIG_FLASHLED_FREWIFI);

    printf("Device init success \n");
    SmartTrashcanTaskEntry();

    if (BOARD_ConnectWifi(ssid, pwd) != 0) {
        RaiseLog(LOG_LEVEL_ERR, "BOARD_ConnectWifi failed! \n");
        if (ret == AFTER_NETCFG_ACTION) {
            NotifyNetCfgResult(NETCFG_DEV_INFO_INVALID);
        }
        hi_hard_reboot(HI_SYS_REBOOT_CAUSE_CMD);
        return;
    }

    OledShowStr(0, 0, " IOT_CONNECT_OK", CHAR_SIZE_OF_LANTTICE_8_16);

    if (ret == AFTER_NETCFG_ACTION) {
        RaiseLog(LOG_LEVEL_DEBUG, "Connect wifi success ! \n");
        NotifyNetCfgResult(NETCFG_OK);
        osDelay(100);
        RaiseLog(LOG_LEVEL_DEBUG, "StopNetCfg wifi success ! \n");
        StopNetCfg();
        IotWifiInfo_set(ssid, pwd);
    }

    // update the RTC time and connect to the cloud
    HisparkLedFlashFrequencySet(CONFIG_FLASHLED_FRECLOUD);
    if (CLOUD_Init() != 0) {
        return;
    }

    if (CLOUD_Connect(nfcInfo.deviceID, nfcInfo.devicePWD, \
        CONFIG_CLOUD_DEFAULT_SERVERIP, CONFIG_CLOUD_DEFAULT_SERVERPORT) != 0) {
        return;
    }

    HisparkLedFlashFrequencySet(CONFIG_FLASHLED_WORKSWELL);
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

    return;
}

/**
 * @brief IoT Main Entry of the IoT-Flower project
 */
static void IotMainEntry(void)
{
    osThreadAttr_t attr;
    RaiseLog(LOG_LEVEL_INFO, "DATA:%s Time:%s \r\n", __FUNCTION__, __DATE__, __TIME__);
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
