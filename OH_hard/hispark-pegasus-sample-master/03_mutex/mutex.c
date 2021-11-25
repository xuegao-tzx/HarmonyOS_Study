#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

static int g_test_value = 0;

void number_thread(void *arg) {
    osMutexId_t *mid = (osMutexId_t *)arg;
    while(1) {
        if (osMutexAcquire(*mid, 100) == osOK) {
            g_test_value++;
            if (g_test_value % 2 == 0) {
                printf("[Mutex Test]%s gets an even value %d.\r\n", osThreadGetName(osThreadGetId()), g_test_value);
            } else {
                printf("[Mutex Test]%s gets an odd value %d.\r\n",  osThreadGetName(osThreadGetId()), g_test_value);
            }
            osMutexRelease(*mid);
            osDelay(5);
        }
    }
}

osThreadId_t newThread(char *name, osThreadFunc_t func, void *arg) {
    osThreadAttr_t attr = {
        name, 0, NULL, 0, NULL, 1024*2, osPriorityNormal, 0, 0
    };
    osThreadId_t tid = osThreadNew(func, arg, &attr);
    if (tid == NULL) {
        printf("[Mutex Test]osThreadNew(%s) failed.\r\n", name);
    } else {
        printf("[Mutex Test]osThreadNew(%s) success, thread id: %d.\r\n", name, tid);
    }
    return tid;
}

void rtosv2_mutex_main(void *arg) {
    (void)arg;
    osMutexAttr_t attr = {0};

    osMutexId_t mid = osMutexNew(&attr);
    if (mid == NULL) {
        printf("[Mutex Test]osMutexNew, create mutex failed.\r\n");
    } else {
        printf("[Mutex Test]osMutexNew, create mutex success.\r\n");
    }

    osThreadId_t tid1 = newThread("Thread_1", number_thread, &mid);
    osThreadId_t tid2 = newThread("Thread_2", number_thread, &mid);
    osThreadId_t tid3 = newThread("Thread_3", number_thread, &mid);

    osDelay(13);
    osThreadId_t tid = osMutexGetOwner(mid);
    printf("[Mutex Test]osMutexGetOwner, thread id: %p, thread name: %s.\r\n", tid, osThreadGetName(tid));
    osDelay(17);

    osThreadTerminate(tid1);
    osThreadTerminate(tid2);
    osThreadTerminate(tid3);
    osMutexDelete(mid);
}

static void MutexTestTask(void)
{
    osThreadAttr_t attr;

    attr.name = "rtosv2_mutex_main";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024;
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)rtosv2_mutex_main, NULL, &attr) == NULL) {
        printf("[MutexTestTask] Falied to create rtosv2_mutex_main!\n");
    }
}

APP_FEATURE_INIT(MutexTestTask);