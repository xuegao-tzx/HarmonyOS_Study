
#include "hi_wifi_api.h"
#include "lwip/ip_addr.h"
#include "lwip/netifapi.h"
#include "lwip/sockets.h"
#include <stdio.h>

#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "cJSON.h"

#include "sta_mode.h"

void wifi_stop_softap(void);


#define APP_INIT_VAP_NUM    2
#define APP_INIT_USR_NUM    2

static struct netif *g_lwip_netif = NULL;


char recvline[1024];

char ssid[50];
char passwd[50];


void udp_thread(void)
{
    int ret;
    struct sockaddr_in servaddr;
    cJSON *recvjson;

    int sockfd = socket(PF_INET, SOCK_DGRAM, 0);
 
    //服务器 ip port
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(50001);

    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    while(1)
    {
        struct sockaddr_in addrClient;
        int sizeClientAddr = sizeof(struct sockaddr_in);

        memset(recvline, sizeof(recvline), 0);
        ret = recvfrom(sockfd, recvline, 1024, 0, (struct sockaddr*)&addrClient,(socklen_t*)&sizeClientAddr);
        
        if(ret>0)
        {
            char *pClientIP =inet_ntoa(addrClient.sin_addr);
 
            printf("%s-%d(%d) says:%s\n",pClientIP,ntohs(addrClient.sin_port),addrClient.sin_port, recvline);

            //进行json解析
            recvjson = cJSON_Parse(recvline);

            printf("ssid : %s\r\n", cJSON_GetObjectItem(recvjson, "ssid")->valuestring);
            printf("passwd : %s\r\n", cJSON_GetObjectItem(recvjson, "passwd")->valuestring);
            
            memset(ssid, sizeof(ssid), 0);
            memset(passwd, sizeof(passwd), 0);

            strcpy(ssid, cJSON_GetObjectItem(recvjson, "ssid")->valuestring);
            strcpy(passwd, cJSON_GetObjectItem(recvjson, "passwd")->valuestring);

            cJSON_Delete(recvjson);

            //先停止AP模式
            wifi_stop_softap();

            //启动STA模式
            start_sta_connect(ssid, strlen(ssid), passwd, strlen(passwd));
		}
		//sendto(sockfd, recvline, ret, 0, (struct sockaddr *)&addrClient, sizeClientAddr);
    }
}



/* clear netif's ip, gateway and netmask */
void softap_reset_addr(struct netif *pst_lwip_netif)
{
    ip4_addr_t st_gw;
    ip4_addr_t st_ipaddr;
    ip4_addr_t st_netmask;

    if (pst_lwip_netif == NULL) {
        printf("hisi_reset_addr::Null param of netdev\r\n");
        return;
    }

    IP4_ADDR(&st_ipaddr, 0, 0, 0, 0);
    IP4_ADDR(&st_gw, 0, 0, 0, 0);
    IP4_ADDR(&st_netmask, 0, 0, 0, 0);

    netifapi_netif_set_addr(pst_lwip_netif, &st_ipaddr, &st_netmask, &st_gw);
}

int wifi_start_softap(void)
{
    int ret;
    errno_t rc;
    char ifname[WIFI_IFNAME_MAX_SIZE + 1] = {0};
    int len = sizeof(ifname);
    hi_wifi_softap_config hapd_conf = {0};
    ip4_addr_t st_gw;
    ip4_addr_t st_ipaddr;
    ip4_addr_t st_netmask;

    

    rc = memcpy_s(hapd_conf.ssid, HI_WIFI_MAX_SSID_LEN + 1, "Hispark-WiFi-IoT", 16); /* 9:ssid length */
    if (rc != EOK) {
        return -1;
    }

    hapd_conf.authmode = HI_WIFI_SECURITY_OPEN;
    hapd_conf.channel_num = 1;

    ret = hi_wifi_softap_start(&hapd_conf, ifname, &len);
    if (ret != HISI_OK) {
        printf("hi_wifi_softap_start\n");
        return -1;
    }

    /* acquire netif for IP operation */
    g_lwip_netif = netifapi_netif_find(ifname);
    if (g_lwip_netif == NULL) {
        printf("%s: get netif failed\n", __FUNCTION__);
        return -1;
    }

    IP4_ADDR(&st_gw, 192, 168, 10, 1);          /* input your IP for example: 192.168.1.1 */
    IP4_ADDR(&st_ipaddr, 192, 168, 10, 1);      /* input your netmask for example: 192.168.1.1 */
    IP4_ADDR(&st_netmask, 255, 255, 255, 0);     /* input your gateway for example: 255.255.255.0 */
    netifapi_netif_set_addr(g_lwip_netif, &st_ipaddr, &st_netmask, &st_gw);

    netifapi_dhcps_start(g_lwip_netif, 0, 0);

    udp_thread();

    return 0;
}

void wifi_stop_softap(void)
{
    int ret;

    netifapi_dhcps_stop(g_lwip_netif);
    softap_reset_addr(g_lwip_netif);

    ret = hi_wifi_softap_stop();
    if (ret != HISI_OK) {
        printf("failed to stop softap\n");
    }

    ret = hi_wifi_deinit();
    if (ret != HISI_OK) {
        printf("failed to deinit wifi\n");
    }

    g_lwip_netif = NULL;
}

