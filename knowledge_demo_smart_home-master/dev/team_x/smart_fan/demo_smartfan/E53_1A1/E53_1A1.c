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

#include "E53_1A1.h"
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

#define FAN_SPEED_LEVEL_1 1
#define FAN_SPEED_LEVEL_2 2
#define FAN_SPEED_LEVEL_3 3
#define FAN_SPEED_LEVEL_4 4

#define CURTAIN_MOTOR_GPIO_IDX 8
#define WIFI_IOT_IO_FUNC_GPIO_8_PWM1_OUT 5
#define CN_MAIN_FAN_PWM1OUT 1
#define CN_MAIN_FAN_FRE  4000


void E531A1_FanInit(void)
{
    IoTGpioInit(CURTAIN_MOTOR_GPIO_IDX);
    IoTGpioSetFunc(CURTAIN_MOTOR_GPIO_IDX, WIFI_IOT_IO_FUNC_GPIO_8_PWM1_OUT);
    IoTGpioSetDir(CURTAIN_MOTOR_GPIO_IDX, IOT_GPIO_DIR_OUT); // 设置GPIO_8为输出模式
    IoTPwmInit(CN_MAIN_FAN_PWM1OUT);
    return;
}

int E531A1_SetFanStatus(int status, int speedLevel)
{
    int ret = 0;
    int pwmDutyCycle = 0;
    if (status == CN_BOARD_SWITCH_ON) {
        switch (speedLevel) {
            case FAN_SPEED_LEVEL_1:
                pwmDutyCycle = 10;
                break;
            case FAN_SPEED_LEVEL_2:
                pwmDutyCycle = 40;
                break;
            case FAN_SPEED_LEVEL_3:
                pwmDutyCycle = 70;
                break;
            case FAN_SPEED_LEVEL_4:
                pwmDutyCycle = 99;
                break;
            default:
                break;
        }
        ret = IoTPwmStart(CN_MAIN_FAN_PWM1OUT, pwmDutyCycle, CN_MAIN_FAN_FRE);
    } else {
        ret = IoTPwmStop(CN_MAIN_FAN_PWM1OUT);
    }

    RaiseLog(LOG_LEVEL_INFO, "status=%d, speedLevel=%d, ret = %d", status, speedLevel, ret);

    return 0;
}