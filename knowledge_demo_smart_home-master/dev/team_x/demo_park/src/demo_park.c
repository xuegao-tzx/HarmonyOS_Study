
/*
 * Copyright (c) 2021 KaiHong Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "ohos_types.h"
#include "iot_errno.h"
#include "iot_gpio.h"
#include "lwip/sockets.h"
#include "oc_mqtt.h"
#include "oc_iot_cloud.h"
#include "iot_adc.h"
#include "oc_mqtt.h"
#include "iot_wifi.h"

#define MSGQUEUE_OBJECTS 16  // number of Message Queue Objects
#define IOT_IO_NAME_GPIO_5 5 // for oled button A
#define IOT_IO_NAME_GPIO_9 9 // light
#define ADC_CHANNEL_3 3
#define ADC_EQU_MODEL_4 2
#define ADC_RESOLUTION 4096
#define PWM_FREQ_DIVITION 64000
#define LIGHT_SENSOR_CHAN_NAME IOT_ADC_CHANNEL_4

#define MAX_LEN 20

typedef struct
{ // object data type
    char *Buf;
    uint8_t Idx;
} MSGQUEUE_OBJ_t;

MSGQUEUE_OBJ_t msg;
osMessageQueueId_t mid_MsgQueue; // message queue id

#define CLIENT_ID "6125e00d0ad1ed0286647065_test20210825111111111_0_0_2021082506"
#define USERNAME "6125e00d0ad1ed0286647065_test20210825111111111"
#define PASSWORD "80c5b63880924315863d80f84035895cb0d3a1074fd2a4e1cb4aba1855b4cc88"

//判断是否有车
static int isPark(void)
{
    uint8 park = 0, i;
    uint16 light_flag[MAX_LEN] = {0};
    int lavg = 0;

    for (i = 0; i < MAX_LEN; i++)
    {
        uint16 duty = 0;
        uint16 data = 0;
        if (IoTAdcRead(LIGHT_SENSOR_CHAN_NAME, &data, IOT_ADC_EQU_MODEL_4, IOT_ADC_CUR_BAIS_DEFAULT, 0) == IOT_SUCCESS)
        {
            //duty = PWM_FREQ_DIVITION * (unsigned int)data / ADC_RESOLUTION;
            osDelay(10);
        }
        light_flag[i] = data;
    }
    uint16 it = 0;

    for (i = 0 ; i < MAX_LEN; i++)
    {
        if (light_flag[i] > 1000)
        {
            it += light_flag[i];
        }
    }

    if (it/MAX_LEN >1000)
    {
        return it/MAX_LEN;
    }

    return 0;
}
static void deal_report_msg(report_t *report)
{
    oc_mqtt_profile_service_t service;
    oc_mqtt_profile_kv_t isPark;

    service.event_time = NULL;
    service.service_id = "TestPark";
    service.service_property = &isPark;
    service.nxt = NULL;

    isPark.key = "isPark";
    isPark.value = &report->isPark;
    isPark.type = EN_OC_MQTT_PROFILE_VALUE_INT;
    isPark.nxt = NULL;

    oc_mqtt_profile_propertyreport(USERNAME, &service);
    return;
}

void oc_cmd_rsp_cb(uint8_t *recv_data, size_t recv_size, uint8_t **resp_data, size_t *resp_size)
{
    app_msg_t *app_msg;

    int ret = 0;
    app_msg = malloc(sizeof(app_msg_t));
    app_msg->msg_type = en_msg_cmd;
    app_msg->msg.cmd.payload = (char *)recv_data;

    printf("recv data is %.*s\n", recv_size, recv_data);
    ret = osMessageQueuePut(mid_MsgQueue, &app_msg, 0U, 0U);
    if (ret != 0)
    {
        free(recv_data);
    }
    *resp_data = NULL;
    *resp_size = 0;
}

///< COMMAND DEAL
#include <cJSON.h>

static int task_main_entry(void)
{
    app_msg_t *app_msg;

    uint32 ret = BOARD_ConnectWifi("Test", "11111111");
    device_info_init(CLIENT_ID, USERNAME, PASSWORD);
    oc_mqtt_init();
    oc_set_cmd_rsp_cb(oc_cmd_rsp_cb);

    while (1)
    {
        app_msg = NULL;
        (void)osMessageQueueGet(mid_MsgQueue, (void **)&app_msg, NULL, 0U);
        if (NULL != app_msg)
        {
            switch (app_msg->msg_type)
            {
            case en_msg_report:
                deal_report_msg(&app_msg->msg.report);
                break;
            default:
                break;
            }
            free(app_msg);
        }
        osDelay(20);
    }
    return 0;
}

static int task_sensor_entry(void)
{
    app_msg_t *app_msg;
    int park =0;

    osDelay(400);
    while (1)
    {
        app_msg = malloc(sizeof(app_msg_t));
        if (NULL != app_msg)
        {
            app_msg->msg_type = en_msg_report;
            park = isPark();
            app_msg->msg.report.isPark = park;
            if (0 != osMessageQueuePut(mid_MsgQueue, &app_msg, 0U, 0U))
            {
                free(app_msg);
            }
        }
        osDelay(20*100);
    }
    return 0;
}

static void OC_Demo(void)
{

    IoTGpioInit(IOT_IO_NAME_GPIO_9);
    IoTGpioSetDir(IOT_IO_NAME_GPIO_9, IOT_GPIO_DIR_IN);

    mid_MsgQueue = osMessageQueueNew(MSGQUEUE_OBJECTS, 10, NULL);
    if (mid_MsgQueue == NULL)
    {
        printf("Falied to create Message Queue!\n");
    }

    osThreadAttr_t attr;

    attr.name = "task_main_entry";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 10240;
    attr.priority = 24;

    if (osThreadNew((osThreadFunc_t)task_main_entry, NULL, &attr) == NULL)
    {
        printf("Falied to create task_main_entry!\n");
    }
    attr.stack_size = 2048;
    attr.priority = 25;
    attr.name = "task_sensor_entry";
    if (osThreadNew((osThreadFunc_t)task_sensor_entry, NULL, &attr) == NULL)
    {
        printf("Falied to create task_sensor_entry!\n");
    }
}

APP_FEATURE_INIT(OC_Demo);