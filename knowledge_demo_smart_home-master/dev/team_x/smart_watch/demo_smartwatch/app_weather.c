/*
 * Copyright (c) 2020 Nanjing Xiaoxiongpai Intelligent Technology Co., Ltd.
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

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "lcd_st7789.h"
#include "lv_port_disp.h"
#include "lvgl.h"
#include "cJSON.h"
#include "lwip/sockets.h"

#define CONFIG_TASK_PRIOR           25                        /* default task priority */
#define CONFIG_TASK_DEFAULT_STACKSIZE    0x1000             /* unit:bytes */


LV_FONT_DECLARE(weather_font_15); /* 申明字体 */
LV_FONT_DECLARE(city_font_25); /* 申明字体 */
extern lv_img_dsc_t background_img;

/* APP日期显示类型定义 */
typedef struct
{
    lv_obj_t *wea;             
    lv_obj_t *wea_img; 
    lv_obj_t *cur_temp;
    lv_obj_t *high_temp;             
    lv_obj_t *low_temp; 
    lv_obj_t *air_level;
    lv_obj_t *humidity;             
    lv_obj_t *air_pm25; 
    lv_obj_t *city;
    lv_obj_t *win;
} Weather_TypeDef;

static Weather_TypeDef weather;        /* 温度显示控件 */

void weather_gui_init(void)
{
    lv_obj_t* src = lv_scr_act();
    static lv_style_t font_15_style;
    static lv_style_t font_25_style;
    
    /* 创建天气控件 */
    lv_style_copy(&font_15_style, &lv_style_transp);
    font_15_style.text.font = &weather_font_15;
    weather.wea = lv_label_create(src,NULL);
    lv_obj_set_pos(weather.wea,120,25);
    lv_label_set_style(weather.wea, 0, &font_15_style);

    /* 创建天气图标控件 */
    lv_style_copy(&font_15_style, &lv_style_transp);
    font_15_style.text.font = &weather_font_15;
    weather.wea_img = lv_label_create(src,NULL);
    lv_obj_set_pos(weather.wea_img,210,30);
    lv_label_set_style(weather.wea_img, 0, &font_15_style);

    /* 创建当前温度控件 */
    lv_style_copy(&font_15_style, &lv_style_transp);
    font_15_style.text.font = &weather_font_15;
    weather.cur_temp = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(weather.cur_temp, 120, 45);
    lv_label_set_style(weather.cur_temp, 0, &font_15_style);

    /* 创建最高温度控件 */
    lv_style_copy(&font_15_style, &lv_style_transp);
    font_15_style.text.font = &weather_font_15;
    weather.high_temp = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(weather.high_temp, 160, 25);
    lv_label_set_style(weather.high_temp, 0, &font_15_style);

    /* 创建最低温度控件 */
    lv_style_copy(&font_15_style, &lv_style_transp);
    font_15_style.text.font = &weather_font_15;
    weather.low_temp = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(weather.low_temp, 160, 45);
    lv_label_set_style(weather.low_temp , 0, &font_15_style);

    /* 创建空气质量控件 */
    lv_style_copy(&font_15_style, &lv_style_transp);
    font_15_style.text.font = &weather_font_15;
    weather.air_level= lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(weather.air_level, 120, 5);
    lv_label_set_style(weather.air_level, 0, &font_15_style);

    /* 创建湿度控件 */
    lv_style_copy(&font_15_style, &lv_style_transp);
    font_15_style.text.font = &weather_font_15;
    weather.humidity = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(weather.humidity, 5, 5);
    lv_label_set_style(weather.humidity, 0, &font_15_style);

    /* 创建PM2.5控件 */
    lv_style_copy(&font_15_style, &lv_style_transp);
    font_15_style.text.font = &weather_font_15;
    weather.air_pm25 = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(weather.air_pm25, 5, 25);
    lv_label_set_style(weather.air_pm25, 0, &font_15_style);

    /* 创建风气控件 */
    lv_style_copy(&font_15_style, &lv_style_transp);
    font_15_style.text.font = &weather_font_15;
    weather.win = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(weather.win , 5, 45);
    lv_label_set_style(weather.win , 0, &font_15_style);

    /* 创建城市控件 */
    lv_style_copy(&font_25_style, &lv_style_transp);
    font_25_style.text.font = &city_font_25;
    weather.city = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(weather.city, 20, 115);
    lv_label_set_style(weather.city, 0, &font_25_style);
}
void gui_display_wea(char *data)
{
    if (data == NULL){
        return;
    }
    lv_label_set_static_text(weather.wea,data); 
}

void gui_display_wea_img(char *data)
{

    if (strcmp("qing", data)==0) {
        lv_label_set_static_text(weather.wea_img,"\xee\x9c\x92"); 
    }
    if (strcmp("yin", data)==0) {
        lv_label_set_static_text(weather.wea_img,"\xee\x9c\x85"); 
    }
    if (strcmp("yu", data)==0) {
        lv_label_set_static_text(weather.wea_img,"\xee\x9c\x88"); 
    }
    if (strcmp("yun", data)==0) {
        lv_label_set_static_text(weather.wea_img,"\xee\x9c\x83"); 
    }
    if (strcmp("yujiaxue", data)==0) {
        lv_label_set_static_text(weather.wea_img,"\xee\x9c\x8b"); 
    }
    if (strcmp("bingbao", data)==0) {
        lv_label_set_static_text(weather.wea_img,"\xee\x9c\x8c"); 
    }
    if (strcmp("lei", data)==0) {
        lv_label_set_static_text(weather.wea_img,"\xee\x9c\x89"); 
    }
    if (strcmp("xue", data)==0) {
        lv_label_set_static_text(weather.wea_img,"\xee\x9c\x8a"); 
    }

}


void gui_display_cur_temp(char *data)
{
    static char temp[32] = {0};
    if (data == NULL) {
        return;
    }

    sprintf(temp, "%s℃", data);
    lv_label_set_static_text(weather.cur_temp, temp);
}

void gui_display_high_temp(char *data)
{
    static char temp[32] = {0};

    if (data == NULL) {
        return;
    }

    sprintf(temp, "%s℃ \xee\x9b\xad", data);
    lv_label_set_static_text(weather.high_temp, temp);
}

void gui_display_low_temp(char *data)
{
    static char temp[32] = {0};

    if (data == NULL) {
        return;
    }

    sprintf(temp, "%s℃ \xee\x9b\xac", data);
    lv_label_set_static_text(weather.low_temp, temp);
}

void gui_display_air_level(char *data)
{
    static char temp[32] = {0};

    if (data == NULL) {
        return;
    }

    /* display: temperature */
    sprintf(temp, "空气%s", data);

    lv_label_set_static_text(weather.air_level, temp);
}

void gui_display_humidity(char *data)
{
    static char temp[32] = {0};
    if (data == NULL) {
        return;
    }

    /* display: temperature */
    sprintf(temp, "湿度：%s", data);

    lv_label_set_static_text(weather.humidity, temp);
}

void gui_display_air_pm25(char *data)
{
    static char temp[32] = {0};
    static lv_style_t style;

    if (data == NULL) {
        return;
    }

    /* display: temperature */
    sprintf(temp, "PM2.5：%s", data);

    lv_label_set_static_text(weather.air_pm25, temp);
}

void gui_display_city(char *data)
{
    static char temp[32] = {0};
    if (data == NULL) {
        return;
    }

    lv_label_set_static_text(weather.city, data);
}
void gui_display_win(char *data1, char *data2)
{
    static char temp[32] = {0};
    if (data1 == NULL || data2 == NULL) {
        return;
    }

    sprintf(temp, "%s：%s", data1,data2);

    lv_label_set_static_text(weather.win, temp);
}

/*

* https://api.seniverse.com/v3/weather/now.json?key=SsNE9m5YvCjl-8NYc&location=nanjing&language=zh-Hans&unit=c

*/
#define  GET_REQUEST_PACKAGE     \
         "GET http://www.tianqiapi.com/api/?version=v6&appid=%s&appsecret=%s\r\n\r\n"

#define  appid    "54612715"
#define  appsecret    "4dlrQLn8 "


#define WEATHER_IP_ADDR "120.27.2.251"
#define WEATHER_PORT    80

LV_FONT_DECLARE(font); /* 申明字体 */

struct weather_info
{
    char *response;
    cJSON *root;
    cJSON *date;
    cJSON *city;
    cJSON *cur_temp;
    cJSON *high_temp;
    cJSON *low_temp;
    cJSON *humidity;
    cJSON *air_level;
    cJSON *wea;
    cJSON *wea_img;
    cJSON *air_pm25;
    cJSON *win;
    cJSON *win_speed;
};
struct weather_info info;
#define FLAGS_MSK1 0x00000001U

extern osEventFlagsId_t evt_id; /* event flags id */

void app_weather_task(void)
{

    int ClientSock;
    char GetRequestBuf[256] = {0};
    static char WeatherRecvBuf[1024*4] = {0};

    evt_id = osEventFlagsNew(NULL);
    if (evt_id == NULL)
    {
        printf("Falied to create EventFlags!\n");
    }

    weather_gui_init();
    while (1)
    {
        int  connect_status = 0;
        
        /* 设置要访问的服务器的信息 */
        struct sockaddr_in  ServerSockAddr;
        memset(&ServerSockAddr, 0, sizeof(ServerSockAddr));            /* 每个字节都用0填充 */
        ServerSockAddr.sin_family = AF_INET;                          /* IPv4 */
        ServerSockAddr.sin_addr.s_addr = inet_addr(WEATHER_IP_ADDR);  /* 天气服务器IP */
        ServerSockAddr.sin_port = htons(WEATHER_PORT);                 /*  端口 */
        
        /* 创建客户端socket */
        if (-1 == (ClientSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)))
        {
            printf("socket error!\n");
            exit(1);
        }

        /* 连接服务端 */
        if (-1 == (connect_status = connect(ClientSock, (struct sockaddr*)&ServerSockAddr, sizeof(struct sockaddr))))
        {
            printf("connect error!\n");
            exit(1);
        }
        
        /* 组合GET请求包 */
        sprintf(GetRequestBuf, GET_REQUEST_PACKAGE, appid, appsecret);

        /* 发送数据到服务端 */
        send(ClientSock, GetRequestBuf, strlen(GetRequestBuf), 0);
            
        /* 接受服务端的返回数据 */
        recv(ClientSock, WeatherRecvBuf, 1024*4, 0);
        printf("ClientSock %s \n\r",WeatherRecvBuf);

        int retSts = false;
        if (strstr(WeatherRecvBuf,"errcode") == NULL) {
            printf("no find error msg");
            retSts = true;
            info.root  = cJSON_Parse(WeatherRecvBuf); /* 解析JSON数据包 */
        } else {
            retSts = false;
            printf("find error msg");
        }
        
        if (info.root  != NULL)  /* 检测JSON数据包是否存在语法上的错误，返回NULL表示数据包无效 */
        {

            info.date = cJSON_GetObjectItem(info.root, "date");
            info.city = cJSON_GetObjectItem(info.root, "city");
            info.cur_temp = cJSON_GetObjectItem(info.root, "tem");
            info.high_temp = cJSON_GetObjectItem(info.root, "tem1");
            info.low_temp = cJSON_GetObjectItem(info.root, "tem2");
            info.humidity = cJSON_GetObjectItem(info.root, "humidity");
            info.wea = cJSON_GetObjectItem(info.root, "wea");             
            info.wea_img = cJSON_GetObjectItem(info.root, "wea_img");
            info.air_pm25 = cJSON_GetObjectItem(info.root, "air_pm25");
            info.air_level = cJSON_GetObjectItem(info.root, "air_level");
            info.win = cJSON_GetObjectItem(info.root, "win");
            info.win_speed = cJSON_GetObjectItem(info.root, "win_speed");
            printf("date : %s, cur_temp : %s, humidity : %s, wea_img : %s  air_pm25: %s\n",
                    info.date->valuestring, info.cur_temp->valuestring,
                    info.humidity->valuestring, info.wea_img->valuestring, info.air_pm25->valuestring);
        
            cJSON_Delete(info.root); /* 释放cJSON_Parse()分配出来的内存空间 */
        }

        if (retSts) {
            /* 显示天气 */
            gui_display_wea(info.wea->valuestring);
            gui_display_cur_temp(info.cur_temp->valuestring);
            gui_display_high_temp(info.high_temp->valuestring);
            gui_display_low_temp(info.low_temp->valuestring);
            gui_display_air_level(info.air_level->valuestring);
            gui_display_wea_img(info.wea_img->valuestring);
            gui_display_humidity(info.humidity->valuestring);
            gui_display_air_pm25(info.air_pm25 ->valuestring);
            gui_display_city(info.city->valuestring);
            gui_display_win(info.win->valuestring,info.win_speed->valuestring);
            sleep(5);
        } else {
            gui_display_wea("2021-08-31");
            gui_display_cur_temp("25");
            gui_display_high_temp("30");
            gui_display_low_temp("20");
            gui_display_air_level("良");
            gui_display_wea_img("qing");
            gui_display_humidity("55");
            gui_display_air_pm25("100");
            gui_display_city("深圳");
            gui_display_win("31","26");
        }
  
        /* 清空缓冲区 */
        memset(GetRequestBuf, 0, 256);   
        memset(WeatherRecvBuf, 0, 1024*4);      
        /* 关闭套接字 */
        closesocket(ClientSock);

        osEventFlagsWait(evt_id, FLAGS_MSK1, osFlagsWaitAny, osWaitForever);

    }
    
}


void weather_task_create(void)
{
    /* Create a task to handle uart communication */
    osThreadAttr_t attr;

    attr.name = "app_weather_task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = CONFIG_TASK_DEFAULT_STACKSIZE;
    attr.priority = CONFIG_TASK_PRIOR;
    
    if (osThreadNew((osThreadFunc_t)app_weather_task, NULL, &attr) == NULL)
    {
        printf("Falied to create uart_demo_task!\n");
    }
}

