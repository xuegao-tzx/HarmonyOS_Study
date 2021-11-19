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

#include "iot_boardbutton.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include "iot_gpio_ex.h"
#include "ohos_init.h"

typedef enum {
    BUTTON_ID_F1 = 0,
    BUTTON_ID_F2,
    BUTTON_ID_MAX,
}ButtonID;

typedef struct {
    ButtonPressedCallback callback;
    char *args;
}ButtonCallBackController;

typedef struct {
    ButtonCallBackController controller[BUTTON_ID_MAX];
}ButtonController;
static ButtonController g_buttonController;



#define BUTTON_F1_GPIO 11
#define BUTTON_F2_GPIO 12

/**
 * @brief Callback for F1 key
 *
 */
static void F1Pressed(char* arg)
{
    (void)arg;
    printf("%s:pressed \r\n", __FUNCTION__);
    if (g_buttonController.controller[BUTTON_ID_F1].callback != NULL) {
        g_buttonController.controller[BUTTON_ID_F1].callback(arg);
    }
}

/**
 * @brief Callback for F2 key
 *
 */
static void F2Pressed(char* arg)
{
    printf("%s:pressed \r\n", __FUNCTION__);
    if (g_buttonController.controller[BUTTON_ID_F2].callback != NULL) {
        g_buttonController.controller[BUTTON_ID_F2].callback(arg);
    }
}

int Board_IsButtonPressedF2(void)
{
    int value = 1;
    int tmcnt = 10;

    IoTGpioInit(BUTTON_F2_GPIO);
    IoTGpioSetFunc(BUTTON_F2_GPIO, IOT_GPIO_FUNC_GPIO_12_GPIO);
    IoTGpioSetDir(BUTTON_F2_GPIO, IOT_GPIO_DIR_IN);

    while (--tmcnt > 0) {
        osDelay(20);
        if (IoTGpioGetInputVal(BUTTON_F2_GPIO, &value) != 0) {
            printf("[ERROR][%s|%d] IoTGpioGetInputVal failed! \n", __func__, __LINE__);
            break;
        }
        if (value) {
            break;
        }
    }

    return value ? 0 : 1;
}

int Board_InitButtonF1(ButtonPressedCallback callback, char *arg)
{
    g_buttonController.controller[BUTTON_ID_F1].callback = callback;
    g_buttonController.controller[BUTTON_ID_F1].args = arg;
    // init gpio of F1 key and set it as the falling edge to trigger interrupt
    IoTGpioInit(BUTTON_F1_GPIO);
    IoTGpioSetFunc(BUTTON_F1_GPIO, IOT_GPIO_FUNC_GPIO_11_GPIO);
    IoTGpioSetDir(BUTTON_F1_GPIO, IOT_GPIO_DIR_IN);
    IoTGpioSetPull(BUTTON_F1_GPIO, IOT_GPIO_PULL_UP);
    IoTGpioRegisterIsrFunc(BUTTON_F1_GPIO, IOT_INT_TYPE_EDGE, IOT_GPIO_EDGE_FALL_LEVEL_LOW, F1Pressed, NULL);

    return 0;
}

int Board_InitButtonF2(ButtonPressedCallback callback, char *arg)
{
    g_buttonController.controller[BUTTON_ID_F2].callback = callback;
    g_buttonController.controller[BUTTON_ID_F2].args = arg;
    // init gpio of F2 key and set it as the falling edge to trigger interrupt
    IoTGpioInit(BUTTON_F2_GPIO);
    IoTGpioSetFunc(BUTTON_F2_GPIO, IOT_GPIO_FUNC_GPIO_12_GPIO);
    IoTGpioSetDir(BUTTON_F2_GPIO, IOT_GPIO_DIR_IN);
    IoTGpioSetPull(BUTTON_F2_GPIO, IOT_GPIO_PULL_UP);
    IoTGpioRegisterIsrFunc(BUTTON_F2_GPIO, IOT_INT_TYPE_EDGE, IOT_GPIO_EDGE_FALL_LEVEL_LOW, F2Pressed, NULL);

    return 0;
}





