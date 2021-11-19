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

#ifndef  __BUTTON_H__
#define  __BUTTON_H__

typedef enum {
    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
}KEY_CODE;

typedef struct {
    int keycode;
    int keyvalue;
}KeyEvent;

typedef enum {
    ADC_USR_MIN = 5,
    ADC_USR_MAX = 228,
    ADC_S1_MIN,
    ADC_S1_MAX  = 512,
    ADC_S2_MIN,
    ADC_S2_MAX  = 854
}AdcValue;

typedef enum {
    SSU_NONE,
    SSU_USER,
    SSU_S1,
    SSU_S2
}KeyCode;

typedef void (*KeyEventCallback)(KeyEvent *event);

/**
 * @brief Setting the param of button gpio.
 *
 * @since 1.0
 * @version 1.0
 */
void ButtonInit(void);

int IsPressUserButton(void);

/**
 * @brief Register the buttion interrupt.
 *
 * @param eventCall
 * @since 1.0
 * @version 1.0
 */
void RegButtonIrq(KeyEventCallback eventCall);

#endif // __BUTTON_H__