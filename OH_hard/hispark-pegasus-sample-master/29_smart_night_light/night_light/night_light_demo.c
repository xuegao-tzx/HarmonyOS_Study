/*
 * Copyright (c) 2020, HiHope Community.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include "hi_io.h"
#include "hi_adc.h"
#include "iot_pwm.h"
#include "iot_errno.h"
#include "hi_gpio.h"
#include "night_light_param.h"
#include "config_params.h"

#define HUMAN_SENSOR_CHAN_NAME  3
#define LIGHT_SENSOR_CHAN_NAME  4

#define RED_LED_PIN_NAME        10
#define GREEN_LED_PIN_NAME      11
#define BLUE_LED_PIN_NAME       12

#define LED_PWM_FUNCTION        5
#define IOT_PWM_PORT_PWM1       1
#define IOT_PWM_PORT_PWM2       2
#define IOT_PWM_PORT_PWM3       3

#define HUMAN_CHECK_LEVEL       1800
#define LIGHT_CHECK_LEVEL       1800

#define PWM_FREQ_DIVITION       64000

unsigned int g_lightStatus = 0;
unsigned int g_lightness = LIGHTNESS_HIGHT_DUTY;
unsigned int g_ledDelayTime = LIGHTDELAYLONG;
unsigned int g_statusChanged = 0;

#define LIGHTNESSLEN 32
#define TIMELEN 32
char lightness[LIGHTNESSLEN] = {0};
char lighttime[TIMELEN] = {0};

int GetValue(void) {
    int ret = UtilsGetValue(PARAM_LIGHTNESS, lightness, LIGHTNESSLEN);
    if (ret < 0) {
        printf("get lightness value failed, ret is %d\r\n", ret);
    }
    else {
        g_lightness = atoi(lightness);
        printf("g_lightness is %d\r\n", g_lightness);
    }

    ret = UtilsGetValue(PARAM_LIGHT_TIME, lighttime, TIMELEN);
    if (ret < 0) {
        printf("get lightness value failed, ret is %d\r\n", ret);
    }
    else {
        g_ledDelayTime = atoi(lighttime);
        printf("g_ledDelayTime is %d\r\n", g_ledDelayTime);
    }

    return ret;
}

static void CorlorfulLightTask(void *arg)
{
    (void)arg;
    hi_io_set_func(RED_LED_PIN_NAME, LED_PWM_FUNCTION);
    hi_io_set_func(GREEN_LED_PIN_NAME, LED_PWM_FUNCTION);
    hi_io_set_func(BLUE_LED_PIN_NAME, LED_PWM_FUNCTION);

    IoTPwmInit(IOT_PWM_PORT_PWM1); // R
    IoTPwmInit(IOT_PWM_PORT_PWM2); // G
    IoTPwmInit(IOT_PWM_PORT_PWM3); // B

    while (1) {
        osDelay(10); 
        unsigned short data = 0;       
        if (hi_adc_read(LIGHT_SENSOR_CHAN_NAME, &data, HI_ADC_EQU_MODEL_4, HI_ADC_CUR_BAIS_DEFAULT, 0)
            == IOT_SUCCESS) {
            if (data > HUMAN_CHECK_LEVEL) {
                unsigned short data_human = 0;
                if (hi_adc_read(HUMAN_SENSOR_CHAN_NAME, &data_human, HI_ADC_EQU_MODEL_4, HI_ADC_CUR_BAIS_DEFAULT, 0)
                        == IOT_SUCCESS) {          
                    if (data_human > LIGHT_CHECK_LEVEL) {
                        IoTPwmStart(IOT_PWM_PORT_PWM1, g_lightness, PWM_FREQ_DIVITION);
                        IoTPwmStart(IOT_PWM_PORT_PWM2, g_lightness, PWM_FREQ_DIVITION);
                        IoTPwmStart(IOT_PWM_PORT_PWM3, g_lightness, PWM_FREQ_DIVITION);
                        osDelay(g_ledDelayTime);
                        IoTPwmStop(IOT_PWM_PORT_PWM1);
                        IoTPwmStop(IOT_PWM_PORT_PWM2);
                        IoTPwmStop(IOT_PWM_PORT_PWM3);
                    }      
                }
            }
        }
    }

}

static void ColorfulLightDemo(void)
{
    osThreadAttr_t attr;

    IoTWatchDogDisable();

    IoTGpioInit(RED_LED_PIN_NAME);
    IoTGpioInit(GREEN_LED_PIN_NAME);
    IoTGpioInit(BLUE_LED_PIN_NAME);

    // set Red/Green/Blue LED pin as output
    IoTGpioSetDir(RED_LED_PIN_NAME, IOT_GPIO_DIR_OUT);
    IoTGpioSetDir(GREEN_LED_PIN_NAME, IOT_GPIO_DIR_OUT);
    IoTGpioSetDir(BLUE_LED_PIN_NAME, IOT_GPIO_DIR_OUT);

    attr.name = "CorlorfulLightTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 4096;
    attr.priority = osPriorityNormal;

    if (osThreadNew(CorlorfulLightTask, NULL, &attr) == NULL) {
        printf("[ColorfulLightDemo] Falied to create CorlorfulLightTask!\n");
    }
}

APP_FEATURE_INIT(ColorfulLightDemo);
