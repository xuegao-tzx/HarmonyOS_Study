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
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>

static long gettid(void)
{
    return syscall(SYS_gettid);
}
#define LOGI(fmt, ...) printf("[%ld] " fmt "\n", gettid(), ##__VA_ARGS__)

#if defined(MQTT_TASK)

#define CHECK(expr) (({ \
    int rc = (expr); \
    if (rc != 0) { \
        printf("%s: %d failed: %s!\n", #expr, rc, strerror(rc)); \
    } \
    rc; \
}))

void MutexInit(Mutex* m)
{
    CHECK(pthread_mutex_init(&m->mutex, NULL));
}

int MutexLock(Mutex* m)
{
    return CHECK(pthread_mutex_lock(&m->mutex));
}

int MutexUnlock(Mutex* m)
{
    return CHECK(pthread_mutex_unlock(&m->mutex));
}

void MutexDeinit(Mutex* m)
{
    CHECK(pthread_mutex_destroy(&m->mutex));
}

int ThreadStart(Thread* t, void (*fn)(void*), void* arg)
{
    return CHECK(pthread_create(&t->thread, NULL, (void*(*)(void*))fn, arg));
}

void ThreadJoin(Thread* t)
{
    CHECK(pthread_join(t->thread, NULL));
}

void ThreadYield()
{
#if (defined __linux__)
    CHECK(pthread_yield());
#endif
}

void Sleep(int ms)
{
    usleep(ms * 1000);
}
#endif
