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
#include <stdlib.h>
#include <unistd.h>

#include "ohos_init.h"
#include "ohos_types.h"
#include "cmsis_os2.h"

#include "lwip/sockets.h"
#include "cJSON.h"
#include <time.h>
#include "kv_store.h"


#include "lcd_st7789.h"
#include "lv_port_disp.h"

#define CONFIG_TASK_PRIOR           25                        /* default task priority */
#define CONFIG_TASK_TIMER_STACKSIZE    0x800             /* unit:bytes */


typedef struct   
{  
    unsigned char LiVnMode;  
    unsigned char Stratum;  
    unsigned char Poll;  
    unsigned char Precision;  
    long int RootDelay;  
    long int RootDispersion;  
    char RefID[4];  
    long int RefTimeInt;  
    long int RefTimeFraction;  
    long int OriTimeInt;  
    long int OriTimeFraction;  
    long int RecvTimeInt;  
    long int RecvTimeFraction;  
    long int TranTimeInt;  
    long int TranTimeFraction;  
}STNP_Header;  

typedef struct HI3861SOFTRTC
{
    int year;
    unsigned char month;
    unsigned char day;
    unsigned char ampm;
    unsigned char week;
    unsigned char hour;
    unsigned char minute;
    unsigned char second;
} HI3861SOFTRTC_T;

static const char *week_list[7] = {"星期一", "星期二", "星期三", "星期四", "星期五", "星期六", "星期日"};                 //日期列表

#define bzero(a, b)             memset(a, 0, b)

#define JAN_1970    2208988676UL+28701        /* 1970 - 1900 in seconds */  
#define NTP_SERVER_NAME "120.24.166.46"    /* 深圳阿里云 */
#define NTP_PORT    123  

LV_FONT_DECLARE(my_font_30); /* 申明字体 */
LV_FONT_DECLARE(my_font_35); /* 申明字体 */
LV_FONT_DECLARE(my_font_15); /* 申明字体 */
HI3861SOFTRTC_T hi3861softrtc;


uint8_t day_change;
extern osEventFlagsId_t evt_id;  /* event flags id */
extern char step_keyvalue[128];


extern void gui_display_background(void);

int app_time_task(void)  
{  
    char hi3861softrtc_data[12];
    char hi3861softrtc_time[10];
    char week_text[24];
    char dailShow[36];
    STNP_Header HeaderSNTP,HeaderSNTP2;  
    time_t t1;  
    struct timeval tv;  
    struct tm *time;
    socklen_t addr_length;

    int sockfd=0;  
    struct sockaddr_in server;  
    fd_set set;   
    struct timeval timeout;  

    int nRet;

    bzero((char *)&HeaderSNTP, sizeof(STNP_Header));  /* 清零 */
    HeaderSNTP.LiVnMode = 0x1b;  /* 设置NTP报文格式，LI=0:无警告;  VN=3:NTP的版本号为3;  Mode=3:客户; */
               
    /* 设置要连接的对方的IP地址和端口等属性 */
    bzero(&server,sizeof(server)); /* 初始化结构体 */
    server.sin_family = AF_INET;    /* 设置地址家族 */
    server.sin_port = htons(NTP_PORT);  /* 设置端口 */
    addr_length = sizeof(server);
    if(inet_aton(NTP_SERVER_NAME,&server.sin_addr) <= 0) /* 设置地址 */
      printf("inet_pton error\n");
        
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);  /* 创建一个UDP socket */

    gui_display_background();

    lv_obj_t* src = lv_scr_act();
    /* **********************创建时间控件*************************** */
    static lv_style_t my_style;
    lv_style_copy(&my_style,&lv_style_transp);
    my_style.text.font = &my_font_35;
    lv_obj_t* time_label = lv_label_create(src,NULL);
    lv_label_set_style(time_label,LV_LABEL_STYLE_MAIN,&my_style);
    // lv_label_set_body_draw(label,true);
    lv_obj_set_pos(time_label,30,75);
    
    /* **********************创建日期控件*************************** */
    lv_obj_t* data_label = lv_label_create(src,NULL);
    lv_obj_set_pos(data_label,155,120);                                                    
    
    static lv_style_t week_style;
    lv_obj_t* week_label = lv_label_create(src,NULL);
    lv_style_copy(&week_style, &lv_style_transp);
    week_style.text.font = &my_font_15;
    lv_obj_set_pos(week_label,155,140);
    lv_label_set_style(week_label, 0, &week_style);


    while(1) {

        char step_keyvalue[128] = {0};

        bzero((char *)&HeaderSNTP2, sizeof(STNP_Header));

        if (sendto(sockfd, (char *)&HeaderSNTP, sizeof(STNP_Header), 0, (struct sockaddr*)&server, addr_length)<0)  
        {  
            printf("sendto error!\n");  
            return -1;  
        }              

        FD_ZERO(&set);  /* 清除描述词组set的全部位 */
        FD_SET(sockfd, &set); /* 设置描述词组set中相关fd的位 */
        timeout.tv_sec = 0;  /* 等待超时时间 */
        timeout.tv_usec = 500000;  
        nRet = select(sockfd+1, &set, NULL, NULL, &timeout);
        if ( nRet < 0 )  
        {
            printf("select error!\n");  
            continue; 
        }
        else if (nRet==0)
        {
            printf("time out!\n");  
            continue;
        }
        else {
            if (FD_ISSET(sockfd, &set)) {
                if (recvfrom(sockfd, (char *)&HeaderSNTP2, sizeof(STNP_Header), 0, (struct sockaddr*)&server, &addr_length)<0)  
                {
                    printf("recv error!\n");
                    return -1;  
                }
            }
        }
        
            
        /* 从1900年1月1号0时0分0秒到服务器向客户发时间戳的时间，单位：秒 */
        t1 = ntohl(HeaderSNTP2.TranTimeInt);  

        /* 减去1900年至1970年的时间 */
        tv.tv_sec=t1-JAN_1970;  
                
        /* 把从1970-1-1零点零分到当前时间系统所偏移的秒数时间转换为日历时间 */
        /* 获得的tm结构体的时间，是已经进行过时区转化为本地时间,年份加上1900，月份加上1 */
        time=localtime((const long int *)&tv.tv_sec);
        if ((hi3861softrtc.year!= ((time->tm_year)+1900)) || (hi3861softrtc.month != ((time->tm_mon)+1)) || (hi3861softrtc.day != time->tm_mday ))
        {
            hi3861softrtc.year = (time->tm_year)+1900;
            hi3861softrtc.month = (time->tm_mon)+1;
            hi3861softrtc.day = time->tm_mday;
            hi3861softrtc.week = time->tm_wday;
            sprintf(hi3861softrtc_data,"%u-%u-%u",  hi3861softrtc.year,hi3861softrtc.month,hi3861softrtc.day);
            char data_keyvaule[128] = {0};
            UtilsGetValue("data",data_keyvaule,128);
            if(strcmp(data_keyvaule, hi3861softrtc_data)!=0)
            {
                UtilsSetValue("step","0");
                memset(step_keyvalue, 0, 128);
            }
            UtilsSetValue("data",hi3861softrtc_data); 


            printf("hi3861softrtc_data : %s\n",hi3861softrtc_data);
            
            /* ****************更新星期***************** */
            strcpy(week_text, week_list[hi3861softrtc.week-1]);
            printf("RTC set date!\n");
        }
        if (hi3861softrtc.hour != time->tm_hour || hi3861softrtc.minute != time->tm_min)
        {
            if(hi3861softrtc.hour != time->tm_hour)
            {
                osEventFlagsSet(evt_id, 0x00000001U);
            }
            hi3861softrtc.hour = time->tm_hour;
            hi3861softrtc.minute = time->tm_min;
            hi3861softrtc.second = time->tm_sec;
            printf("RTC set time!\n");
        }
        while (hi3861softrtc.second <60)
        {
            /* 显示时间,学员自行补充 */
            sprintf(hi3861softrtc_time,"%u:%02d:%02d",  hi3861softrtc.hour,hi3861softrtc.minute,hi3861softrtc.second);

            lv_label_set_static_text(time_label,hi3861softrtc_time);
            lv_label_set_static_text(data_label,hi3861softrtc_data);
            lv_label_set_static_text(week_label,week_text);        
            osDelay(100);
            hi3861softrtc.second +=1;
        }

        printf("softrtc:%d-%d-%d,%d:%d:%d\n",hi3861softrtc.year,hi3861softrtc.month,hi3861softrtc.day,hi3861softrtc.hour,hi3861softrtc.minute,hi3861softrtc.second);

    }    
}  


void time_task_create(void)
{
    /* Create a task to handle uart communication */
    osThreadAttr_t attr;

    attr.name = "app_time_task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = CONFIG_TASK_TIMER_STACKSIZE;
    attr.priority = CONFIG_TASK_PRIOR;
    
    if (osThreadNew((osThreadFunc_t)app_time_task, NULL, &attr) == NULL)
    {
        printf("Falied to create uart_demo_task!\n");
    }
}
