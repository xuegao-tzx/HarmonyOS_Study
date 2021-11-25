#include <string.h>
#include <stdio.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include "iot_i2c.h"
#include "iot_errno.h"

#include "ssd1306.h"
#include "night_light_param.h"
#include "config_params.h"

#define     OLED_I2C_BAUDRATE   400*1000
#define     IOT_I2C_IDX_0          0
extern unsigned int g_lightStatus;
extern unsigned int g_lightness;
extern unsigned int g_ledDelayTime;
extern unsigned int g_statusChanged;
unsigned short g_wifiStatus = WIFI_AP;

void OledUpdata(void) {
    ssd1306_Fill(Black);
        
    ssd1306_SetCursor(10, 0);
    ssd1306_DrawString("Night Lignt", Font_11x18, White);

    ssd1306_SetCursor(10, 25);
    ssd1306_DrawString("Lightness:", Font_6x8, White);

    ssd1306_SetCursor(10, 40);
    ssd1306_DrawString("DelayTime:", Font_6x8, White);
        
    if (g_lightness <= LIGHTNESS_WEAK_DUTY) {
        ssd1306_SetCursor(75, 25);
        ssd1306_DrawString("weak", Font_6x8, White);
    }
    else if (g_lightness <= LIGHTNESS_MID_DUTY && g_lightness > LIGHTNESS_WEAK_DUTY) {
        ssd1306_SetCursor(75, 25);
        ssd1306_DrawString("mid", Font_6x8, White);
    }
    else {
        ssd1306_SetCursor(75, 25);
        ssd1306_DrawString("high", Font_6x8, White);
    }

    if (g_ledDelayTime <= LIGHTDELAYSHORT) {
        ssd1306_SetCursor(75, 40);
        ssd1306_DrawString("short", Font_6x8, White);
    }        
    else if (g_ledDelayTime <= LIGHTDELAYMID && g_ledDelayTime > LIGHTDELAYSHORT) {
        ssd1306_SetCursor(75, 40);
        ssd1306_DrawString("mid", Font_6x8, White);
    }
    else {
        ssd1306_SetCursor(75, 40);
        ssd1306_DrawString("long", Font_6x8, White);
    }        
    ssd1306_UpdateScreen();
    g_statusChanged = 0;
}

void OledDisplay(void) {
    ssd1306_Fill(Black);
    ssd1306_SetCursor(5, 0);
    ssd1306_DrawString("OpenHarmony", Font_11x18, White);
    ssd1306_SetCursor(20, 25);
    ssd1306_DrawString("Lightness", Font_7x10, White);

    ssd1306_SetCursor(20, 40);
    ssd1306_DrawString("DelayTime", Font_7x10, White);

    ssd1306_UpdateScreen();
}

static void OledTask(void *arg)
{
    (void)arg;
    int times = 0;
    unsigned int retval = 0;

    retval = IoTGpioInit(13);
    if (retval != IOT_SUCCESS) {
        printf("IoTGpioInit(13) failed, %0X!\n", retval);
        return retval;
    }    
    retval = IoTGpioInit(14);
    if (retval != IOT_SUCCESS) {
        printf("IoTGpioInit(14) failed, %0X!\n", retval);
        return retval;
    }
    retval = hi_io_set_func(13, 6);
    if (retval != IOT_SUCCESS) {
        printf("hi_io_set_func(13) failed, %0X!\n", retval);
        return retval;
    }
    retval = hi_io_set_func(14, 6);
    if (retval != IOT_SUCCESS) {
        printf("hi_io_set_func(14) failed, %0X!\n", retval);
        return retval;
    }     

    int ret = IoTI2cInit(IOT_I2C_IDX_0, OLED_I2C_BAUDRATE);
    ssd1306_Init();  
    OledDisplay();
    osDelay(100);
        
    while (1) {
        osDelay(10);
        if (g_wifiStatus == WIFI_STA) {
            int ret = GetValue();
            if(ret < 0) {
                printf("Get time ande lightness value failed\r\n");
            }
            printf("lightness is %d, time is %d\r\n", g_lightness, g_ledDelayTime); 
            OledUpdata();
        }
    }
}

static void OledDemo(void)
{
    osThreadAttr_t attr;

    attr.name = "OledTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 4096;
    attr.priority = osPriorityNormal;

    if (osThreadNew(OledTask, NULL, &attr) == NULL) {
        printf("[OledDemo] Falied to create OledTask!\n");
    }
}

APP_FEATURE_INIT(OledDemo);