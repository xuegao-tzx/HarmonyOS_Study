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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include "cJSON.h"
#include "sntp.h"
#include "common.h"
#include "schedule_net_status.h"

#define LOG_DEBUG(fmt, args...)  printf("[DEBUG][%s|%d]" fmt, __func__, __LINE__, ##args)
#define LOG_INFO(fmt,  args...)  printf("[INFO ][%s|%d]" fmt, __func__, __LINE__, ##args)
#define LOG_ERROR(fmt, args...)  printf("[ERROR][%s|%d]" fmt, __func__, __LINE__, ##args)

static int g_connectStatus = 1;

static int GetNetStatus(void);

static int GetLocalTime(struct tm *tm)
{
    time_t t = time(NULL);
    struct tm *mt = localtime(&t);
    if (mt) {
        (void)memcpy_s(tm, sizeof(struct tm), mt, sizeof(struct tm));
    } else {
        LOG_ERROR("get localtime failed! \n");
        return -1;
    }

    return 0;
}

int SystemGetTime(struct tm *rtcTime)
{
    int ret = 0;
    if (rtcTime == NULL) {
        return -1;
    }
    g_connectStatus = GetNetStatus();
    if (g_connectStatus == 1) {
        ret = SntpGetRtcTime(LOCAL_TIME_ZONE, rtcTime);
    } else {
        ret = GetLocalTime(rtcTime);
    }

    return ret;
}

int SystemGetNetStatus()
{
    return GetNetStatus();
}

// ***************** weather ************************************************ //
#define USE_WEATHER_API_FREE    0

#if USE_WEATHER_API_FREE
// 没有空气湿度以及PM2.5 无限制次数
#define GET_REQUEST_PACKAGE "GET https://www.tianqiapi.com/free/day?appid=%s&appsecret=%s&unescape=1\r\n\r\n"
#else
// 每天限制访问次数 300
#define GET_REQUEST_PACKAGE "GET http://www.tianqiapi.com/api/?version=v6&appid=%s&appsecret=%s\r\n\r\n"
#endif

#define	WEATHER_PORT	80
#define WEATHER_IP_ADDR "120.27.2.251"

#define  APPID			"54612715"
#define  APPSECRET		"4dlrQLn8 "
#define __CHECK_NET__

static int GetNetStatus(void)
{
#ifdef  __CHECK_NET__
    return ScheduleGetStatus();
#else
    return 1;
#endif
}

static int GetWeatherMsgFromNet(char *weatherRecvBuf, int size)
{
    int sockfd = -1;
    char getRequestBuf[256] = {0};
    struct sockaddr_in  ServerSockAddr;

    if (weatherRecvBuf == NULL || size <= 0) {
        return -1;
    }

    (void)memset_s(weatherRecvBuf, size, 0x00, size);

    (void)memset_s(&ServerSockAddr, sizeof(ServerSockAddr), 0, sizeof(ServerSockAddr));
    ServerSockAddr.sin_family = AF_INET;                            // IPv4
    ServerSockAddr.sin_addr.s_addr = inet_addr(WEATHER_IP_ADDR);    // 天气服务器IP
    ServerSockAddr.sin_port = htons(WEATHER_PORT);                  // 端口

    sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        LOG_ERROR("socket failed!\n");
        return -1;
    }

    if (connect(sockfd, (struct sockaddr*)&ServerSockAddr, sizeof(struct sockaddr)) < 0) {
        close(sockfd);
        LOG_ERROR("connect failed!\n");
        return -1;
    }

    //sprintf(getRequestBuf, GET_REQUEST_PACKAGE, APPID, APPSECRET);
    if (sprintf_s(getRequestBuf, sizeof(getRequestBuf), GET_REQUEST_PACKAGE, APPID, APPSECRET) < 0) {
        close(sockfd);
        LOG_ERROR("getRequestBuf string failed! \n");
        return -1;
    }

    if (send(sockfd, getRequestBuf, strlen(getRequestBuf), 0) <= 0) {
        close(sockfd);
        LOG_ERROR("send failed! \n");
        return -1;
    }

    if (recv(sockfd, weatherRecvBuf, size, 0) <= 0) {
        close(sockfd);
        LOG_ERROR("recv failed! \n");
        return -1;
    }

    close(sockfd);

    return 0;
}

static int GetJsonItemValue(const cJSON *const json, const char *const item, char *buff)
{
    cJSON *obj;
    obj = cJSON_GetObjectItem(json, item);
    if (obj == NULL) {
        LOG_ERROR("get json %s failed! \n", item);
        return -1;
    }
    strcpy(buff, cJSON_GetStringValue(obj));

    return 0;
}

static int ParseWeatherInfo(const char *pMsg, WeatherInfo *wInfo)
{
#define WEATHER_NBR 10
    cJSON *root = NULL;
    const char *keyWord[WEATHER_NBR] = {
        "city", "date", "week", "update_time",
        "wea", "tem", "humidity", "win_meter",
        "air", "air_pm25"
    };
    char *msgBuff[WEATHER_NBR];
    int retval = 0;

    if (wInfo == NULL) {
        LOG_ERROR("NULL POINT! \n");
        return -1;
    }
    (void)memset_s(wInfo, sizeof(WeatherInfo), 0x00, sizeof(WeatherInfo));
    msgBuff[0] = wInfo->city;           // idx 0
    msgBuff[1] = wInfo->date;           // idx 1
    msgBuff[2] = wInfo->week;           // idx 2
    msgBuff[3] = wInfo->time;           // idx 3
    msgBuff[4] = wInfo->weather;        // idx 4
    msgBuff[5] = wInfo->temperature;    // idx 5
    msgBuff[6] = wInfo->humidity;       // idx 6
    msgBuff[7] = wInfo->airSpeed;       // idx 7
    msgBuff[8] = wInfo->airQuality;     // idx 8
    msgBuff[9] = wInfo->airPM25;        // idx 9

    root = cJSON_Parse(pMsg);
    if (root == NULL) {
        LOG_ERROR("cJSON_Parse failed! \n");
        return -1;
    }

    for (int i = 0; i < WEATHER_NBR; i++) {
        if (GetJsonItemValue(root, keyWord[i], msgBuff[i]) < 0) {
            LOG_ERROR("GetJsonItemValue %s failed! \n", keyWord[i]);
            retval = -1;
            break;
        }
    }

    cJSON_Delete(root);

    return retval;
}

int SystemGetWeather(WeatherInfo *wInfo)
{
    char weatherRecvBuf[1024 * 4];
    g_connectStatus = GetNetStatus();
    if (g_connectStatus <= 0) {
        LOG_ERROR("network have not been connectted! \n");
        return -1;
    }

    if (wInfo == NULL) {
        return -1;
    }

    if (GetWeatherMsgFromNet(weatherRecvBuf, sizeof(weatherRecvBuf)) < 0) {
        LOG_ERROR("GetWeatherMsgFromNet failed! \n");
        return -1;
    }

    if (ParseWeatherInfo((const char *)weatherRecvBuf, wInfo) < 0) {
        LOG_ERROR("ParseWeatherInfo failed! \n");
        return -1;
    }

    return 0;
}
