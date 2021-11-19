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
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <securec.h>
#include "socket_udp.h"
#include "cenctrl_log.h"

// *********************************************** //

#define DISC_PORT       9090
#define BC_SLEEP_TIME   5
#define BUFF_SHORT_SIZE 128

static pthread_t g_discThreadID = 0;
static int g_udpFlag = 1;

static void *SocketUdpStart(void *p)
{
    struct sockaddr_in sendAddr, recvAddr;
    int set = 1;
    pthread_detach(pthread_self());
    int sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        LOG(CENCTRL_ERROR, "socket failed! \n");
        return NULL;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (char *) &set, sizeof(set)) < 0) {
        LOG(CENCTRL_ERROR, "open setsockopt failed! errno=%d, sockfd = %d\n", errno, sockfd);
        return NULL;
    }

    memset_s(&sendAddr, sizeof(struct sockaddr_in), 0, sizeof(struct sockaddr_in));
    sendAddr.sin_family = AF_INET;
    sendAddr.sin_port = htons(DISC_PORT);
    sendAddr.sin_addr.s_addr = INADDR_BROADCAST;

    char sendbuf[BUFF_SHORT_SIZE] = "I am udp";
    int writeBytes = 0;
    socklen_t slen = sizeof(sendAddr);
    while (g_udpFlag) {
        writeBytes = sendto(sockfd, sendbuf, strlen(sendbuf), 0, (struct sockaddr *)&sendAddr, slen);
        if (writeBytes == -1) {
            writeBytes = 0;
            LOG(CENCTRL_ERROR, "#### udp sendto err ###\n");
            break;
        }
        sleep(BC_SLEEP_TIME); // 10 second
    }

    close(sockfd);

    return NULL;
}

void SocketUdpCreateBC(void)
{
    LOG(CENCTRL_DEBUG, "create the broadcast\n");
    g_udpFlag = 1;
    int ret = pthread_create(&g_discThreadID, NULL, SocketUdpStart, NULL);
    if (ret != 0) {
        LOG(CENCTRL_ERROR, "socket_udp_main [ERROR]thread error %s\n", strerror(errno));
        return;
    }
}

void SocketUdpDelBC()
{
    g_udpFlag = 0;
    int ret;
    if (g_discThreadID != 0) {
        ret = pthread_cancel(g_discThreadID);
        pthread_join(g_discThreadID, NULL);
        if (ret != 0) {
            LOG(CENCTRL_ERROR, "pthread_cancel(g_discThreadID) ret -> %d \n", ret);
            return;
        }
        g_discThreadID = 0;
        LOG(CENCTRL_DEBUG, " SocketUdpDelete\n ");
    }
}