# HiSpark WiFi-IoT 鸿蒙套件样例开发--wificonnect(STA模式)                    

![hihope_illustration](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/hihope_illustration.png)

[HiSpark WiFi-IoT鸿蒙开发套件](https://item.taobao.com/item.htm?id=622343426064&scene=taobao_shop) 首发于HDC 2020，是首批支持HarmonyOS 2.0的开发套件，亦是鸿蒙官方推荐套件，由润和软件HiHope量身打造，已在鸿蒙社区和广大鸿蒙开发者中得到广泛应用。

![wifi_iot](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/wifi_iot.png) 



##  一、Wifi STA API

使用鸿蒙原始WiFI API接口进行编程，STA模式需要使用原始STA接口以及一些DHCP客户端接口。

#### 鸿蒙WiFi STA模式相关的API接口文件路径

**foundation/communication/interfaces/kits/wifi_lite/wifiservice/wifi_device.h**
所使用的API接口有：


| API                                                          | 接口说明                                |
| ------------------------------------------------------------ | --------------------------------------- |
| WifiErrorCode EnableWifi(void);                              | 开启STA                                 |
| WifiErrorCode DisableWifi(void);                             | 关闭STA                                 |
| int IsWifiActive(void);                                      | 查询STA是否已开启                       |
| WifiErrorCode Scan(void);                                    | 触发扫描                                |
| WifiErrorCode GetScanInfoList(WifiScanInfo* result, unsigned int* size); | 获取扫描结果                            |
| WifiErrorCode AddDeviceConfig(const WifiDeviceConfig* config, int* result); | 添加热点配置，成功会通过result传出netld |
| WifiErrorCode GetDeviceConfigs(WifiDeviceConfig* result, unsigned int* size); | 获取本机所有热点配置                    |
| WifiErrorCode RemoveDevice(int networkId);                   | 删除热点配置                            |
| WifiErrorCode ConnectTo(int networkId);                      | 连接到热点                              |
| WifiErrorCode Disconnect(void);                              | 断开热点连接                            |
| WifiErrorCode GetLinkedInfo(WifiLinkedInfo* result);         | 获取当前连接热点信息                    |
| WifiErrorCode RegisterWifiEvent(WifiEvent* event);           | 注册事件监听                            |
| WifiErrorCode UnRegisterWifiEvent(const WifiEvent* event);   | 解除事件监听                            |
| WifiErrorCode GetDeviceMacAddress(unsigned char* result);    | 获取Mac地址                             |
| WifiErrorCode AdvanceScan(WifiScanParams *params);           | 高级搜索                                |

#### Hi3861 SDK的DHCP客户端接口：

| API                 | 描述               |
| ------------------- | ------------------ |
| netifapi_netif_find | 按名称查找网络接口 |
| netifapi_dhcp_start | 启动DHCP客户端     |
| netifapi_dhcp_stop  | 停止DHCP客户端     |

## 二、代码分析

```c
//wifi connect task
static void WifiConnectTask(void *arg)
{
    (void)arg;
    WifiErrorCode errCode;
    WifiEvent eventListener = {
        .OnWifiConnectionChanged = OnWifiConnectionChanged,
        .OnWifiScanStateChanged = OnWifiScanStateChanged
    };
    WifiDeviceConfig apConfig = {};
    int netId = -1;

    osDelay(10);
    errCode = RegisterWifiEvent(&eventListener);
    printf("RegisterWifiEvent: %d\r\n", errCode);

    // 配置要连接的热点的账号密码
    strcpy(apConfig.ssid, "ABCD");
    strcpy(apConfig.preSharedKey, "12345678");
    apConfig.securityType = WIFI_SEC_TYPE_PSK;

    while (1) {
    	//打开wifi
        errCode = EnableWifi();
        printf("EnableWifi: %d\r\n", errCode);
        osDelay(11);
		//添加热点配置
        errCode = AddDeviceConfig(&apConfig, &netId);
        printf("AddDeviceConfig: %d\r\n", errCode);

        g_connected = 0;
        //连接热点
        errCode = ConnectTo(netId);
        printf("ConnectTo(%d): %d\r\n", netId, errCode);
		//等待wifi连接
        while (!g_connected) {
            osDelay(10);
        }
        printf("g_connected: %d\r\n", g_connected);
        osDelay(50);


        // 联网业务开始
        struct netif* iface = netifapi_netif_find("wlan0");
        if (iface) {
            err_t ret = netifapi_dhcp_start(iface);
            printf("netifapi_dhcp_start: %d\r\n", ret);

            osDelay(200); // wait DHCP server give me IP
            ret = netifapi_netif_common(iface, dhcp_clients_info_show, NULL);
            printf("netifapi_netif_common: %d\r\n", ret);
        }

        // 模拟一段时间的联网业务
        int timeout = 60;
        while (timeout--) {
            osDelay(100);
            printf("after %d seconds, I'll disconnect WiFi!\n", timeout);
        }

        // 联网业务结束
        err_t ret = netifapi_dhcp_stop(iface);
        printf("netifapi_dhcp_stop: %d\r\n", ret);

        Disconnect(); // disconnect with your AP

        RemoveDevice(netId); // remove AP config
		
        errCode = DisableWifi(); //close wifi
        printf("DisableWifi: %d\r\n", errCode);
        osDelay(200);
    }
}

```



## 三、如何编译

1. 将此目录下的 `wifi_connect_demo.c` 和 `BUILD.gn` 复制到openharmony源码的`applications\sample\wifi-iot\app\iothardware`目录下，
2. 修改openharmony源码的`applications\sample\wifi-iot\app\BUILD.gn`文件，将其中的 `features` 改为：

```
    features = [
        "iothardware:wifi_demo",
    ]
```

   3.在openharmony源码顶层目录执行：`python build.py wifiiot`

## 四、运行结果

```

ready to OS start
sdk ver:Hi3861V100R001C00SPC025 2020-09-03 18:10:00
FileSystem mount ok.
wifi init success!

00 00:00:00 0 132 D 0/HIVIEW: hilog init success.
00 00:00:00 0 132 D 0/HIVIEW: log limit init success.
00 00:00:00 0 132 I 1/SAMGR: Bootstrap core services(count:3).
00 00:00:00 0 132 I 1/SAMGR: Init service:0x4af3e8 TaskPool:0xe4f38
00 00:00:00 0 132 I 1/SAMGR: Init service:0x4af3f4 TaskPool:0xe4f58
00 00:00:00 0 132 I 1/SAMGR: Init service:0x4af51c TaskPool:0xe4f78
00 00:00:00 0 164 I 1/SAMGR: Init service 0x4af3f4 <time: 0ms> success!
00 00:00:00 0 64 I 1/SAMGR: Init service 0x4af3e8 <time: 0ms> success!
00 00:00:00 0 8 D 0/HIVIEW: hiview init success.
00 00:00:00 0 8 I 1/SAMGR: Init service 0x4af51c <time: 0ms> success!
00 00:00:00 0 8 I 1/SAMGR: Initialized all core system services!
00 00:00:00 0 64 I 1/SAMGR: Bootstrap system and application services(count:0).
00 00:00:00 0 64 I 1/SAMGR: Initialized all system and application services!
00 00:00:00 0 64 I 1/SAMGR: Bootstrap dynamic registered services(count:0).
RegisterWifiEvent: 0
EnableWifi: 0
AddDeviceConfig: 0
ConnectTo(0): 0
+NOTICE:SCANFINISH
+NOTICE:CONNECTED
OnWifiConnectionChanged 58, state = 1, info = 
bssid: 08:1F:71:24:B8:29, rssi: 0, connState: 0, reason: 0, ssid: ABCD
g_connected: 1
netifapi_dhcp_start: 0
server :
	server_id : 192.168.0.1
	mask : 255.255.255.0, 1
	gw : 192.168.0.1
	T0 : 7200
	T1 : 3600
	T2 : 6300
clients <1> :
	mac_idx mac             addr            state   lease   tries   rto     
	0       b4c9b96199d6    192.168.0.107   10      0       1       2       
netifapi_netif_common: 0
after 59 seconds, I'll disconnect WiFi!
after 58 seconds, I'll disconnect WiFi!
after 57 seconds, I'll disconnect WiFi!
...
...
...
after 3 seconds, I'll disconnect WiFi!
after 2 seconds, I'll disconnect WiFi!
after 1 seconds, I'll disconnect WiFi!
after 0 seconds, I'll disconnect WiFi!
netifapi_dhcp_stop: 0
+NOTICE:DISCONNECTED
OnWifiConnectionChanged 58, state = 0, info = 
bssid: 08:1F:71:24:B8:29, rssi: 0, connState: 0, reason: 3, ssid: 
DisableWifi: 0

```





### 【套件支持】

##### 1. 套件介绍  http://www.hihope.org/pro/pro1.aspx?mtt=8

##### 2. 套件购买  https://item.taobao.com/item.htm?id=622343426064&scene=taobao_shop

##### 3. 技术资料

- Gitee码云网站（OpenHarmony Sample Code等) **https://gitee.com/hihopeorg**

- HiHope官网-资源中心（SDK包、技术文档下载）**http://www.hihope.org/**

##### 4. 互动交流

- 润和HiHope鸿蒙技术交流-微信群（加群管理员微信13605188699，发送文字#申请加入润和官方鸿蒙群#，予以邀请入群）
- HiHope开发者社区-论坛 **https://bbs.elecfans.com/group_1429**
- 润和HiHope鸿蒙售后服务群（QQ：980599547）
- 售后服务电话（025-52668590）
