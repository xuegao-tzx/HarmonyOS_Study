# HiSpark WiFi-IoT 鸿蒙套件样例开发--oled_player

![hihope_illustration](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/hihope_illustration.png)

[HiSpark WiFi-IoT鸿蒙开发套件](https://item.taobao.com/item.htm?spm=a1z10.1-c-s.w5003-23341819265.1.bf644a82Da9PZK&id=622343426064&scene=taobao_shop) 首发于HDC 2020，是首批支持HarmonyOS 2.0的开发套件，亦是鸿蒙官方推荐套件，由润和软件HiHope量身打造，已在鸿蒙社区和广大鸿蒙开发者中得到广泛应用。

![wifi_iot](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/wifi_iot.png)

## 一、简介

* 基于HiSpark WiFi IoT套件（Hi3861芯片），板载0.96寸 128x64 分辨率的 OLED屏
* 板端程序使用了[鸿蒙OS SSD1306 OLED屏驱动库](https://gitee.com/hihopeorg/hispark-hm-pegasus/tree/master/sample/12_ssd1306)，用于实现每帧画面的绘制；
* PC端使用了`opencv-python`，用于实现视频解码、画面缩放、二值化和帧数据打包；

## 二、如何编译

1. 将此目录复制到openharmony源码的根目录下
2. 修改openharmony源码的`build\lite\product\wifiiot.json`文件：
   * 将其中的`//applications/sample/wifi-iot/app`替换为`//13_oledplayer:app`

3. 在openharmony源码顶层目录下执行：`python build.py wifiiot`

## 三、如何运行

网络环境：一个无线热点，一台PC，PC连接在该热点上；

### 准备视频资源

1. 选择准备播放的视频，使用工具将帧率转为10fps；
   * 目前ssd1306库实测的最大帧率为10fps；
   * ffmpeg转换命令：`ffmpeg -i input.mp4 -r 10 output.mp4`
   * ffmpeg在Ubuntu上可以直接试用`sudo apt install ffmpeg`安装，Windows上可以在官网下载二进制包：https://ffmpeg.org/download.html#build-windows
2. 运行命令：`./video2bin.py output.mp4 out.bin`，将视频转为bin文件；
3. video2bin.py 依赖 `opencv-python` 包，使用 `pip install opencv-python` 命令安装；


### 运行程序

1. PC上运行命令：`./bin2stream.py out.bin`，将会启动一个TCP服务器，默认监听`5678`端口
2. 根据热点信息（SSID,PSK）和PC的IP地址，修改`play/net_params.h`文件中的相关参数：
3. 重新编译：`python build.py wifiiot`
4. 将重新编译好的固件烧录到WiFi IoT开发板上，并复位设备；
   * 板子启动后，首先会连上你的热点，然后会连接PC上的TCP服务；
   * 然后就可以看到视频的在OLED屏播放了；



## 四、原理介绍

整体为C/S架构：

* 使用TCP传输帧数据，实现了简单的二进制协议：
  * 请求格式：帧ID（4B）；
  * 响应格式：状态码（4B） + 帧数据长度（4B）+ 帧数据（可选）；
* PC上运行服务端，默认监听`5678`端口，使用Python开发；
  * 启动时加载整个bin文件，并将其按照帧数据大小分割，放入一个list中；
  * 客户端请求特定帧时，直接根据下标索引取出对应帧，并发送给客户端；
  * 这样的设计（视频预先转换好），可以保证服务端的响应尽可能快；
* 板上运行客户端；
  * 主动发送帧请求，并接收服务端的回应；
  * 收到帧数据后通过I2C向OLED屏发送帧数据；

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

