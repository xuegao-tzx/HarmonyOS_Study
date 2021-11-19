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

#include <hi_timer.h>
#include <hi_task.h>

#include <stdlib.h> 
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include "hi_io.h"
#include "hi_time.h"
#include "robot_control.h"
#include "hi_adc.h"
#include "iot_errno.h"
#include "iot_cloud.h"
#include "iot_profile.h"

#define GPIO_0 0
#define GPIO_1 1
#define GPIO_9 9
#define GPIO_10 10

#define GPIO11 11
#define GPIO12 12

#define GPIO_0_PWM3 5
#define GPIO_1_PWM4 5
#define GPIO_9_PWM0 5
#define GPIO_10_PWM1 5
#define PWM0 0
#define PWM1 1
#define PWM3 3
#define PWM4 4

#define     PWM_DUTY_LEFT_RIGHT               (5000)
#define     MAX_SPEED                         (100)
#define     MIN_SPEED                         (30000)

#define LEFT 1
#define RIGHT 2
#define ALL 3
hi_u16 clean_step=1;
hi_u32  g_car_speed = 1000;

/*correct car speed*/
hi_void correct_car_speed(hi_void)
{
    if (g_car_speed < MAX_SPEED) {
        g_car_speed = MAX_SPEED;
    }

    if (g_car_speed > MIN_SPEED) {
        g_car_speed = MIN_SPEED;
    }
}


hi_void Clean_car_turn_right(hi_u16 time)
{
    correct_car_speed();
    gpio_control(HI_IO_NAME_GPIO_0, GPIO_0, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1, HI_IO_FUNC_GPIO_0_GPIO);
    PWMControl(HI_IO_NAME_GPIO_1,HI_IO_FUNC_GPIO_1_PWM4_OUT, PWM4, 20);
    PWMControl(HI_IO_NAME_GPIO_9,HI_IO_FUNC_GPIO_9_PWM0_OUT,PWM0,20);
    gpio_control(HI_IO_NAME_GPIO_10, GPIO_10, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1, HI_IO_FUNC_GPIO_10_GPIO);

    hi_sleep(time);
    car_stop();
}

hi_void Clean_car_turn_left(hi_u16 time)
{
    correct_car_speed();
    PWMControl(HI_IO_NAME_GPIO_0,HI_IO_FUNC_GPIO_0_PWM3_OUT,PWM3, 20); 
    gpio_control(HI_IO_NAME_GPIO_1, GPIO_1, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1, HI_IO_FUNC_GPIO_1_GPIO);
    gpio_control(HI_IO_NAME_GPIO_9, GPIO_9, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1, HI_IO_FUNC_GPIO_9_GPIO);
    PWMControl(HI_IO_NAME_GPIO_10,HI_IO_FUNC_GPIO_10_PWM1_OUT,PWM1, 20);
    hi_sleep(time);
    car_stop();
}

hi_void short_route(hi_u16 time)
{
    car_go_forward();
    hi_sleep(time);
    car_stop();
}

void AvoidObj(void)
{
    float l_distance = 0.0;//left wall distance
    float r_distance = 0.0;

    car_stop();
    car_go_back();
    hi_sleep(500);
    car_stop();//meet wall

    engine_turn_left();
    hi_sleep(280);
    GetDistance1(& l_distance );
    hi_sleep(28);//get left distance

    engine_turn_right();
    hi_sleep(280);
    GetDistance1(& r_distance);
    hi_sleep(28);//get right distance

    regress_middle();

    if(l_distance > r_distance)
    {
        srand((unsigned)(time(NULL)));
        Clean_car_turn_left(rand()%301+300);//rand会随机产生一个很大数，对501求余，即返回一个0到500的值随机数，
    }
    else
    {
        Clean_car_turn_right(rand()%301+300);
    }

}
hi_void HangHandle(hi_u16 direction)
{
    switch(direction)
    {
        case LEFT:
            GpioControl(GPIO_0, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE0);
            GpioControl(GPIO_1, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE0);
            PWMControl(GPIO_9, GPIO_9_PWM0, PWM0, 5);
            GpioControl(GPIO_10, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
            hi_sleep(108);
        break;

        case RIGHT:
            PWMControl(GPIO_0, GPIO_0_PWM3, PWM3, 5);
            GpioControl(GPIO_1, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
            GpioControl(GPIO_9, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE0);
            GpioControl(GPIO_10, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE0);
            hi_sleep(108);
        break;

        case ALL:
            AvoidObj();
        break;

        default:
        break;
    }
    
}
hi_void  clean_module(hi_void)
{
    float m_distance = 0.0;
    float l_distance = 0.0;//left wall distance
    float r_distance = 0.0;
    static hi_u16 count=0;
    IotGpioValue io_status_left;
    IotGpioValue io_status_right;

    IoTGpioGetInputVal(GPIO11,&io_status_left);
    IoTGpioGetInputVal(GPIO12,&io_status_right);

    GetDistance1(& m_distance );
   
    switch (clean_step)
    {
        case 1:
           
            if(io_status_left == 0 || io_status_right == 0)
            {
                if(io_status_left == 0 && io_status_right != 0)
                {
                    car_stop();
                    car_go_back();
                    hi_sleep(318);
                    car_stop();//meet wall
                     RaiseLog(LOG_LEVEL_DEBUG,"io_status_left \n");
                    HangHandle(LEFT);
                    clean_step=1;
                }
                else if(io_status_left != 0 && io_status_right == 0)
                {
                    car_stop();
                    car_go_back();
                    hi_sleep(318);
                    car_stop();//meet wall
                    RaiseLog(LOG_LEVEL_DEBUG,"io_status_right \n");
                    HangHandle(RIGHT);
                    clean_step=1;
                }
                else 
                {
                    AvoidObj();
                }
                
            }
            else if(m_distance <= 18.0)
            {
                clean_step=2;
                RaiseLog(LOG_LEVEL_DEBUG,"clean_step=2 \n");
            }
            else
            {
                car_go_forward_speed(5);
                RaiseLog(LOG_LEVEL_DEBUG,"car_go_forward_speed5 \n");
            }
            break;

        case 2://turn left or turn right

            if(m_distance <= 18.0)
            {
                AvoidObj();
                clean_step=1;
            }
            else
            {
                clean_step=1;
            }
            break;

        case 3:
            RaiseLog(LOG_LEVEL_DEBUG,"io_status_left \n");
            HangHandle(LEFT);
            clean_step=1;
            break;

        case 4:
            RaiseLog(LOG_LEVEL_DEBUG,"io_status_right \n");
            HangHandle(RIGHT);
            clean_step=1;
            break;

        case 5:
           RaiseLog(LOG_LEVEL_DEBUG,"io_status_right \n");
            HangHandle(ALL);
            clean_step=1;
            break;

        default:
            break;                
    }
}

