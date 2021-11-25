/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
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
#include "mqtt_ohos.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include <stdio.h>

#include "cmsis_os2.h"

#define LOGD(fmt, ...) printf("[%d] %s " fmt "\n", osKernelGetTickCount(), osThreadGetName(osThreadGetId()), ##__VA_ARGS__)

#define US_PER_SEC (1000*1000)

static uint32_t g_tickPerSec = 0;
static uint32_t g_sysPerSec = 0;
static uint32_t g_sysPerTick = 0;

#if defined(MQTT_TASK)
void MutexInit(Mutex* m)
{
    m->mutex = osMutexNew(NULL);
}

int MutexLock(Mutex* m)
{
    return osMutexAcquire(m->mutex, osWaitForever);
}

int MutexUnlock(Mutex* m)
{
    return osMutexRelease(m->mutex);
}

void MutexDeinit(Mutex* m)
{
    osMutexDelete(m->mutex);
}

int ThreadStart(Thread* t, void (*fn)(void*), void* arg)
{
    osThreadAttr_t attr = {0};
    attr.name = "MqttTask";
    attr.stack_size = MQTT_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;

    t->thread = osThreadNew(fn, arg, &attr);
    if (t->thread == NULL) {
        printf("osThreadNew failed!\r\n");
        return -1;
    }
    return 0;
}

void ThreadJoin(Thread* t)
{
    (void) t;
    // osThreadJoin(t->thread);
    osDelay(1);
}

void ThreadYield(void)
{
    osStatus_t rc = osThreadYield();
    printf("osThreadYield: %d!\n", rc);
}

#include <unistd.h>
void Sleep(int ms)
{
    uint32_t tickFreq = osKernelGetTickFreq();
    uint32_t msPerTick = 1000 / tickFreq;
    osDelay(ms / msPerTick);
    uint32_t restMs = ms % msPerTick;
    if (restMs) {
        usleep(restMs * 1000);
    }
}
#endif

static int GetCurrentTime(struct timeval* now)
{
    if (now == NULL) {
        return -1;
    }

    uint32_t tickCount = osKernelGetTickCount();
    now->tv_sec = tickCount / g_tickPerSec;
    now->tv_usec = (tickCount % g_tickPerSec) * (US_PER_SEC / g_tickPerSec);

    uint64_t sysCount = osKernelGetSysTimerCount() % g_sysPerTick;
    now->tv_usec += sysCount * US_PER_SEC / g_sysPerSec;
    return 0;
}

#define gettimeofday(tv, tz) GetCurrentTime(tv)

/*******************************************************************************
 * Copyright (c) 2014, 2017 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Allan Stockdill-Mander - initial API and implementation and/or initial documentation
 *    Ian Craggs - return codes from linux_read
 *******************************************************************************/

void TimerInit(Timer* timer)
{
    timer->endTime = (struct timeval){0, 0};

    if (g_tickPerSec == 0) {
        g_tickPerSec = osKernelGetTickFreq();
        g_sysPerSec = osKernelGetSysTimerFreq();
        g_sysPerTick = g_sysPerSec / g_tickPerSec;
    }
}

char TimerIsExpired(Timer* timer)
{
    struct timeval now, res;
    gettimeofday(&now, NULL);
    timersub(&timer->endTime, &now, &res);
    return res.tv_sec < 0 || (res.tv_sec == 0 && res.tv_usec <= 0);
}

void TimerCountdownMS(Timer* timer, unsigned int ms)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    struct timeval interval = {ms / 1000, (ms % 1000) * 1000};
    timeradd(&now, &interval, &timer->endTime);
}

void TimerCountdown(Timer* timer, unsigned int seconds)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	struct timeval interval = {seconds, 0};
	timeradd(&now, &interval, &timer->endTime);
}

int TimerLeftMS(Timer* timer)
{
    struct timeval now, res;
    gettimeofday(&now, NULL);
    timersub(&timer->endTime, &now, &res);
    return (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000;
}

#undef gettimeofday

static int NetworkRead(Network* network, unsigned char* buffer, int len, int timeoutMs)
{
    struct timeval interval = {timeoutMs / 1000, (timeoutMs % 1000) * 1000};
    if (interval.tv_sec < 0 || (interval.tv_sec == 0 && interval.tv_usec <= 0))
    {
        interval.tv_sec = 0;
        interval.tv_usec = 100;
    }

    // LOGD("read with timeout = %d", timeoutMs);
    setsockopt(network->socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&interval, sizeof(struct timeval));

    int bytes = 0;
    while (bytes < len) {
        int rc = recv(network->socket, &buffer[bytes], (size_t)(len - bytes), 0);
        if (rc == -1) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                bytes = -1;
            }
            break;
        } else if (rc == 0) {
            bytes = 0;
            break;
        } else {
            bytes += rc;
        }
    }
    return bytes;
}


static int NetworkWrite(Network* network, unsigned char* buffer, int len, int timeoutMs)
{
    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = timeoutMs * 1000;  // Not init'ing this can cause strange errors

    setsockopt(network->socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv,sizeof(struct timeval));
    return send(network->socket, buffer, len, 0);
}

void NetworkInit(Network* network)
{
    network->socket = -1;
    network->mqttread = NetworkRead;
    network->mqttwrite = NetworkWrite;
}

int NetworkConnect(Network* network, char* host, int port)
{
    int rc = -1;
    struct sockaddr_in address;
    struct addrinfo *result = NULL;
    struct addrinfo hints = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL};

    if ((rc = getaddrinfo(host, NULL, &hints, &result)) == 0) {
        struct addrinfo* res = result;

        while (res) {
            if (res->ai_family == AF_INET) { /* prefer ip4 addresses */
                result = res;
                break;
            }
            res = res->ai_next;
        }

        if (result->ai_family == AF_INET) {
            address.sin_port = htons(port);
            address.sin_family = AF_INET;
            address.sin_addr = ((struct sockaddr_in*)(result->ai_addr))->sin_addr;
        } else {
            rc = -1;
        }
        freeaddrinfo(result);
    }

    if (rc == 0) {
        network->socket = socket(AF_INET, SOCK_STREAM, 0);
        if (network->socket != -1) {
            rc = connect(network->socket, (struct sockaddr*)&address, sizeof(address));
        } else {
            rc = -1;
        }
    }
    return rc;
}

void NetworkDisconnect(Network* network)
{
    lwip_close(network->socket);
}
