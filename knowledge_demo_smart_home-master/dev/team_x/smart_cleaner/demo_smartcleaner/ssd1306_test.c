/*
 * Copyright (C) 2021 HiHope Open Source Organization .
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
 *
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include "hi_io.h"
#include "hi_time.h"
#include "ssd1306.h"
#include "iot_i2c.h"
#include "iot_watchdog.h"
#include "robot_control.h"
#include "iot_errno.h"

#define OLED_I2C_BAUDRATE 400*1000
#define GPIO13 13
#define GPIO14 14
#define FUNC_SDA 6
#define FUNC_SCL 6
extern unsigned char g_car_status;
extern unsigned char g_wifi_status;

void Ssd1306TestTask(void* arg)
{
    (void) arg;
    
    float m_distance = 0.0;
    char strff[50];

    hi_io_set_func(GPIO13, FUNC_SDA);
    hi_io_set_func(GPIO14, FUNC_SCL);
    IoTI2cInit(0, OLED_I2C_BAUDRATE);

    IoTWatchDogDisable();

    usleep(20*1000);
    ssd1306_Init();
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_DrawString("Hello OpenHarmony!", Font_7x10, White);

    uint32_t start = HAL_GetTick();
    ssd1306_UpdateScreen();
    uint32_t end = HAL_GetTick();
    printf("ssd1306_UpdateScreen time cost: %d ms.\r\n", end - start);
    osDelay(100);

    while (1)
    {
        printf("g_car_status is %d\r\n",g_car_status);
        if(g_wifi_status == CAR_WIFI_CONNECT_ING)
        {
            ssd1306_Fill(Black);
            ssd1306_SetCursor(10, 10);
            ssd1306_DrawString("WIFI_CONNECT_ING", Font_7x10, White);
            ssd1306_UpdateScreen();
        }
        else if(g_wifi_status == CAR_WIFI_CONNECT_OK)
        {
            ssd1306_Fill(Black);
            ssd1306_SetCursor(10, 10);
            ssd1306_DrawString("WIFI_CONNECT_OK", Font_7x10, White);
            ssd1306_UpdateScreen();
        }
         else if(g_wifi_status == CAR_IOT_CONNECT_OK) {
            ssd1306_Fill(Black);
            ssd1306_SetCursor(10, 10);
            ssd1306_DrawString("IOT_CONNECT_OK", Font_7x10, White);
            ssd1306_UpdateScreen();
        }
        osDelay(100);
    }
    
}

void Ssd1306TestDemo(void)
{
    osThreadAttr_t attr;

    attr.name = "Ssd1306Task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 10240;
    attr.priority = 25;

    if (osThreadNew(Ssd1306TestTask, NULL, &attr) == NULL) {
        printf("[Ssd1306TestDemo] Falied to create Ssd1306TestTask!\n");
    }
}
APP_FEATURE_INIT(Ssd1306TestDemo);
