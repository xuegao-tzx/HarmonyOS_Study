/*
 * Copyright (c) 2020, HiHope Community.
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

#include <stdio.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "wifiiot_at.h"

#include "httpd.h"
#include "net_params.h"
#include "wifi_connecter.h"

static int g_netId = -1;

#ifdef LWIP_HTTPD_CGI

const char *HelloHandler(int iIndex, int nParams, char *pcParam[], char *pcValue[])
{
    printf("HelloHandler\r\n");
    printf("iIndex = %d\r\n", iIndex);
    for (int i = 0; i < nParams; i++) {
        printf("pcParam[%d] = '%s'\r\n", i, pcParam[i]);
        printf("pcValue[%d] = '%s'\r\n", i, pcValue[i]);
    }
    return "/index.html"; // forward to home page.
}

static tCGI g_cgiHandlers[] = {
    { "/hello", HelloHandler },
};
#endif

#if LWIP_HTTPD_SSI
u16_t FooSsiHandler(
#if LWIP_HTTPD_SSI_RAW
                             const char* tag,
#else /* LWIP_HTTPD_SSI_RAW */
                             int tag,
#endif /* LWIP_HTTPD_SSI_RAW */
                            char *pcInsert, int iInsertLen
#if LWIP_HTTPD_SSI_MULTIPART
                             , u16_t currentTagPart, u16_t *nextTagPart
#endif /* LWIP_HTTPD_SSI_MULTIPART */

#if defined(LWIP_HTTPD_FILE_STATE) && LWIP_HTTPD_FILE_STATE
                             , void *connectionState
#endif /* LWIP_HTTPD_FILE_STATE */
) {
    printf("FooSsiHandler\r\n");

#if LWIP_HTTPD_SSI_RAW
    printf("tag = %s\r\n", tag);
#else
    printf("tag = %d\r\n", tag);
#endif
    printf("insertLen = %d\r\n", iInsertLen);
    printf("insertText = %s\r\n", pcInsert);
    return 0;
}
#endif

static void HttpdTask(void* arg)
{
    (void) arg;
    WifiDeviceConfig config = {0};

    // 准备AP的配置参数
    strcpy(config.ssid, PARAM_HOTSPOT_SSID);
    strcpy(config.preSharedKey, PARAM_HOTSPOT_PSK);
    config.securityType = PARAM_HOTSPOT_TYPE;

    g_netId = ConnectToHotspot(&config);
    printf("netId = %d\r\n", g_netId);

#ifdef LWIP_HTTPD_CGI
    http_set_cgi_handlers(g_cgiHandlers, LWIP_ARRAYSIZE(g_cgiHandlers));
#endif

#ifdef LWIP_HTTPD_SSI
    http_set_ssi_handler(FooSsiHandler, NULL, 0);
#endif

    httpd_init();
}

static void HttpdEntry(void)
{
    osThreadAttr_t attr = {0};

    attr.name = "HttpdTask";
    attr.stack_size = 4096;
    attr.priority = osPriorityNormal;

    if (osThreadNew(HttpdTask, NULL, &attr) == NULL) {
        printf("[HttpdEntry] create HttpdTask failed!\n");
    }
}
SYS_RUN(HttpdEntry);
