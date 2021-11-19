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

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include "hi_io.h"
#include "hi_time.h"
#include "iot_watchdog.h"
#include "robot_control.h"
#include "iot_errno.h"
#include "hi_pwm.h"
#include "hi_timer.h"
#include "iot_pwm.h"

#define GPIO0 0
#define GPIO1 1
#define GPIO9 9
#define GPIO10 10
#define GPIO11 11
#define GPIO12 12
#define GPIO_FUNC 0
#define car_speed_left 0
#define car_speed_right 0
extern unsigned char g_car_status;

unsigned int g_car_speed_left = car_speed_left;
unsigned int g_car_speed_right = car_speed_right;
IotGpioValue io_status_left;
IotGpioValue io_status_right;
void timer1_callback(unsigned int arg)
{
    IotGpioValue io_status;
    if(g_car_speed_left != car_speed_left)   
    {
        IoTGpioGetInputVal(GPIO11,&io_status);
        if(io_status != IOT_GPIO_VALUE0){
            g_car_speed_left = car_speed_left;
            printf("left speed change \r\n");
        }
    }

    if(g_car_speed_right != car_speed_right)   
    {
        IoTGpioGetInputVal(GPIO12,&io_status);
        if(io_status != IOT_GPIO_VALUE0){
            g_car_speed_right = car_speed_right;
            printf("right speed change \r\n");
        }
    }
    if(g_car_speed_left != car_speed_left && g_car_speed_right != car_speed_right)
    {
        g_car_speed_left = car_speed_left;
        g_car_speed_right = car_speed_right;
    }
    IoTGpioGetInputVal(GPIO11,&io_status_left);
    IoTGpioGetInputVal(GPIO12,&io_status_right);
    if(io_status_right == IOT_GPIO_VALUE0 && io_status_left != IOT_GPIO_VALUE0)
    {
        g_car_speed_left = car_speed_left;
        g_car_speed_right = 1;
    } 
    if(io_status_right != IOT_GPIO_VALUE0 && io_status_left == IOT_GPIO_VALUE0)
    {
        g_car_speed_left = 1;
        g_car_speed_right = car_speed_right;
    }
}

void trace_module(void)
{    
    unsigned int timer_id1;
    hi_timer_create(&timer_id1);
    hi_timer_start(timer_id1, HI_TIMER_TYPE_PERIOD, 1, timer1_callback, 0);
    gpio_control(GPIO0, GPIO_FUNC, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
    gpio_control(GPIO1, GPIO_FUNC, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE0);
    gpio_control(GPIO9, GPIO_FUNC, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
    gpio_control(GPIO10, GPIO_FUNC, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE0);
    
    while (1) {
        gpio_control(GPIO1, 0, IOT_GPIO_DIR_OUT, g_car_speed_left);
        gpio_control(GPIO10, 0, IOT_GPIO_DIR_OUT, g_car_speed_right);
        hi_udelay(20);

        if (g_car_status != CAR_TRACE_STATUS) {
            break;
        }
    }
    hi_timer_delete(timer_id1);
}