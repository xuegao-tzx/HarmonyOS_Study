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
#include "iot_pwm.h"
#include "hi_pwm.h"

#define GPIO0 0
#define GPIO1 1
#define GPIO9 9
#define GPIO10 10
#define GPIOFUNC 0
#define PWM_FREQ_FREQUENCY  (60000)

void gpio_control (unsigned int  gpio, unsigned int  func, IotGpioDir dir, IotGpioValue value) {
    hi_io_set_func(gpio, func);
    IoTGpioSetDir(gpio, dir);
    IoTGpioSetOutputVal(gpio, value);
}

void car_backward(void) {
    gpio_control(GPIO0, GPIOFUNC, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE0);
    gpio_control(GPIO1, GPIOFUNC, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
    gpio_control(GPIO9, GPIOFUNC, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE0);
    gpio_control(GPIO10, GPIOFUNC, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
}

void car_forward(void) {
    gpio_control(GPIO0, GPIOFUNC, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
    gpio_control(GPIO1, GPIOFUNC, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE0);
    gpio_control(GPIO9, GPIOFUNC, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
    gpio_control(GPIO10, GPIOFUNC, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE0);
}

void car_left(void) {
    gpio_control(GPIO0, GPIOFUNC, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE0);
    gpio_control(GPIO1, GPIOFUNC, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE0);
    gpio_control(GPIO9, GPIOFUNC, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
    gpio_control(GPIO10, GPIOFUNC, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE0);
}

void car_right(void) {
    gpio_control(GPIO0, GPIOFUNC, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
    gpio_control(GPIO1, GPIOFUNC, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE0);
    gpio_control(GPIO9, GPIOFUNC, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE0);
    gpio_control(GPIO10, GPIOFUNC, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE0);
}

void car_stop(void) {
    gpio_control(GPIO0, GPIOFUNC, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
    gpio_control(GPIO1, GPIOFUNC, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
    gpio_control(GPIO9, GPIOFUNC, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
    gpio_control(GPIO10, GPIOFUNC, IOT_GPIO_DIR_OUT, IOT_GPIO_VALUE1);
}
