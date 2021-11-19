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

#include "log.h"
#include "oled.h"
#include "common_init.h"
#include "peripheral_hal.h"
#include "iot_pwm.h"
#include "iot_demo_def.h"

#define I2C_IDX_BAUDRATE                  (400000)

static void I2cBusInit(void)
{
    IoTGpioInit(HAL_WIFI_IOT_IO_NAME_GPIO_13);
    HalIoSetFunc(HAL_WIFI_IOT_IO_NAME_GPIO_13, WIFI_IOT_IO_FUNC_GPIO_13_I2C0_SDA); // Set up the gpio funcion as i2c bus
    IoTGpioInit(HAL_WIFI_IOT_IO_NAME_GPIO_14);
    HalIoSetFunc(HAL_WIFI_IOT_IO_NAME_GPIO_14, WIFI_IOT_IO_FUNC_GPIO_14_I2C0_SCL);
    IoTI2cInit(WIFI_IOT_OLED_I2C_IDX_0, I2C_IDX_BAUDRATE); // Rate: 400kbps
    LOG_I("I2C0 bus init success !\n");
}

void CommonDeviceInit(void)
{
    I2cBusInit();
    while (IOT_SUCCESS != OledInit()) {
        LOG_E("Connecting oled board falied! Please access oled board !\n");
        usleep(SLEEP_100_MS); // refer from the datasheet of SSD1306
    }
    LOG_I("Oled  init success !\n");
}

#define DEVICE_STATUS_LED_GPIO      9
#define IOT_GPIO_FUNC_GPIO_9_GPIO   0
#define CONFIG_TASK_MAIN_STACKSIZE 0x800                 // main task stacksize must be bigger
#define CONFIG_TASK_MAIN_PRIOR 22                        // default task priority
#define CN_LED_FLASH_HIGHBASE  200
#define CN_LED_FLASH_CYCLEBASE 1000
#define CONFIG_FLASHLED_FREDEFAULT 5

int BOARD_InitIoLed(void)
{
    // 设置GPIO_9的复用功能为普通GPIO
    HalIoSetFunc(DEVICE_STATUS_LED_GPIO, IOT_GPIO_FUNC_GPIO_9_GPIO);
    // 设置GPIO_9为输出模式
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

int HisparkLedFlashFrequencySet(float flashHz)
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
    HisparkLedFlashFrequencySet(CONFIG_FLASHLED_FREDEFAULT);
    while (1) {
        BOARD_SetIoLedStatus(1);
        osDelay((uint32_t)g_ledFlashController.cycleLightTicks);
        BOARD_SetIoLedStatus(0);
        osDelay((uint32_t)(g_ledFlashController.cycleTicks - g_ledFlashController.cycleLightTicks));
    }
    return;
}

static void LedFlashTaskInit(void)
{
    osThreadAttr_t attr;
    RaiseLog(LOG_LEVEL_INFO,"DATA:%s Time:%s \r\n", __DATE__, __TIME__);
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
