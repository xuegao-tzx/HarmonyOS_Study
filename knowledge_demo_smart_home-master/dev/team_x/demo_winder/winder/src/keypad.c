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

#include "iot_gpio.h"
#include "iot_errno.h"

#include "peripheral_hal.h"
#include "keypad.h"
#include "defines.h"

#include "ohos_types.h"

enum {
    ADC_S0_MIN = 5,
    ADC_S0_MAX = 228,
    ADC_S1_MIN,
    ADC_S1_MAX  = 512,
    ADC_S2_MIN,
    ADC_S2_MAX  = 854
};

static KEY_CODE KeyPadGetKey(void)
{
    unsigned short data = 0;
    if (HalAdcRead(ADC_CHANNEL_2, &data) == 0) {
        if ((ADC_S0_MIN <= data) && (data <= ADC_S0_MAX))  return KEY_PAD_MODE;
        if ((ADC_S1_MIN <= data) && (data <= ADC_S1_MAX))  return KEY_PAD_PLUS;
        if ((ADC_S2_MIN <= data) && (data <= ADC_S2_MAX))  return KEY_PAD_MINUS;
    }

    return KEY_PAD_NONE;
}

static void *KeyEventTask(const char *args)
{
    uint8 keycode = KEY_PAD_NONE;
    keypad_event_call g_event_call = (keypad_event_call)args;
    if (g_event_call == NULL) {
        KEY_ERR("no key event call! \n");
    }
    while (1) {
        KEY_CODE key = KeyPadGetKey();
        if (keycode == key) {
            osDelay(KEY_DELAY_TIME);
            continue;
        }
        keycode = key;
        if (keycode == KEY_PAD_NONE) {
            osDelay(KEY_DELAY_TIME);
            continue;
        }

        if (g_event_call != NULL) {
            KeyEvent e;
            e.code = keycode;
            e.value = 1;
            g_event_call(&e);
        }
    }

    return NULL;
}

int KeyRegisterInit(keypad_event_call gcall)
{
    osThreadAttr_t attr;
    attr.name = "KeyEventTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = KEY_TASK_STACK_SIZE;
    attr.priority = KEY_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)KeyEventTask, gcall, &attr) == NULL) {
        KEY_ERR("Falied to create KeyEventTask! \n");
    }

    return 0;
}
