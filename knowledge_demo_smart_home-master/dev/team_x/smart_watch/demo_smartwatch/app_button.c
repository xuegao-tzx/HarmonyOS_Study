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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include "iot_gpio_ex.h"

#include "app_gsm.h"


#define BUTTON_F2_GPIO 12
#define GPIO_PIN_NUMBER 12

BUTTON_STS  F2_flag;

typedef struct
{
    int  connected;
    int  led;
    int   key2;
}app_cb_t;

osEventFlagsId_t evt_button_id; // event flags id
app_cb_t  g_app_cb;


static bool g_isKey2Pressed;
static bool g_isKey2LongPressed;

bool button_GetKey2Sts(void)
{
    return g_isKey2Pressed;
}

bool button_GetKey2LongPressSts(void)
{
    return g_isKey2LongPressed;
}


/**************************************************
 * 任务：F2_Pressed
 * F2按键回调
 * ***********************************************/
static void F2_Pressed(char *arg)
{
    (void)arg;
    F2_flag = BUTTON_ON;
    osEventFlagsSet(evt_button_id, 0x00000002U); 
}


/**************************************************
 * 任务：button_init
 * 按键硬件初始化
 * ***********************************************/
void button_init(void)
{
    F2_flag = BUTTON_OFF;

    //初始化F2按键，设置为下降沿触发中断
    IoTGpioInit(GPIO_PIN_NUMBER);
    IoTGpioSetFunc(GPIO_PIN_NUMBER, IOT_GPIO_FUNC_GPIO_12_GPIO);
    IoTGpioSetDir(GPIO_PIN_NUMBER, IOT_GPIO_DIR_IN);
    IoTGpioSetPull(GPIO_PIN_NUMBER, IOT_GPIO_PULL_UP);

    osDelay(50);
}


/**************************************************
 * 任务：button_task_entry
 * 按键处理任务
 * ***********************************************/
int button_task_entry(void)
{
    int value = 1;
    g_isKey2Pressed = false;
    g_isKey2LongPressed = false;

    F2_flag = BUTTON_OFF;
    /* 按键硬件初始化 */
    button_init();

    /* 创建按键事件 */
    evt_button_id = osEventFlagsNew(NULL);
    if (evt_button_id == NULL)
    {
        printf("Falied to create EventFlags!\n");
    }
     printf("button_task_entry\n");
     static count = 0;
     static lastCount = 0;

    /* 等待按键事件 */
    while (1)
    {
        osDelay(10);

        IoTGpioGetInputVal(BUTTON_F2_GPIO, &value);

        if (value == 0) {
            count++;
            lastCount = 0;
        }

        if (value == 1) {
            lastCount = count;
            count = 0;
        }
        
        if ( lastCount > 10){
            if( g_isKey2LongPressed == BUTTON_OFF)
            {
                g_isKey2LongPressed = BUTTON_ON;// 开   
                printf("\nF2 LONG BUTTON_ON\n");
            }
            else
            {
                g_isKey2LongPressed = BUTTON_OFF;// 关
                printf("\nF2 LONG BUTTON_OFF\n");
            }
        } else if ( lastCount > 0 && lastCount < 20){

            if ( g_isKey2Pressed == BUTTON_OFF)
            {
                g_isKey2Pressed = BUTTON_ON;// 开   
                printf("\nF2 BUTTON_ON\n");
            } else {
                g_isKey2Pressed = BUTTON_OFF;// 关
                printf("\nF2 BUTTON_OFF\n");
            }
        }
    }
    return 0;
}
