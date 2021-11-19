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

#include <hi_adc.h>
#include <hi_time.h>
#include <hi_wifi_api.h>
#include "control.h"
#include "ohos_types.h"
#include "log.h"
#include "oled.h"
#include "iot_gpio.h"
#include "iot_errno.h"
#include "cmsis_os2.h"

#include "peripheral_hal.h"
#include "utils_hal.h"

#define TRASH_DEMO_TASK_STACK_SIZE (1024*4)
#define TRASH_DEMO_TASK_PRIOPITY   (26)
#define ADC_TEST_LENGTH            (20)
#define VLT_MIN (100)
#define SMART_CAN_SLEEP_2500MS  2500
#define Y_SHOW_SMART_CAN_STATUS 6
#define Y_SHOW_SMART_CAN_SHOW_OPEN 4
#define THE_PARM_OF_IR_SENSOR 3.1
#define THE_MAX_PARM_OF_DISTANCE_SENSOR 2.0
#define THE_MIN_PARM_OF_DISTANCE_SENSOR 1.2
#define THE_PARM_OF_CACL_CYCLE 20
#define THE_CYCLES_OF_PWM_CONTROL 20000
#define THE_ANGLE_OF_TRASHCAN_WROK 1000

uint32 g_num = 0;
bool   g_trashcanIsOpen = FALSE;
float  g_Capacityvalue = 0;
hi_u16  g_adc_buf_trash[ADC_TEST_LENGTH] = { 0 };

void GpioInitOperation(HalWifiIotIoName name, uint8 val, IotGpioDir dir, IotGpioValue value)
{
    IoTGpioInit(name);
    HalIoSetFunc(name, val);
    IoTGpioSetDir(name, dir);
    if (IOT_GPIO_DIR_OUT == dir) {
        IoTGpioSetOutputVal(name, value);
    }
}

float get_adc_value(HalWifiIotAdcChannelIndex channel)
{
    uint32 ret = 0;
    hi_u16 data = 0;
    hi_u16 vlt = 0;
    float voltage = 0;
    float vlt_max = 0;
    float vlt_min = VLT_MIN;

    for(uint32 i = 0; i < ADC_TEST_LENGTH; i++) {
        ret = HalAdcRead(channel, &data, HAL_WIFI_IOT_ADC_EQU_MODEL_4, HAL_WIFI_IOT_ADC_CUR_BAIS_DEFAULT, 0xF0);
        if (ret = IOT_SUCCESS) {
            printf("HalAdcRead failed");
        }

        g_adc_buf_trash[i] = data;
    }

    for (uint32 i = 0; i < ADC_TEST_LENGTH; i++){
        vlt = g_adc_buf_trash[i];
        voltage = (float)vlt * 1.8 * 4 / 4096.0;
        vlt_max = (voltage > vlt_max) ? voltage : vlt_max;
        vlt_min = (voltage < vlt_min) ? voltage : vlt_min;
    }
    return vlt_max;
}

bool is_someone_there(void)
{
    float value = get_adc_value(HAL_WIFI_IOT_ADC_CHANNEL_3);
    if (value > THE_PARM_OF_IR_SENSOR) {
        return TRUE;
    }
    return FALSE;
}

bool is_night_time(void)
{
    float value = get_adc_value(HAL_WIFI_IOT_ADC_CHANNEL_4);
    if (value > THE_PARM_OF_IR_SENSOR) {
        return TRUE;
    }
    return FALSE;
}

bool is_trash_can_full(void)
{
    g_Capacityvalue = get_adc_value(HAL_WIFI_IOT_ADC_CHANNEL_5);
    if(g_Capacityvalue < THE_MAX_PARM_OF_DISTANCE_SENSOR && g_Capacityvalue >= THE_MIN_PARM_OF_DISTANCE_SENSOR) {
        printf("trash can is full");
        return TRUE;
    }
    printf("trash can is not full");
    return FALSE;
}

/* 设置舵机旋转的角度 */
void set_engine_angle(hi_s32 duty)
{
    uint32 count = duty/THE_PARM_OF_CACL_CYCLE;
    for (int i = 1; i <= count; i++) {
        IoTGpioSetOutputVal(HAL_WIFI_IOT_IO_NAME_GPIO_10, IOT_GPIO_VALUE1);
        hal_udelay(duty - i*THE_PARM_OF_CACL_CYCLE);
        IoTGpioSetOutputVal(HAL_WIFI_IOT_IO_NAME_GPIO_10, IOT_GPIO_VALUE0);
        hal_udelay(THE_CYCLES_OF_PWM_CONTROL - (duty - i*THE_PARM_OF_CACL_CYCLE));
    }
}

void set_engine_angle_reversal(hi_s32 duty)
{
    uint32 count = duty/THE_PARM_OF_CACL_CYCLE;
    for (int i = 1; i <= count; i++) {
        IoTGpioSetOutputVal(HAL_WIFI_IOT_IO_NAME_GPIO_10, IOT_GPIO_VALUE1);
        hal_udelay(i*THE_PARM_OF_CACL_CYCLE);
        IoTGpioSetOutputVal(HAL_WIFI_IOT_IO_NAME_GPIO_10, IOT_GPIO_VALUE0);
        hal_udelay(THE_CYCLES_OF_PWM_CONTROL - i*THE_PARM_OF_CACL_CYCLE);
    }
}

void trash_can_close(void)
{
    printf("trash_can close\n");
    set_engine_angle_reversal(THE_ANGLE_OF_TRASHCAN_WROK);
    g_trashcanIsOpen = FALSE;
}

void trash_can_open(void)
{
    printf("trash_can open\n");
    set_engine_angle(THE_ANGLE_OF_TRASHCAN_WROK);
    g_trashcanIsOpen = TRUE;
}

int CalcCapacityUsedPercentage(int* capacityUsed)
{
    printf("Current g_Capacityvalue = %f \n",  g_Capacityvalue);

    if ((g_Capacityvalue < 2.0) && (g_Capacityvalue > 1.2))  { // 使用率 100%
        *capacityUsed = 100;
        return 0;
    } else if ((g_Capacityvalue < 0.7) && (g_Capacityvalue > 3.2)) { // 抛弃无意义的数值读取
        printf("the value is invalid\n");
        return -1;
    }
    *capacityUsed = ((2.5 - g_Capacityvalue) / 0.6) * 100; // 计算公式
    if ((capacityUsed < 0) || (capacityUsed > 100)) {
        printf("the value is invalid\n");
        return -1;
    }
    return 0;
}

void SmartTrashcanInit(void)
{
    // 舵机初始化 GPIO10
    GpioInitOperation(HAL_WIFI_IOT_IO_NAME_GPIO_10, 0, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE0);
    // 绿色LED灯初始化 GPIO8
    GpioInitOperation(HAL_WIFI_IOT_IO_NAME_GPIO_8, 0, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE0);
}

void TrashcanDistanceDetection(void)
{
    if (is_someone_there() == TRUE) {
        printf("[test] someone here \n");
        if (is_night_time() == TRUE) {
            printf("is night\n");
            HalIoSetFunc(HAL_WIFI_IOT_IO_NAME_GPIO_8, IOT_GPIO_VALUE1);
        }
        if (g_trashcanIsOpen == FALSE) {
            printf("===open===\n");
            OledShowStr(0, Y_SHOW_SMART_CAN_SHOW_OPEN, "           ", 1);
            OledShowStr(0, Y_SHOW_SMART_CAN_SHOW_OPEN, "  Can:open ", 1);
            trash_can_open();
        }
    } else if (is_someone_there() == FALSE) {
        /* 关灯 */
        IoTGpioSetOutputVal(HAL_WIFI_IOT_IO_NAME_GPIO_8, IOT_GPIO_VALUE0);
        /* 关闭垃圾盖 */
        if (g_trashcanIsOpen == TRUE) {
            printf("===close===\n");
            OledShowStr(0, Y_SHOW_SMART_CAN_SHOW_OPEN, "           ", 1);
            OledShowStr(0, Y_SHOW_SMART_CAN_SHOW_OPEN, "  Can:close", 1);
            trash_can_close();
        }
    }
}

void TrashcanStatusReporting(void)
{
    if ((g_trashcanIsOpen == FALSE) && (is_trash_can_full() == TRUE)) {
        printf("publish success : please deal with it in time \n");
        OledShowStr(0, Y_SHOW_SMART_CAN_STATUS, "              ", 1);
        OledShowStr(0, Y_SHOW_SMART_CAN_STATUS, "  Can: Is Full", 1);
    }
    if ((g_trashcanIsOpen == FALSE) && (is_trash_can_full() == FALSE)) {
        printf("publish success : the trash can is not full \n");
        OledShowStr(0, Y_SHOW_SMART_CAN_STATUS, "              ", 1);
        OledShowStr(0, Y_SHOW_SMART_CAN_STATUS, "  Can:Not Full", 1);
    }
}

void *SmartTrashcanTask(void *param)
{
    CommonDeviceInit(); // Init OLED device
    SmartTrashcanInit();
    while (1) {
        TrashcanDistanceDetection();

        TrashcanStatusReporting();
        hi_sleep(SMART_CAN_SLEEP_2500MS);
    }
}

void SmartTrashcanTaskEntry(void)
{
    osThreadAttr_t attr = {0};
    attr.name = (char*)"trashcanTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TRASH_DEMO_TASK_STACK_SIZE;
    attr.priority = TRASH_DEMO_TASK_PRIOPITY;
    if (osThreadNew((osThreadFunc_t)SmartTrashcanTask, NULL, &attr) == NULL) {
        printf("Failed to create trashcan test !\n");
    }
}