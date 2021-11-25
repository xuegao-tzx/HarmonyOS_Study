# HiSpark WiFi-IoT 鸿蒙套件样例开发--IO控制（gpio_led）

![hihope_illustration](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/hihope_illustration.png)

[HiSpark WiFi-IoT鸿蒙开发套件](https://item.taobao.com/item.htm?spm=a1z10.1-c-s.w5003-23341819265.1.bf644a82Da9PZK&id=622343426064&scene=taobao_shop) 首发于HDC 2020，是首批支持HarmonyOS 2.0的开发套件，亦是鸿蒙官方推荐套件，由润和软件HiHope量身打造，已在鸿蒙社区和广大鸿蒙开发者中得到广泛应用。

![wifi_iot](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/wifi_iot.png)

## 一、GPIO API

| API名称                                                      | 说明                                                         |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| unsigned int GpioInit(void);                                 | GPIO模块初始化                                               |
| unsigned int GpioSetDir(WifiIotGpioIdx id, WifiIotGpioDir dir); | 设置GPIO引脚方向，id参数用于指定引脚，dir参数用于指定输入或输出 |
| unsigned int GpioSetOutputVal(WifiIotGpioIdx id, WifiIotGpioValue val); | 设置GPIO引脚的输出状态，id参数用于指定引脚，val参数用于指定高电平或低电平 |
| unsigned int IoSetFunc(WifiIotIoName id, unsigned char val); | 设置引脚功能，id参数用于指定引脚，val用于指定引脚功能        |
| unsigned int GpioDeinit(void);                               | 解除GPIO模块初始化                                           |

## 二、核心板LED与主控芯片（Pegasus）引脚的对应关系

- **LED：**GPIO9/输出低电平点亮LED灯


## 三、如何编译

1. 将此目录下的 `led_example.c` 和 `BUILD.gn` 复制到openharmony源码的`applications\sample\wifi-iot\app\iothardware`目录下，
2. 修改openharmony源码的`applications\sample\wifi-iot\app\BUILD.gn`文件，将其中的 `features` 改为：

```
    features = [
        "iothardware:led_example",
    ]
```

   3.在openharmony源码顶层目录执行：`python build.py wifiiot`

## 四、运行结果

烧录文件后，按下reset按键，会发现核心板上的led等一闪一闪的

串口工具也会打印：LED_SPARK

### 【套件支持】

##### 1. 套件介绍  http://www.hihope.org/pro/pro1.aspx?mtt=8

##### 2. 套件购买  https://item.taobao.com/item.htm?id=622343426064&scene=taobao_shop

##### 3. 技术资料

- Gitee码云网站（OpenHarmony Sample Code等) **https://gitee.com/hihopeorg**

- HiHope官网-资源中心（SDK包、技术文档下载）[**www.hihope.org**](http://www.hihope.org/)

##### 4. 互动交流

- 润和HiHope鸿蒙技术交流-微信群（加群管理员微信13605188699，发送文字#申请加入润和官方鸿蒙群#，予以邀请入群）
- HiHope开发者社区-论坛 **https://bbs.elecfans.com/group_1429**
- 润和HiHope鸿蒙售后服务群（QQ：980599547）
- 售后服务电话（025-52668590）

