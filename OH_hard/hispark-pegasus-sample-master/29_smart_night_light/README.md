# HiSpark WiFi-IoT HarmonyOS 套件样例开发--智能夜灯

![hihope_illustration](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/hihope_illustration.png)

[HiSpark WiFi-IoTHarmonyOS 开发套件](https://item.taobao.com/item.htm?spm=a1z10.1-c-s.w5003-23341819265.1.bf644a82Da9PZK&id=622343426064&scene=taobao_shop) 首发于HDC 2020，是首批支持HarmonyOS  2.0的开发套件，亦是HarmonyOS 官方推荐套件，由润和软件HiHope量身打造，已在HarmonyOS 社区和广大HarmonyOS 开发者中得到广泛应用。

![wifi_iot](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/wifi_iot.png)



## 下载源码

建议将本教程的设备源码下载后，放在applications/sample/wifi-iot/app目录下：

* 执行命令`git clone https://gitee.com/hihope_iot/hispark-pegasus-sample/tree/master/29_smart_night_light`

* APP源码下载：

  `git clone https://gitee.com/hihopeorg_group/hcia_harmonyos_application`



## 文件说明

udpserver_env目录说明：

| 文件名             | 说明                                           |
| ------------------ | ---------------------------------------------- |
| BUILD.gn           | HarmonyOS 构建脚本                             |
| cjson.c            | json格式数据解析、封装                         |
| config_params.h    | 业务参数配置                                   |
| demo_entry_cmsis.c | HarmonyOS liteos-m程序入口                     |
| net_common.h       | 系统网络接口头文件                             |
| net_demo.h         | demo脚手架头文件                               |
| net_params.h       | 网络参数，包括WiFi热点信息，服务器IP、端口信息 |
| udp_server_test.c  | UDP服务端                                      |
| wifi_connecter.c   | HarmonyOS WiFi STA模式API的封装实现文件        |
| wifi_connecter.h   | HarmonyOS WiFi STA模式API的封装头文件          |
| wifi_starter.c     | HarmonyOS WiFi AP模式API的封装实现文件         |
| wifi_starter.h     | HarmonyOS WiFi AP模式API的封装头文件           |

## 实验指南

### 准备网络环境

1. 准备一个无线路由器，
2. harmony os 手机一部

### 编译和烧录测试程序

在Hi3861开发板上运行程序，需要分别编译、烧录程序；

1. 需要修改`applications/sample/wifi-iot/app/BUILD.gn`代码，注释掉不需要运行的代码：

   添加`"night_light",`

   ​       ` "udpserver_light:net_demo"`

2. 使用DevEco Device Tool 进行一键式编译或者执行`hb build -f`

3. 使用DevEco Device Tool或者HiBurn将二进制程序烧录到Hi3861开发板上；

4. 烧录成功后请复位程序

### 测试APP安装

将提供的harmony os APP 安装到手机上

### 运行测试程序

1. 复位开发板上的程序后，手机连接上开发板的热点`HarmonyOS-AP`,然后打开安装好的APP，配置新的热点名称与密码，点击`配网`按键

   将热点的名称与密码下发到板端，然后关闭APP

2. 板端接收到APP下发的热点与密码之后会重启设备，并连接上配置好的热点，手机同样连接上配置的热点，再次打开APP。板端与手机APP建立通信

3. APP切换到智能夜灯界面，可以调节灯的亮度和时长，在无光有人经过的情况下灯会依据当前的亮度以及时长保持相应的状态。


### 【套件支持】

##### 1. 套件介绍  http://www.hihope.org/pro/pro1.aspx?mtt=8

##### 2. 套件购买  https://item.taobao.com/item.htm?id=622343426064&scene=taobao_shop

##### 3. 技术资料

- Gitee码云网站（OpenHarmony Sample Code等) **https://gitee.com/hihopeorg**

- HiHope官网-资源中心（SDK包、技术文档下载）[**www.hihope.org**](http://www.hihope.org/)

##### 4. 互动交流

- 润和HiHopeHarmonyOS 技术交流-微信群（加群管理员微信13605188699，发送文字#申请加入润和官方HarmonyOS 群#，予以邀请入群）
- HiHope开发者社区-论坛 **https://bbs.elecfans.com/group_1429**
- 润和HiHopeHarmonyOS 售后服务群（QQ：980599547）
- 售后服务电话（025-52668590）