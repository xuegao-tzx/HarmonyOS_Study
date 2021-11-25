#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

void rtosv2_delay_main(void *arg) {
    (void)arg;

    printf("[Delay Test] Current system tick: %d.\r\n", osKernelGetTickCount());
    osStatus_t status = osDelay(100);
    printf("[Delay Test] osDelay, status: %d.\r\n", status);
    printf("[Delay Test] Current system tick: %d.\r\n", osKernelGetTickCount());

    uint32_t tick = osKernelGetTickCount();
    tick += 100;
    status = osDelayUntil(tick);
    printf("[Delay Test] osDelayUntil, status: %d.\r\n", status);
    printf("[Delay Test] Current system tick: %d.\r\n", osKernelGetTickCount());
}

static void DelayTestTask(void)
{
    osThreadAttr_t attr;

    attr.name = "rtosv2_delay_main";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024;
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)rtosv2_delay_main, NULL, &attr) == NULL) {
        printf("[DelayTestTask] Falied to create rtosv2_delay_main!\n");
    }
}

APP_FEATURE_INIT(DelayTestTask);