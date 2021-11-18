
#include <stdio.h>

#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include <unistd.h>
#include "hi_wifi_api.h"
//#include "wifi_sta.h"
#include "lwip/ip_addr.h"
#include "lwip/netifapi.h"


static struct netif *g_lwip_netif = NULL;

/* clear netif's ip, gateway and netmask */
void hi_sta_reset_addr(struct netif *pst_lwip_netif)
{
    ip4_addr_t st_gw;
    ip4_addr_t st_ipaddr;
    ip4_addr_t st_netmask;
    printf("%s %d \r\n", __FILE__, __LINE__);
    if (pst_lwip_netif == NULL) {
        printf("hisi_reset_addr::Null param of netdev\r\n");
        return;
    }

    IP4_ADDR(&st_gw, 0, 0, 0, 0);
    IP4_ADDR(&st_ipaddr, 0, 0, 0, 0);
    IP4_ADDR(&st_netmask, 0, 0, 0, 0);

    netifapi_netif_set_addr(pst_lwip_netif, &st_ipaddr, &st_netmask, &st_gw);
}

void wifi_wpa_event_cb(const hi_wifi_event *hisi_event)
{
    if (hisi_event == NULL)
        return;

    switch (hisi_event->event) {
        case HI_WIFI_EVT_SCAN_DONE:
            printf("WiFi: Scan results available\n");
            break;
        case HI_WIFI_EVT_CONNECTED:
            printf("WiFi: Connected\n");
            netifapi_dhcp_start(g_lwip_netif);
            break;
        case HI_WIFI_EVT_DISCONNECTED:
            printf("WiFi: Disconnected\n");
            netifapi_dhcp_stop(g_lwip_netif);
            hi_sta_reset_addr(g_lwip_netif);
            break;
        case HI_WIFI_EVT_WPS_TIMEOUT:
            printf("WiFi: wps is timeout\n");
            break;
        default:
            break;
    }
}

int hi_wifi_start_connect(void)
{
    int ret;
    errno_t rc;
    hi_wifi_assoc_request assoc_req = {0};

    /* copy SSID to assoc_req */
    //热点名称
    rc = memcpy_s(assoc_req.ssid, HI_WIFI_MAX_SSID_LEN + 1, "15919500", 8); /* 9:ssid length */
    if (rc != EOK) {
        printf("%s %d \r\n", __FILE__, __LINE__);
        return -1;
    }

    /*
     * OPEN mode
     * for WPA2-PSK mode:
     * set assoc_req.auth as HI_WIFI_SECURITY_WPA2PSK,
     * then memcpy(assoc_req.key, "12345678", 8).
     */
    //热点加密方式
    assoc_req.auth = HI_WIFI_SECURITY_WPA2PSK;

    /* 热点密码 */
    memcpy(assoc_req.key, "11206582488", 11);


    ret = hi_wifi_sta_connect(&assoc_req);
    if (ret != HISI_OK) {
        printf("%s %d \r\n", __FILE__, __LINE__);
        return -1;
    }
    printf("%s %d \r\n", __FILE__, __LINE__);
    return 0;
}

void sta_demo(void)
{
    int ret;
    char ifname[WIFI_IFNAME_MAX_SIZE + 1] = {0};
    int len = sizeof(ifname);
    unsigned int  num = WIFI_SCAN_AP_LIMIT;

    ret = hi_wifi_sta_start(ifname, &len);
    if (ret != HISI_OK) {
        printf("%s %d \r\n", __FILE__, __LINE__);
        return;
    }

    /* register call back function to receive wifi event, etc scan results event,
     * connected event, disconnected event.
     */
    ret = hi_wifi_register_event_callback(wifi_wpa_event_cb);
    if (ret != HISI_OK) {
        printf("register wifi event callback failed\n");
    }

    /* acquire netif for IP operation */
    g_lwip_netif = netifapi_netif_find(ifname);
    if (g_lwip_netif == NULL) {
        printf("%s: get netif failed\n", __FUNCTION__);
        return ;
    }

    /* start scan, scan results event will be received soon */
    ret = hi_wifi_sta_scan();
    if (ret != HISI_OK) {
        printf("%s %d \r\n", __FILE__, __LINE__);
        return ;
    }

    sleep(5);   /* sleep 5s, waiting for scan result. */

    hi_wifi_ap_info *pst_results = malloc(sizeof(hi_wifi_ap_info) * WIFI_SCAN_AP_LIMIT);
    if (pst_results == NULL) {
        printf("%s %d \r\n", __FILE__, __LINE__);
        return ;
    }

    ret = hi_wifi_sta_scan_results(pst_results, &num);
    if (ret != HISI_OK) {
        printf("%s %d \r\n", __FILE__, __LINE__);
        free(pst_results);
        return ;
    }

    for (unsigned int loop = 0; (loop < num) && (loop < WIFI_SCAN_AP_LIMIT); loop++) {
        printf("SSID: %s\n", pst_results[loop].ssid);
    }
    free(pst_results);

    /* if received scan results, select one SSID to connect */
    ret = hi_wifi_start_connect();
    if (ret != 0) {
        printf("%s %d \r\n", __FILE__, __LINE__);
        return ;
    }


    return;

}


SYS_RUN(sta_demo);

