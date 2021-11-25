# HiSpark WiFi-IoT 鸿蒙套件样例开发--wifihotspot（AP模式）

![hihope_illustration](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/hihope_illustration.png)

[HiSpark WiFi-IoT鸿蒙开发套件](https://item.taobao.com/item.htm?id=622343426064&scene=taobao_shop) 首发于HDC 2020，是首批支持HarmonyOS 2.0的开发套件，亦是鸿蒙官方推荐套件，由润和软件HiHope量身打造，已在鸿蒙社区和广大鸿蒙开发者中得到广泛应用。 

![wifi_iot](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/wifi_iot.png)

##  一、Wifi STA API

使用鸿蒙原始WiFI API接口进行编程，AP模式需要使用原始接口以及一些DHCP客户端接口。

#### 鸿蒙WiFi AP模式相关的API接口文件路径

**foundation/communication/interfaces/kits/wifi_lite/wifiservice/wifi_hotspot_config.h**

**foundation/communication/interfaces/kits/wifi_lite/wifiservice/wifi_hotspot.h**

所使用的API接口有：

| API                                                          | 接口说明                 |
| ------------------------------------------------------------ | ------------------------ |
| WifiErrorCode EnableHotspot(void);                           | 打开Wifi AP 模式         |
| WifiErrorCode DisableHotspot(void);                          | 关闭Wifi AP 模式         |
| WifiErrorCode SetHotspotConfig(const HotspotConfig* config); | 设置当前AP热点的配置参数 |
| WifiErrorCode GetHotspotConfig(HotspotConfig* result);       | 获取当前AP热点的配置参数 |
| int IsHotspotActive(void);                                   | 查询AP是否已经开启       |
| WifiErrorCode GetStationList(StationInfo* result, unsigned int* size); | 获取接入的设备列表       |
| int GetSignalLevel(int rssi, int band);                      | 获取信号强度等级         |
| WifiErrorCode SetBand(int band);                             | 设置当前频段             |
| WifiErrorCode GetBand(int* result);                          | 获取当前频段             |

#### Hi3861 SDK的DHCP客户端接口：

| API                 | 描述               |
| ------------------- | ------------------ |
| netifapi_netif_find | 按名称查找网络接口 |
| netifapi_dhcp_start | 启动DHCP客户端     |
| netifapi_dhcp_stop  | 停止DHCP客户端     |

## 二、代码分析

```c
//wifi ap task
static void WifiHotspotTask(void *arg)
{
    (void)arg;
    WifiErrorCode errCode;
    HotspotConfig config = {0};

    // 配置作为AP热点的ssid和key
    strcpy(config.ssid, "HiSpark-AP");
    strcpy(config.preSharedKey, "12345678");
    config.securityType = WIFI_SEC_TYPE_PSK;
    config.band = HOTSPOT_BAND_TYPE_2G;
    config.channelNum = 7;

    osDelay(10);

    printf("starting AP ...\r\n");
    //开启热点
    errCode = StartHotspot(&config);
    printf("StartHotspot: %d\r\n", errCode);
    //热点将开启1分钟
    int timeout = 60;
    while (timeout--) {
        printf("After %d seconds Ap will turn off!\r\n", timeout);
        osDelay(100);
    }
    // 可以通过串口工具发送 AT+PING=192.168.xxx.xxx(如手机连接到该热点后的IP) 去ping连接到该热点的设备的IP地址 
    
    printf("stop AP ...\r\n");
    //关闭热点
    StopHotspot();
    printf("stop AP ...\r\n");
}

```



## 三、如何编译

1. 将此目录下的 `wifi_hotspot_demo.c` 和 `BUILD.gn` 复制到openharmony源码的`applications\sample\wifi-iot\app\iothardware`目录下，
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
00 00:00:00 0 132 I 1/SAMGR: Init service:0x4af278 TaskPool:0xe4b38
00 00:00:00 0 132 I 1/SAMGR: Init service:0x4af284 TaskPool:0xe4b58
00 00:00:00 0 132 I 1/SAMGR: Init service:0x4af3ac TaskPool:0xe4b78
00 00:00:00 0 164 I 1/SAMGR: Init service 0x4af284 <time: 0ms> success!
00 00:00:00 0 64 I 1/SAMGR: Init service 0x4af278 <time: 0ms> success!
00 00:00:00 0 8 D 0/HIVIEW: hiview init success.
00 00:00:00 0 8 I 1/SAMGR: Init service 0x4af3ac <time: 0ms> success!
00 00:00:00 0 8 I 1/SAMGR: Initialized all core system services!
00 00:00:00 0 64 I 1/SAMGR: Bootstrap system and application services(count:0).
00 00:00:00 0 64 I 1/SAMGR: Initialized all system and application services!
00 00:00:00 0 64 I 1/SAMGR: Bootstrap dynamic registered services(count:0).
starting AP ...
RegisterWifiEvent: 0
SetHotspotConfig: 0
OnHotspotStateChanged: 1.
EnableHotspot: 0
g_hotspotStarted = 1.
netifapi_netif_set_addr: 0
netifapi_dhcp_start: 0
StartHotspot: 0
After 59 seconds Ap will turn off!
After 58 seconds Ap will turn off!
After 57 seconds Ap will turn off!
+NOTICE:STA CONNECTED
 PrintStationInfo: mac=54:19:C8:D9:A7:35, reason=0.
+OnHotspotStaJoin: active stations = 1.
After 56 seconds Ap will turn off!
After 55 seconds Ap will turn off!

AT+PING=192.168.1.2

+PING:
[0]Reply from 192.168.1.2:time=90ms TTL=64
After 49 seconds Ap will turn off!
[1]Reply from 192.168.1.2:time=25ms TTL=64
After 48 seconds Ap will turn off!
[2]Reply from 192.168.1.2:time=28ms TTL=64
After 47 seconds Ap will turn off!
[3]Reply from 192.168.1.2:time=4ms TTL=64
4 packets transmitted, 4 received, 0 loss, rtt min/avg/max = 4/36/90 ms

OK
...
...
...
After 3 seconds Ap will turn off!
After 2 seconds Ap will turn off!
After 1 seconds Ap will turn off!
After 0 seconds Ap will turn off!
stop AP ...
netifapi_dhcps_stop: 0
UnRegisterWifiEvent: 0
+NOTICE:STA DISCONNECTED
EnableHotspot: 0
stop AP ...


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
