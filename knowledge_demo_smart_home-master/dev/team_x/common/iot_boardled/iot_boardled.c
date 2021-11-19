/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "iot_demo_def.h"
#include "iot_boardled.h"
#include "iot_gpio.h"
#include "iot_gpio_ex.h"
#include "iot_pwm.h"
#include "ohos_init.h"
#include <cmsis_os2.h>


#define DEVICE_STATUS_LED_GPIO  2
#define DEVICE_SATTUS_LED_PWD   2
#define CONFIG_FLASHLED_FREDEFAULT 5
  
/**
 * @brief Initialize the board pwd led
 * @return Returns 0 success while others failed
 */
int BOARD_InitPwmLed(void)
{
    IoTGpioInit(DEVICE_STATUS_LED_GPIO);
    IoTGpioSetFunc(DEVICE_STATUS_LED_GPIO, IOT_GPIO_FUNC_GPIO_2_PWM2_OUT);
    IoTGpioSetDir(DEVICE_STATUS_LED_GPIO, IOT_GPIO_DIR_OUT);
    IoTPwmInit(DEVICE_SATTUS_LED_PWD);

    return 0;
}

/**
 * @brief Set Pwm led duty cycle
 * @param dutyCycle, the duty cycle to set, not max than 40000
 * @return Returns 0 success while others failed
 */
int BOARD_SetPwmLedDutyCycle(int dutyCycle)
{
    dutyCycle = dutyCycle < 0 ? 1 : dutyCycle;
    dutyCycle = dutyCycle >= 100 ? 99 : dutyCycle;
    IoTPwmStart(DEVICE_STATUS_LED_GPIO, dutyCycle, 40000);
    return 0;
}

// the led part
int BOARD_InitIoLed(void)
{
    //设置GPIO_2的复用功能为普通GPIO
    IoTGpioSetFunc(DEVICE_STATUS_LED_GPIO, IOT_GPIO_FUNC_GPIO_2_GPIO);
    //设置GPIO_2为输出模式
    IoTGpioSetDir(DEVICE_STATUS_LED_GPIO, IOT_GPIO_DIR_OUT);
    return 0;
}

int BOARD_SetIoLedStatus(int status)
{
    if (status) {
        IoTGpioSetOutputVal(DEVICE_STATUS_LED_GPIO, 1);
    } else {
        IoTGpioSetOutputVal(DEVICE_STATUS_LED_GPIO, 0);
    }
    return 0;
}


#define CONFIG_TASK_MAIN_STACKSIZE 0x800                 // main task stacksize must be bigger
#define CONFIG_TASK_MAIN_PRIOR 22                        // default task priority

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


/**
 @ brief Set the main task flash frequency
 */

typedef struct
{
    int cycleTicks;
    int cycleLightTicks;
    osThreadId_t taskID;
}LedFlashController;
static LedFlashController g_ledFlashController;

#define CN_LED_FLASH_HIGHBASE  200
#define CN_LED_FLASH_CYCLEBASE 1000
int LedFlashFrequencySet(float flashHz)
{
    int cycleHighMs = 0;
    int cycleMs = 0;

    cycleMs = (int)(CN_MINISECONDS_IN_SECOND / flashHz);
    cycleHighMs = cycleMs *CN_LED_FLASH_HIGHBASE /CN_LED_FLASH_CYCLEBASE;

    g_ledFlashController.cycleTicks = Time2Tick(cycleMs);
    g_ledFlashController.cycleLightTicks = Time2Tick(cycleHighMs);
    RaiseLog(LOG_LEVEL_INFO, "cycle:%d cycleon:%d ",g_ledFlashController.cycleTicks, g_ledFlashController.cycleLightTicks);
    return 0;
}


int LedFlashTaskDeinit(void)
{
    osThreadTerminate(g_ledFlashController.taskID);
    g_ledFlashController.taskID = NULL;
    return 0;
}

/**
 * @brief LED flashing task entry
 */
static void LedTaskEntry(const void *arg)
{
    (void)arg;
    BOARD_InitIoLed();
    LedFlashFrequencySet(CONFIG_FLASHLED_FREDEFAULT);
    while (1) {
        BOARD_SetIoLedStatus(CN_BOARD_SWITCH_ON);
        osDelay((uint32_t)g_ledFlashController.cycleLightTicks);
        BOARD_SetIoLedStatus(CN_BOARD_SWITCH_OFF);
        osDelay((uint32_t)(g_ledFlashController.cycleTicks - g_ledFlashController.cycleLightTicks));
    }
    return;
}

static void LedFlashTaskInit(void)
{
    osThreadAttr_t attr;
    RaiseLog(LOG_LEVEL_INFO, "DATA:%s Time:%s \r\n", __DATE__, __TIME__);
    // Create the IoT Main task
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = CONFIG_TASK_MAIN_STACKSIZE;
    attr.priority = CONFIG_TASK_MAIN_PRIOR;
    attr.name = "LedFlashTask";
    g_ledFlashController.taskID =  osThreadNew((osThreadFunc_t)LedTaskEntry, NULL, (const osThreadAttr_t *)&attr);
    if (g_ledFlashController.taskID == NULL) {
        RaiseLog(LOG_LEVEL_ERR, "Create the LED FLASH TASK failed");
    } else {
        RaiseLog(LOG_LEVEL_INFO, "Create the LED FLASH TASK success");
    }

    return;
}

APP_FEATURE_INIT(LedFlashTaskInit);