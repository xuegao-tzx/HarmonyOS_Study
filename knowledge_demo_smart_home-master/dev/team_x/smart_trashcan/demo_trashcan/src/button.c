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
#include "button.h"
#include "iot_gpio.h"
#include "iot_errno.h"
#include "peripheral_hal.h"

#define  WIFI_IOT_GPIO_IDX_5         5
#define  IOT_IO_FUNC_GPIO_5_GPIO     0

typedef struct {
    KeyEventCallback mEventCall;
    int mButtonID;
}KettleButton;

static KettleButton g_button;
void ButtonIrqCallback(char *param)
{
    LOG_D("Debug for gpio5_irq_callback\n");
    KettleButton *button = (KettleButton *)param;
    if (button != NULL && button->mEventCall != NULL) {
        KeyEvent e;
        e.keycode = button->mButtonID;
        e.keyvalue = 1;
        button->mEventCall(&e);
    }
}

int IsPressUserButton(void)
{
    int ret = SSU_NONE;
    unsigned short data = 0;
    ButtonInit();
    if (HalAdcRead(HAL_WIFI_IOT_ADC_CHANNEL_2, &data, HAL_WIFI_IOT_ADC_EQU_MODEL_4, HAL_WIFI_IOT_ADC_CUR_BAIS_DEFAULT, 0) == 0) {
        if ((ADC_USR_MIN <= data) && (data <= ADC_USR_MAX))  ret = SSU_USER;
        if ((ADC_S1_MIN <= data) && (data <= ADC_S1_MAX))  ret = SSU_S1;
        if ((ADC_S2_MIN <= data) && (data <= ADC_S2_MAX))  ret = SSU_S2;
    }
    
    if (SSU_USER == ret) {
        return 1;   
    } else {
        return 0;
    }
}

void ButtonInit(void)
{
    IoTGpioInit(WIFI_IOT_GPIO_IDX_5);
    HalIoSetFunc(HAL_WIFI_IOT_IO_NAME_GPIO_5, IOT_IO_FUNC_GPIO_5_GPIO); // Set the gpio5 function
    IoTGpioSetDir(WIFI_IOT_GPIO_IDX_5, IOT_GPIO_DIR_IN); // Set gpio dir: in/out
}

void RegButtonIrq(KeyEventCallback eventCall)
{
    g_button.mEventCall = eventCall;
    g_button.mButtonID = KEY_5;
    if (IoTGpioRegisterIsrFunc(WIFI_IOT_GPIO_IDX_5, IOT_INT_TYPE_EDGE, IOT_GPIO_EDGE_FALL_LEVEL_LOW,
        &ButtonIrqCallback, (char *)(&g_button)) != IOT_SUCCESS) {
        LOG_E("Register gpio5 isr failed !\n");
    }
}

