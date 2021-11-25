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
#ifndef MQTT_OHOS_H
#define MQTT_OHOS_H

#include <sys/time.h>

typedef struct {
    struct timeval endTime;
} Timer;

void TimerInit(Timer* timer);
char TimerIsExpired(Timer* timer);
void TimerCountdownMS(Timer* timer, unsigned int ms);
void TimerCountdown(Timer* timer, unsigned int seconds);
int TimerLeftMS(Timer* timer);

typedef struct Network {
    int socket;
    int (*mqttread)(struct Network*, unsigned char*, int, int);
    int (*mqttwrite)(struct Network*, unsigned char*, int, int);
} Network;

void NetworkInit(Network* network);
int NetworkConnect(Network* network, char* host, int port);
void NetworkDisconnect(Network* network);

#if defined(MQTT_TASK)

#ifdef OHOS_CMSIS
#include "cmsis_os2.h"

typedef struct {
    osMutexId_t mutex;
} Mutex;

typedef struct {
    osThreadId_t thread;
} Thread;

#ifndef MQTT_TASK_STACK_SIZE
#define MQTT_TASK_STACK_SIZE 4096
#endif

#else // OHOS_CMSIS
#define __USE_GNU
#include <pthread.h>

typedef struct {
    pthread_mutex_t mutex;
} Mutex;

typedef struct {
    pthread_t thread;
} Thread;

#endif // OHOS_CMSIS

void MutexInit(Mutex* mutex);
int MutexLock(Mutex* mutex);
int MutexUnlock(Mutex* mutex);
void MutexDeinit(Mutex* mutex);

int ThreadStart(Thread* thread, void (*fn)(void*), void* arg);
void ThreadJoin(Thread* thread);
void ThreadYield(void);
void Sleep(int ms);
#endif

#endif // MQTT_OHOS_H
