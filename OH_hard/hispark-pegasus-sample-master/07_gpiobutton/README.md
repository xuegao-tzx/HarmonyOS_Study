# HiSpark WiFi-IoT 鸿蒙套件样例开发--IO控制（gpio_button）

![hihope_illustration](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/hihope_illustration.png)

[HiSpark WiFi-IoT鸿蒙开发套件](https://item.taobao.com/item.htm?spm=a1z10.1-c-s.w5003-23341819265.1.bf644a82Da9PZK&id=622343426064&scene=taobao_shop) 首发于HDC 2020，是首批支持HarmonyOS 2.0的开发套件，亦是鸿蒙官方推荐套件，由润和软件HiHope量身打造，已在鸿蒙社区和广大鸿蒙开发者中得到广泛应用。

![wifi_iot](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/wifi_iot.png)

## 一、GPIO API

| API名称                                                      | 说明                                                         |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| unsigned int GpioGetInputVal(WifiIotGpioIdx id, WifiIotGpioValue *val); | 获取GPIO引脚状态，id参数用于指定引脚，val参数用于接收GPIO引脚状态 |
| unsigned int IoSetPull(WifiIotIoName id, WifiIotIoPull val); | 设置引脚上拉或下拉状态，id参数用于指定引脚，val参数用于指定上拉或下拉状态 |
| unsigned int GpioRegisterIsrFunc(WifiIotGpioIdx id, WifiIotGpioIntType intType, WifiIotGpioIntPolarity intPolarity, GpioIsrCallbackFunc func, char *arg); | 注册GPIO引脚中断，id参数用于指定引脚，intType参数用于指定中断触发类型（边缘触发或水平触发），intPolarity参数用于指定具体的边缘类型（下降沿或上升沿）或水平类型（高电平或低电平），func参数用于指定中断处理函数，arg参数用于指定中断处理函数的附加参数 |
| typedef void (*GpioIsrCallbackFunc) (char *arg);             | 中断处理函数原型，arg参数为附加参数，可以不适用（填NULL），或传入指向用户自定义类型的参数 |
| unsigned int GpioUnregisterIsrFunc(WifiIotGpioIdx id);       | 解除GPIO引脚中断注册，id参数用于指定引脚                     |

## 二、核心板USER按键与主控芯片（Pegasus）引脚的对应关系

- **USER按键：**GPIO5/按键中断控制LED灯状态反转


## 三、如何编译

1. 将此目录下的 `button_example.c` 和 `BUILD.gn` 复制到openharmony源码的`applications\sample\wifi-iot\app\iothardware`目录下，
2. 修改openharmony源码的`applications\sample\wifi-iot\app\BUILD.gn`文件，将其中的 `features` 改为：

```
    features = [
        "iothardware:button_example",
    ]
```

   3.在openharmony源码顶层目录执行：`python build.py wifiiot`

## 四、运行结果

烧录文件后，按下reset按键，程序开始运行，led灯会先闪烁，在按下USER按键时，led会熄灭，再次按下USER按键，led会亮



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

