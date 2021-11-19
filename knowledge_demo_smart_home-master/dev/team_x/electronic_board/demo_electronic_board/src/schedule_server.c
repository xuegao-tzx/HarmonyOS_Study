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
#include <pthread.h>
#include <sys/errno.h>

#include "cJSON.h"
#include "schedule_server.h"
#include "schedule_net_status.h"

#define SCHEDULE_DEBUG

#ifdef  SCHEDULE_DEBUG
#define SCH_SEV_DBG(fmt, args...)   printf("[DEBUG][%s|%d]" fmt"\n", __func__, __LINE__, ##args)
#define SCH_SEV_ERR(fmt, args...)   printf("[ERROR][%s|%d]" fmt"\n", __func__, __LINE__, ##args)
#else
#define SCH_SEV_DBG(fmt, args...)   do {}while (0)
#define SCH_SEV_ERR(fmt, args...)   do {}while (0)
#endif

#define RETRY_TIME              3
#define ONE_MINUTE              60
#define NET_CHECK_TIME          5
#define SHCEDULE_SLEEP_TIME     (30 * ONE_MINUTE)
#define SERVER_IP               "81.71.17.188"
#define SERVER_PORT             8080

#define REQUEST_PARAMS          "GET /%s HTTP/1.1\r\nHost: %s:%d\r\nConnection:\
 keep-alive\r\naccept:*/*\r\nAccept-Language: zh-CN,zh;q=0.9\r\n%s"

#define GET_USER_ID_API         "distschedule-api/user/phone/%s"
#define USER_PHONE              "13528881305"
#define USER_ID_PARAM           "\r\n"

#define GET_SCHEDULE_LIST_API   "distschedule-api/schedule/query?startTime=%s&endTime=%s"
#define SCHEDULE_LIST_PARAM     "userId:%s\r\n\r\n"

static pthread_t g_thread = NULL;
static ListHandle g_handle = NULL;
static int g_pthread_Running = 0;
static ScheduleUpdate g_ScheduleUpdateFunc = NULL;
static void *g_updateData = NULL;
static int g_netWork = 1;

static void GetRequestUserIDStr(char *buff, int size, char *phone)
{
    char apiBuf[256] = {0};
    if (buff == NULL || size <= 0 || phone == NULL) {
        SCH_SEV_ERR("NULL POINT!");
        return;
    }
    sprintf(apiBuf, GET_USER_ID_API, phone);
    sprintf(buff, REQUEST_PARAMS, apiBuf, SERVER_IP, SERVER_PORT, USER_ID_PARAM);
}

static void GetRequestScheduleListStr(char *buff, int size, char *startTime, char *endTime, char *userID)
{
    char apiBuf[256] = {0};
    char parBuf[256] = {0};
    if (buff == NULL || size <= 0 || startTime == NULL || endTime == NULL || userID == NULL) {
        SCH_SEV_ERR("NULL POINT!");
        return;
    }
    sprintf(apiBuf, GET_SCHEDULE_LIST_API, startTime, endTime);
    sprintf(parBuf, SCHEDULE_LIST_PARAM, userID);

    sprintf(buff, REQUEST_PARAMS, apiBuf, SERVER_IP, SERVER_PORT, parBuf);
}

static int ScheduleServerGetRequest(char *request, char *result, int size)
{
    int sockfd = -1;
    struct sockaddr_in  ServerSockAddr;
    char recvBuf[1024 * 16] = {0};
    int pos = 0;
    int retry = 0;

    if (request == NULL || result == NULL || size <= 0) {
        SCH_SEV_ERR("NULL POINT!");
        return -1;
    }

    (void)memset_s(result, size, 0x00, size);

    (void)memset_s(&ServerSockAddr, sizeof(ServerSockAddr), 0, sizeof(ServerSockAddr));
    ServerSockAddr.sin_family = AF_INET;
    ServerSockAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    ServerSockAddr.sin_port = htons(SERVER_PORT);

    sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        SCH_SEV_ERR("socket failed! \n");
        return -1;
    }

    if (connect(sockfd, (struct sockaddr*)&ServerSockAddr, sizeof(struct sockaddr)) < 0) {
        SCH_SEV_ERR("connect sock failed! \n");
        close(sockfd);
        return -1;
    }
    SCH_SEV_DBG("send:%s\n", request);
    if (send(sockfd, request, strlen(request), 0) <= 0) {
        SCH_SEV_ERR("send request failed! \n");
        close(sockfd);
        return -1;
    }

    while (1) {
        int ret = 0;
        fd_set rset;
        struct timeval tv = {2, 0};
        FD_ZERO(&rset);
        FD_SET(sockfd, &rset);

        ret = select(sockfd + 1, &rset, NULL, NULL, &tv);
        if (ret <= 0) {
            if (++retry >= RETRY_TIME) {
                break;
            }
            continue;
        }

        if (recv(sockfd, recvBuf + pos, sizeof(recvBuf) - pos, 0) <= 0) {
            SCH_SEV_ERR("recv failed! \n");
            break;
        }
        pos = strlen(recvBuf);
        retry = 0;
    }
    SCH_SEV_DBG("recvBuf[%d]:%s\n", strlen(recvBuf), recvBuf);
    close(sockfd);
    if (pos > 0) {
        char *ps = NULL;
        char *pe = NULL;
        int len = 0;
        ps = strstr(recvBuf, "{\"");
        if (ps == NULL) {
            SCH_SEV_ERR("get json string failed! \n");
            return -1;
        }

        pe = strrchr(ps, '}');
        if (pe == NULL) {
            SCH_SEV_ERR("get json string failed! \n");
            return -1;
        }
        len = pe - ps + 1;
        (void)strncpy_s(result, size, ps, len);
        result[len] = 0x00;
    }

    SCH_SEV_DBG("result[%d]:%s\n", strlen(result), result);

    return pos > 0 ? 0 : -1;
}

static int ParseUserId(const char *pString, char *idBuf, int size)
{
    cJSON *root = NULL;
    cJSON *objs = NULL;
    cJSON *obj = NULL;

    if (pString == NULL || idBuf == NULL || size <= 0) {
        SCH_SEV_ERR("NULL POINT!");
        return -1;
    }

    root = cJSON_Parse(pString);
    if (root == NULL) {
        SCH_SEV_ERR("cJSON_Parse %s failed! \n", pString);
        return -1;
    }

    objs = cJSON_GetObjectItem(root, "result");
    if (objs == NULL) {
        cJSON_Delete(root);
        SCH_SEV_ERR("no result! \n");
        return -1;
    }

    obj = cJSON_GetObjectItem(objs, "id");
    if (obj == NULL) {
        cJSON_Delete(root);
        SCH_SEV_ERR("no id! \n");
        return -1;
    }

    (void)strcpy_s(idBuf, size, cJSON_GetStringValue(obj));

    cJSON_Delete(root);

    return 0;
}

static void DealString(char *buff, int length)
{
    char *tmp = NULL, *ps = NULL;
    if (buff == NULL || length <= 0) {
        return;
    }
    SCH_SEV_DBG("buff : %s \n", buff);
    tmp = (char *)malloc(length + 1);
    (void)memset_s(tmp, length + 1, 0x00, length);
    if (strcpy_s(tmp, length + 1, buff) < 0) {
        SCH_SEV_ERR("strcpy_s failed! \n");
    }
    ps = buff;
    SCH_SEV_DBG("tmp : %s \n", buff);
    for (int i = 0; i < length; i++) {
        if (tmp[i] == '\\') {
            continue;
        }
        *ps++ = tmp[i];
    }
    *ps = 0x00;

    free(tmp);
    SCH_SEV_DBG("buff : %s \n", buff);
}

static void GetDetailString(const char *pString, char *buff, int size)
{
#define KEY_WORD    "\\\"command\\\":\\\"{"
    char *ps = NULL;
    char *pe = NULL;
    int offset = 0;

    if (pString == NULL || buff == NULL || size <= 0) {
        SCH_SEV_ERR("NULL POINT!");
        return;
    }

    ps = strstr(pString, KEY_WORD);
    if (ps == NULL) {
        SCH_SEV_ERR("no command[%s] string! %s\n", KEY_WORD, pString);
        (void)strcpy_s(buff, size, "No Details.\n");
        return;
    }

    ps += strlen(KEY_WORD) + 1;
    pe = strchr(ps, '}');
    if (pe == NULL) {
        SCH_SEV_ERR("no command string! \n");
        (void)strcpy_s(buff, size, "No Details.\n");
        return;
    }
    offset = pe - ps;
    (void)strncpy_s(buff, size, ps, offset);

    buff[offset] = 0x00;
    DealString(buff, offset);
}

static void GetScheduleDetail(cJSON *objArray, char *buff, int size)
{
    int len = 0;
    int offs = 0;
    char *pBuf = buff;

    if (objArray == NULL || buff == NULL || size <= 0) {
        SCH_SEV_ERR("NULL POINT!");
        return;
    }

    if (cJSON_IsArray(objArray)) {
        len = cJSON_GetArraySize(objArray);
        if (len <= 0) {
            SCH_SEV_ERR("objArray(%s) size is 0!\n", cJSON_Print(objArray));
            (void)strcpy_s(buff, size, "No Details.\n");
            return;
        }

        for (int i = 0; i < len; i++) {
            cJSON *item = NULL;
            cJSON *obj = NULL;
            char *pMsg = NULL;
            item = cJSON_GetArrayItem(objArray, i);
            if (item == NULL) {
                SCH_SEV_ERR("get schedule list item failed! \n");
                (void)strcpy_s(buff, size, "No Details.\n");
                break;
            }
            obj = cJSON_GetObjectItem(item, "command");
            pMsg = cJSON_Print(obj);
            if ((offs + strlen(pMsg)) > size) {
                break;
            }

            (void)strcpy_s((pBuf + offs), (size - offs + 1), pMsg);
            *(pBuf + offs + 1) = 0x20;
            offs += strlen(pMsg) + 1;
        }
    } else {
        char *pMsg = cJSON_Print(objArray);
        GetDetailString((const char *)pMsg, buff, size);
    }
}

static void GetWeekDay(const char *timeString, int *weekDay)
{
    int year = 0;
    int month = 0;
    int day = 0;

    int week = 0;

    if (timeString == NULL || weekDay == NULL) {
        return;
    }

    if (sscanf_s(timeString, "%04d-%02d-%02d", &year, &month, &day) < 0) {
        SCH_SEV_ERR("sscanf_s failed! \n");
        return;
    }

    // 1,2月看成上一年的13，14月，所以月份需加12， 年份减1 
    if (month == 1 || month == 2) {
        month += 12;
        year -= 1;
    }

    // 基姆拉尔森计算公式: 2, 3, 5 月份计算参数, 4, 100, 400 闰年计算参数.
    week = (day + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400 + 1) % 7;
    printf("%04d-%02d-%02d is week %d \n", year, month, day, week);
    if (week == 0) {
        week = 7;
    }

    *weekDay = week;
}

static int CheckRemindDay(const cJSON *obj, int weekday)
{
    cJSON *remindDay;
    char *remindDayString = NULL;

    if (obj == NULL || weekday <= 0) {
        return -1;
    }
    printf("[%s|%d]obj : %s \n", __func__, __LINE__, cJSON_Print(obj));
    remindDay = cJSON_GetObjectItem(obj, "remindDay");
    if (remindDay == NULL) {
        SCH_SEV_ERR("Get remindDay obj failed! \n");
        return -1;
    }

    remindDayString = cJSON_GetStringValue(remindDay);
    // this string format must be "1,2,3,4,5,6,7"
    for (int i = 0; i < strlen(remindDayString); i += 2) {
        if ((remindDayString[i] - '0') == weekday) {
            return 1;
        }
    }

    return 0;
}

static int ParseScheduleList(const char *pString, ListHandle mHandle, const char *starttime)
{
    cJSON *root = NULL;
    cJSON *objArray = NULL;
    int size = 0;
    int weekday = 0;

    GetWeekDay(starttime, &weekday);
    SCH_SEV_DBG("weekday : %d \n", weekday);
    if (pString == NULL || mHandle == NULL) {
        SCH_SEV_ERR("NULL POINT!");
        return -1;
    }

    root = cJSON_Parse(pString);
    if (root == NULL) {
        SCH_SEV_ERR("cJSON_Parse %s failed! \n", pString);
        return -1;
    }

    objArray = cJSON_GetObjectItem(root, "result");
    if (objArray == NULL) {
        cJSON_Delete(root);
        SCH_SEV_ERR("no result! \n");
        return -1;
    }

    size = cJSON_GetArraySize(objArray);
    SCH_SEV_DBG("size = %d", size);

    if (size <= 0 && LinkListGetSize(mHandle) > 0) {
        LinkListClear(mHandle);
        cJSON_Delete(root);
        return 1;
    }

    if (size > 0 && size != LinkListGetSize(mHandle)) {
        LinkListClear(mHandle);

        for (int i = 0; i < size; i++) {
            ScheduleInfoT info;
            cJSON *obj = NULL;
            cJSON *item = NULL;
            (void)memset_s(&info, sizeof(info), 0x00, sizeof(info));

            item = cJSON_GetArrayItem(objArray, i);
            if (item == NULL) {
                SCH_SEV_ERR("get schedule list item failed! \n");
                break;
            }
            obj = cJSON_GetObjectItem(item, "id");
            if (obj == NULL) {
                SCH_SEV_ERR("get schedule list item id failed! \n");
                break;
            }
            (void)strcpy_s(info.id, sizeof(info.id), cJSON_GetStringValue(obj));

            obj = cJSON_GetObjectItem(item, "name");
            if (obj == NULL) {
                SCH_SEV_ERR("get schedule list item name failed! \n");
                break;
            }
            (void)strcpy_s(info.name, sizeof(info.name), cJSON_GetStringValue(obj));

            obj = cJSON_GetObjectItem(item, "startTime");
            if (obj == NULL) {
                SCH_SEV_ERR("get schedule list item startTime failed! \n");
                break;
            }

            (void)strcpy_s(info.startTime, sizeof(info.startTime), cJSON_GetStringValue(obj));
            if (strncmp(info.startTime, starttime, 10)) {   // cmp date length of 10
                SCH_SEV_ERR("the schedule date[%s] is not taday[%s]! \n", info.startTime, starttime);

                if (CheckRemindDay(obj, weekday) <= 0) {
                    SCH_SEV_ERR("the schedule date[%s] is not the remind day! \n");
                    continue;
                }
            }

            obj = cJSON_GetObjectItem(item, "endTime");
            if (obj == NULL) {
                SCH_SEV_ERR("get schedule list item endTime failed! \n");
                break;
            }
            (void)strcpy_s(info.endTime, sizeof(info.endTime), cJSON_GetStringValue(obj));

            obj = cJSON_GetObjectItem(item, "deviceConfig");
            if (obj == NULL) {
                SCH_SEV_ERR("get schedule list item deviceConfig failed! \n");
                break;
            }
            GetScheduleDetail(obj, info.details, sizeof(info.details));

            LinkListAdd(mHandle, (void *)(&info), sizeof(ScheduleInfoT));
        }

        cJSON_Delete(root);

        return 1;
    }

    cJSON_Delete(root);

    return 0;
}

// @func: if the list has been changed, tell the ui process schedule info has been changed
// @params: idBuf -- schedule id
// @return: 1 schedule has update. others schedule do not update
static int CheckScheduleUpdate(char *idBuf, int size)
{
    char id[BUFF_SHORT] = {0};
    char requestBuf[1024 * 16] = {0};
    char responseBuf[1024] = {0};
    char startTime[BUFF_SHORT] = {0};
    char endTime[BUFF_SHORT] = {0};
    struct tm rtcTime;
    int year, month, day;
    const int baseYear = 1900;

    SystemGetTime(&rtcTime);
    year = rtcTime.tm_year + baseYear;
    month = rtcTime.tm_mon + 1;
    day = rtcTime.tm_mday;

    SCH_SEV_ERR("GetSysDate[%04d-%02d-%02d] \n", year, month, day);

    if (idBuf == NULL || strlen(idBuf) == 0) {
        GetRequestUserIDStr(responseBuf, sizeof(responseBuf), USER_PHONE);
        if (ScheduleServerGetRequest(responseBuf, requestBuf, sizeof(requestBuf)) < 0) {
            SCH_SEV_ERR("ScheduleServerGetRequest failed! \n");
            return -1;
        }

        if (ParseUserId(requestBuf, id, sizeof(id)) < 0) {
            return -1;
        }
        if (idBuf) {
            (void)strcpy_s(idBuf, size, id);
        }
        (void)memset_s(responseBuf, sizeof(responseBuf), 0x00, sizeof(responseBuf));
        (void)memset_s(requestBuf, sizeof(requestBuf), 0x00, sizeof(requestBuf));
    } else {
        (void)strcpy_s(id, sizeof(id), idBuf);
    }

    sprintf(startTime, "%04d-%02d-%02d%%2000%%3A00%%3A00", year, month, day);
    sprintf(endTime, "%04d-%02d-%02d%%2023%%3A59%%3A00", year, month, day);

    GetRequestScheduleListStr(responseBuf, sizeof(responseBuf), startTime, endTime, id);
    if (ScheduleServerGetRequest(responseBuf, requestBuf, sizeof(requestBuf)) < 0) {
        SCH_SEV_ERR("ScheduleServerGetRequest failed! \n");
        return -1;
    }

    if (ParseScheduleList(requestBuf, g_handle, (const char *)startTime) > 0) {
        return 1;
    }

    return 0;
}

static void *ScheduleServerProcess(void *params)
{
    char scheduleID[BUFF_SHORT] = {0};
    g_handle = LinkListCreate();
    if (g_handle == NULL) {
        SCH_SEV_ERR("LinkListCreate failed! \n");
        g_pthread_Running = 0;
        return NULL;
    }

    while (g_pthread_Running) {
        g_netWork = ScheduleGetStatus();
        if (g_netWork == 0) {
            sleep(NET_CHECK_TIME);
            continue;
        }

        if (CheckScheduleUpdate((char *)scheduleID, sizeof(scheduleID)) > 0) {
            if (g_ScheduleUpdateFunc != NULL) {
                g_ScheduleUpdateFunc(g_updateData, g_handle);
            }
        }
        sleep(SHCEDULE_SLEEP_TIME);
    }

    return NULL;
}

int ScheduleServerStart(ScheduleUpdate updateFunc, void *data)
{
    if (g_pthread_Running != 0) {
        SCH_SEV_ERR("ScheduleServer has been already startting!");
        return 1;
    }

    g_ScheduleUpdateFunc = updateFunc;
    g_updateData = data;
    g_pthread_Running = 1;

    if (pthread_create(&g_thread, NULL, ScheduleServerProcess, NULL) == -1) {
        g_pthread_Running = -1;
        SCH_SEV_ERR("pthread_create failed! \n");
        return -1;
    }
    pthread_detach(g_thread);

    return 0;
}

void ScheduleServerStop(void)
{
    g_pthread_Running = 0;
}
