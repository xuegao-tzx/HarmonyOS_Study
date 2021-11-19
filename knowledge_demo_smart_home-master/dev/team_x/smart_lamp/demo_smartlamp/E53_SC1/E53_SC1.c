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

#include "E53_SC1.h"
#include "cmsis_os2.h"
#include "iot_pwm.h"
#include "ohos_init.h"
#include "iot_errno.h"
#include "iot_gpio.h"
#include "iot_gpio_ex.h"
#include "iot_i2c.h"
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define CN_MAIN_LED_FRE  4000
#define CN_MAIN_LED_GPIO_FUNC IOT_GPIO_FUNC_GPIO_7_PWM0_OUT
#define CN_MAIN_LED_GPIO  7
#define CN_MAIN_LED_PWDOUT 0

int E53SC1_InitMainLed(void)
{
    IoTGpioInit(CN_MAIN_LED_GPIO);
    IoTGpioSetFunc(CN_MAIN_LED_GPIO, CN_MAIN_LED_GPIO_FUNC);
    IoTGpioSetDir(CN_MAIN_LED_GPIO, IOT_GPIO_DIR_OUT);
    IoTPwmInit(CN_MAIN_LED_PWDOUT);
    return 0;
}


int E53SC1_SetMainLed(int status, int dutyCycle)
{
    int ret = 0;
    if (status == CN_BOARD_SWITCH_ON) {
        ret = IoTPwmStart(CN_MAIN_LED_PWDOUT, dutyCycle, CN_MAIN_LED_FRE);
    } else {
        ret = IoTPwmStop(CN_MAIN_LED_PWDOUT);
    }

    RaiseLog(LOG_LEVEL_INFO, "status=%d, dutyCycle=%d, ret = %d", status, dutyCycle, ret);

    return 0;
}


#define WIFI_IOT_IO_FUNC_GPIO_0_I2C1_SDA 6
#define WIFI_IOT_IO_FUNC_GPIO_1_I2C1_SCL 6
#define WIFI_IOT_IO_FUNC_GPIO_7_GPIO 0
#define WIFI_IOT_I2C_IDX_1 1

#define BH1750_ADDR 0x23


static void E53SC1IoInit(void)
{
    IoTGpioInit(0);
    IoTGpioSetFunc(0, WIFI_IOT_IO_FUNC_GPIO_0_I2C1_SDA); // GPIO_0复用为I2C1_SDA
    IoTGpioInit(1);
    IoTGpioSetFunc(1, WIFI_IOT_IO_FUNC_GPIO_1_I2C1_SCL); // GPIO_1复用为I2C1_SCL
    IoTI2cInit(WIFI_IOT_I2C_IDX_1, 400000);              /* baudrate: 400kbps */
}

static int InitBH1750(void)
{
    int ret;
    uint8_t send_data[1] = {0x01};
    ret = IoTI2cWrite(WIFI_IOT_I2C_IDX_1, (BH1750_ADDR << 1) | 0x00, send_data, 1);
    if (ret != 0) {
        printf("===== Error: I2C write ret = 0x%x! =====\r\n", ret);
        return -1;
    }
    return 0;
}

static int StartBH1750(void)
{
    int ret;
    uint8_t send_data[1] = {0x10};
    ret = IoTI2cWrite(WIFI_IOT_I2C_IDX_1, (BH1750_ADDR << 1) | 0x00, send_data, 1);
    if (ret != 0) {
        printf("===== Error: I2C write ret = 0x%x! =====\r\n", ret);
        return -1;
    }
    return 0;
}

int E53SC1_InitLightIntensity(void)
{
    int ret;
    E53SC1IoInit();
    ret = InitBH1750();
    if (ret != 0) {
        return -1;
    }
    return 0;
}

int E53SC1_GetLightIntensity(int *lightIntensity)
{
    int ret;
    float intensity;
    ret = StartBH1750();
    if (ret != 0) {
        printf("Start BH1750 failed!\r\n");
        return -1;
    }
    usleep(180000);
    uint8_t recv_data[2] = {0};
    ret = IoTI2cRead(WIFI_IOT_I2C_IDX_1, (BH1750_ADDR << 1) | 0x01, recv_data, 2);
    if (ret != 0) {
        return -1;
    }
    intensity = (float)(((recv_data[0] << 8) + recv_data[1]) / 1.2);
    *lightIntensity = (int) intensity;

    return 0;
}


