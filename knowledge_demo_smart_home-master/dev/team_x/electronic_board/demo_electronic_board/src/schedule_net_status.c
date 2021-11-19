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
#include <pthread.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/ioctl.h>

#define NET_ERR(fmt, args...)   printf("[NET_STATUS_ERROR][%s|%d]" fmt"\n", __func__, __LINE__, ##args)
#define NET_DBG(fmt, args...)   printf("[NET_STATUS_DEBUG][%s|%d]" fmt"\n", __func__, __LINE__, ##args)

#define STATUS_CHECK_TIMEOUT    5
#define CHECK_STATUS_ADDR       "120.27.2.251"
#define CHECK_STATUS_PORT       80

static int g_netWorkStatus = 0;
static int g_threadRunning = 0;
static pthread_t g_thread = NULL;
static pthread_mutex_t g_mutex;

static int GetNetStatus(void)
{
    int status;
    pthread_mutex_lock(&g_mutex);
    status = g_netWorkStatus;
    pthread_mutex_unlock(&g_mutex);

    return status;
}

static void SetNetStatus(int status)
{
    pthread_mutex_lock(&g_mutex);
    g_netWorkStatus = status;
    pthread_mutex_unlock(&g_mutex);
}

static unsigned int CheckNetStatus(void)
{
    int sockfd = -1;
    struct sockaddr_in  ServerSockAddr;
    unsigned int result = 0;
    unsigned long ul = 1;

    sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        NET_ERR("socket failed!\n");
        goto EXIT;
    }

    (void)memset_s(&ServerSockAddr, sizeof(ServerSockAddr), 0, sizeof(ServerSockAddr));
    ServerSockAddr.sin_family = AF_INET;                                // IPv4
    ServerSockAddr.sin_addr.s_addr = inet_addr(CHECK_STATUS_ADDR);      // 服务器IP
    ServerSockAddr.sin_port = htons(CHECK_STATUS_PORT);                 // 端口

    ioctl(sockfd, FIONBIO, &ul); //设置为非阻塞模式

    if (connect(sockfd, (struct sockaddr *)&ServerSockAddr, sizeof(ServerSockAddr)) == -1) {
        struct timeval tm = {1, 0};
        fd_set set;
        FD_ZERO(&set);
        FD_SET(sockfd, &set);
        if (select(sockfd + 1, NULL, &set, NULL, &tm) > 0) {
            int err = 0;
            int length = sizeof(int);
            getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err, (socklen_t *)&length);
            if (err == 0) {
                result = 1;
            }
        }
    } else {
        result = 1;
    }

EXIT:
    if (sockfd >= 0) {
        close(sockfd);
        sockfd = -1;
    }

    return result;
}

static void *NetStatusProcess(void *p)
{
    while(g_threadRunning) {
        int status = CheckNetStatus();
        SetNetStatus(status);
        sleep(STATUS_CHECK_TIMEOUT);
    }
}

int ScheduleStartNetStatus(void)
{
    if (g_threadRunning && g_thread) {
        NET_ERR("NetStatusProcess has been created!");
        return 1;
    }
    
    pthread_mutex_init(&g_mutex, NULL);
    g_threadRunning = 1;

    if (pthread_create(&g_thread, NULL, NetStatusProcess, NULL) < 0) {
        NET_ERR("create NetStatusProcess failed!");
        return -1;
    }

    return 0;
}

void ScheduleStopNetStatus(void)
{
    if (g_threadRunning) {
        g_threadRunning = 0;
        pthread_join(g_thread, NULL);
        pthread_mutex_destroy(&g_mutex);
    }
}

int ScheduleGetStatus(void)
{
    return GetNetStatus();
}
