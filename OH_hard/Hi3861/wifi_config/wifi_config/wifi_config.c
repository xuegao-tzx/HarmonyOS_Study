
#include <stdio.h>

#include <unistd.h>
#include <hi_types_base.h>
#include <hi_io.h>
#include <hi_early_debug.h>
#include <hi_gpio.h>
#include <hi_task.h>
#include "ohos_init.h"
#include "cmsis_os2.h"

#include <unistd.h>
#include "hi_wifi_api.h"
//#include "wifi_sta.h"
#include "lwip/ip_addr.h"
#include "lwip/netifapi.h"

#include "lwip/sockets.h"

#include "ap_mode.h"


volatile char start_wifi_config_flg = 0;

/* gpio callback func */
hi_void my_gpio_isr_func(hi_void *arg)
{
    hi_unref_param(arg);
    printf("----- gpio isr success -----\r\n");

    //启动配网功能
    start_wifi_config_flg = 1;
}

/* 设置 按键中断响应 */
hi_void my_gpio_isr_demo(hi_void)
{
    hi_u32 ret;

    printf("----- gpio isr demo -----\r\n");

    (hi_void)hi_gpio_init();
    
    hi_io_set_func(HI_IO_NAME_GPIO_5, HI_IO_FUNC_GPIO_5_GPIO); /* uart1 rx */

    ret = hi_gpio_set_dir(HI_GPIO_IDX_5, HI_GPIO_DIR_IN);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ======gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }

    ret = hi_gpio_register_isr_function(HI_GPIO_IDX_5, HI_INT_TYPE_EDGE,
                                        HI_GPIO_EDGE_RISE_LEVEL_HIGH, my_gpio_isr_func, HI_NULL);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ======gpio -> hi_gpio_register_isr_function ret:%d\r\n", ret);
    }
}

void *wifi_config_thread(const char *arg)
{
    arg = arg;

    my_gpio_isr_demo();

    
    while(start_wifi_config_flg == 0)
    {
        usleep(300000);
    }

    printf("wifi_start_softap \r\n");
    wifi_start_softap();

    osThreadExit();
    return NULL;
}



void wifi_config_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "wifi_config_thread";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 4096;
    attr.priority = 36;

    if (osThreadNew((osThreadFunc_t)wifi_config_thread, NULL, &attr) == NULL) {
        printf("[LedExample] Falied to create LedTask!\n");
    }
}


SYS_RUN(wifi_config_entry);

