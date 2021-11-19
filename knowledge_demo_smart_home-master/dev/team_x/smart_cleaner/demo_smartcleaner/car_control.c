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

#include "iot_gpio.h"
#include "iot_errno.h"
#include "iot_pwm.h"
#include "hi_pwm.h"

#define GPIO_0 0
#define GPIO_1 1
#define GPIO_9 9
#define GPIO_10 10
#define GPIO_0_PWM3 5
#define GPIO_1_PWM4 5
#define GPIO_9_PWM0 5
#define GPIO_10_PWM1 5
#define PWM0 0
#define PWM1 1
#define PWM3 3
#define PWM4 4
#define CLK_160M 160000000

void gpio_control (unsigned int  gpio, unsigned int  func, IotGpioDir dir, IotGpioValue value) {
    hi_io_set_func(gpio, func);
    IoTGpioSetDir(gpio, dir);
    IoTGpioSetOutputVal(gpio, value);
}


void GpioControl(unsigned int id, IotGpioDir dir, IotGpioValue val) 
{
    hi_io_set_func(id, 0);
    IoTGpioInit(id);
    IoTGpioSetDir(id, dir);
    IoTGpioSetOutputVal(id, val);
}
void PWMControl(unsigned int id, unsigned char ioFunc, unsigned int port, unsigned short duty)
{
    hi_io_set_func(id, ioFunc);
    IoTPwmInit(port);
    IoTPwmStart(port, duty, (unsigned int)CLK_160M/6000);
}



/*car go forward */
void car_go_forward(void)
{
    GpioControl(GPIO_0, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
    PWMControl(GPIO_1, GPIO_1_PWM4, PWM4, 5);
    GpioControl(GPIO_9, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
    PWMControl(GPIO_10, GPIO_10_PWM1, PWM1, 5);
}
/*car go forward */
void car_go_forward_speed(hi_u16 speed)
{
    GpioControl(GPIO_0, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
    PWMControl(GPIO_1, GPIO_1_PWM4, PWM4, speed);
    GpioControl(GPIO_9, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
    PWMControl(GPIO_10, GPIO_10_PWM1, PWM1, speed);
}

/*car stop */
void car_stop(void)
{
    GpioControl(GPIO_0, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE0);
    GpioControl(GPIO_1, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE0);
    GpioControl(GPIO_9, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE0);
    GpioControl(GPIO_10, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE0);
}

/*car turn left */
void car_trace_right(void)
{
    
    GpioControl(GPIO_0, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
    PWMControl(GPIO_1, GPIO_1_PWM4, PWM4, 10);
    GpioControl(GPIO_9, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
    PWMControl(GPIO_10, GPIO_10_PWM1, PWM1, 15);
}

/*car turn right */
void car_trace_left(void)
{
    
    GpioControl(GPIO_0, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
    PWMControl(GPIO_1, GPIO_1_PWM4, PWM4, 15);
    GpioControl(GPIO_9, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
    PWMControl(GPIO_10, GPIO_10_PWM1, PWM1, 10);
}

/*car turn right */
void car_back_right(void)
{
   
    PWMControl(GPIO_0, GPIO_0_PWM3, PWM3, 15);
    GpioControl(GPIO_1, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
    PWMControl(GPIO_9, GPIO_9_PWM0, PWM0, 10);
    GpioControl(GPIO_10, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
}

void car_back_left(void)
{
    
    PWMControl(GPIO_0, GPIO_0_PWM3, PWM3, 10);
    GpioControl(GPIO_1, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
    PWMControl(GPIO_9, GPIO_9_PWM0, PWM0, 15);
    GpioControl(GPIO_10, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
}

// /*car turn right */
void car_turn_left(void)
{
    GpioControl(GPIO_0, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
    PWMControl(GPIO_1, GPIO_1_PWM4, PWM4, 10);
    PWMControl(GPIO_9, GPIO_9_PWM0, PWM0, 10);
    GpioControl(GPIO_10, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE0);
}

// /*car go back */
void car_go_back(void)
{
    PWMControl(GPIO_0, GPIO_0_PWM3, PWM3, 18);
    GpioControl(GPIO_1, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
    PWMControl(GPIO_9, GPIO_9_PWM0, PWM0, 18);
    GpioControl(GPIO_10, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
}

// /*car turn left */
void car_turn_right(void)
{
    PWMControl(GPIO_0, GPIO_0_PWM3, PWM3, 10);
    GpioControl(GPIO_1, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE0);
    GpioControl(GPIO_9, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
    PWMControl(GPIO_10, GPIO_10_PWM1, PWM1, 10);
}

void car_left(void)
{
    car_turn_left();
}
void car_right(void)
{
    car_turn_right();
}
void car_forward(void)
{
    car_go_forward();
}
void car_backward(void)
{
    car_go_back();
}




