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

#include <unistd.h>
#include "iot_gpio.h"
#include "iot_errno.h"
#include "cmsis_os2.h"
#include "base64.h"
#include "ohos_types.h"

#include "peripheral_hal.h"
#include "pwm.h"
#include "defines.h"

static uint8 g_level = PWM_LEVEL_1;


static void *PwmProcess(const char *arg)
{
    (void)arg;
    int delayTimes = 0;

    IoTGpioInit(WIFI_IOT_GPIO_IDX_2);
    HalIoSetFunc(HAL_WIFI_IOT_IO_NAME_GPIO_2, IOT_IO_FUNC_GPIO);
    IoTGpioSetDir(WIFI_IOT_GPIO_IDX_2, IOT_GPIO_DIR_OUT);

    while (1) {
        if (g_level == PWM_LEVEL_0) {           // when the PWM is set close
            IoTGpioSetOutputVal(WIFI_IOT_GPIO_IDX_2, GPIO_PIN_LOW);
            osDelay(PWM_DELAY_TIME);
            continue;
        }

        if (g_level >= PWM_LEVEL_NBR)
            g_level = PWM_LEVEL_1;

        IoTGpioSetOutputVal(WIFI_IOT_GPIO_IDX_2, GPIO_PIN_HIGH);
        delayTimes = PWM_DELAY_TIME / (PWM_LEVEL_NBR - g_level);
        osDelay(delayTimes);

        IoTGpioSetOutputVal(WIFI_IOT_GPIO_IDX_2, GPIO_PIN_LOW);
        delayTimes = PWM_DELAY_TIME / g_level;
        osDelay(delayTimes);
    }
}


int PwmInit(void)
{
    osThreadAttr_t attr;
    attr.name = "PwmProcess";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = PWM_TASK_STACK_SIZE;
    attr.priority = PWM_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)PwmProcess, NULL, &attr) == NULL) {
        PWM_ERR("Falied to create NanCfgNetTask!\n");
        return -1;
    }

    return 0;
}

void PwmSetLevel(PWM_LEVEL level)
{
    if (level >= 0 && level < PWM_LEVEL_NBR)
        g_level = level;
}
