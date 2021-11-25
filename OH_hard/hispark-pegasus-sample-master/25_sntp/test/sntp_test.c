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

#include "sntp.h"
#include "net_params.h"
#include "wifi_connecter.h"
#include "lwip/pbuf.h"
#include "lwip/dns.h"
#include "lwip/ip4_addr.h"

#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])

static int g_netId = -1;

#if SNTP_SERVER_DNS
static const char* g_ntpServerList[] = {
    // refers from https://dns.icoa.cn/ntp/#china
    "cn.ntp.org.cn", // 中国 NTP 快速授时服务
    "ntp.ntsc.ac.cn", // 国家授时中心 NTP 服务器
    "time.pool.aliyun.com", // 阿里云公共 NTP 服务器
    "cn.pool.ntp.org", // 国际 NTP 快速授时服务
};
#define SNTP_SERVERS ARRAY_SIZE(g_ntpServerList)

void SntpSetServernames(void)
{
    for (size_t i = 0; i < SNTP_SERVERS; i++) {
        sntp_setservername(i, g_ntpServerList[i]);
    }
}

#else

ip4_addr_t g_ntpServerList[SNTP_MAX_SERVERS];

void SntpSetServers(void)
{
    IP4_ADDR(&g_ntpServerList[0], 114, 67, 237, 130); // cn.ntp.org.cn
    IP4_ADDR(&g_ntpServerList[1], 114, 118, 7, 163);  // ntp.ntsc.ac.cn
    IP4_ADDR(&g_ntpServerList[2], 182, 92, 12, 11); // time.pool.aliyun.com
    IP4_ADDR(&g_ntpServerList[3], 193, 182, 111, 12); // cn.pool.ntp.org
#define SNTP_SERVERS 4
    for (size_t i = 0; i < SNTP_SERVERS; i++) {
        sntp_setserver(i, (ip_addr_t*)&g_ntpServerList[i]);
    }
}
#endif

static void SntpTask(void* arg)
{
    (void) arg;
    WifiDeviceConfig config = {0};

    // 准备AP的配置参数
    strcpy(config.ssid, PARAM_HOTSPOT_SSID);
    strcpy(config.preSharedKey, PARAM_HOTSPOT_PSK);
    config.securityType = PARAM_HOTSPOT_TYPE;

    g_netId = ConnectToHotspot(&config);
    printf("netId = %d\r\n");

#if SNTP_SERVER_DNS
    ip4_addr_t dnsServerAddr;
    IP4_ADDR(&dnsServerAddr, 192, 168, 1, 1);
    dns_setserver(0, (struct ip_addr *)&dnsServerAddr);
    dns_init();

    SntpSetServernames();
#else
    SntpSetServers();
#endif

    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_init();

    printf("sntp_enabled: %d\r\n", sntp_enabled());
    for (size_t i = 0; i < SNTP_SERVERS; i++) {
        printf("sntp_getreachability(%d): %d\r\n", i, sntp_getreachability(i));
    }

    osDelay(500);
    for (size_t i = 0; i < SNTP_SERVERS; i++) {
        printf("sntp_getreachability(%d): %d\r\n", i, sntp_getreachability(i));
    }
}

static void SntpEntry(void)
{
    osThreadAttr_t attr = {0};

    attr.name = "SntpTask";
    attr.stack_size = 4096;
    attr.priority = osPriorityNormal;

    if (osThreadNew(SntpTask, NULL, &attr) == NULL) {
        printf("[SntpEntry] create SntpTask failed!\n");
    }
}
SYS_RUN(SntpEntry);
