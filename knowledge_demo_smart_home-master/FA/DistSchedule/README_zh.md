# 数字管家手机端介绍

### 简介

数字管家场景需要手机端、设备端和服务端三方协同完成，本文档介绍的demo是数字管家大场景中的手机端应用；用户注册登录后可创建我的家庭并管理家庭成员；可以添加设备（包括智能台灯，智能窗帘，智能养花机等）并进行配网和控制；添加日程编排、控制设备，从而达到多设备联动的效果；除了控制类设备外，告警类设备还可以检测环境状态，并主动将告警消息通知到家庭成员；

### 代码结构

本demo包括两个entry和netconfig两个模块，如下图是详细代码结构说明：

entry模块：

![代码结构](https://images.gitee.com/uploads/images/2021/1014/153519_fbc0e251_9290617.png "代码.png")

netconfig配网模块

![配网模块](https://images.gitee.com/uploads/images/2021/1014/161720_8b8b8d05_9290617.png "配网.png")

### 安装部署

##### 1.代码编译运行步骤

1）提前准备好搭载HarmonyOS系统的手机，确保HarmonyOS 手机中api version 在 API 6以上。

查看HarmonyOS  手机API方法：在HarmonyOS SDK存储目录下的**Sdk\toolchains**文件下打开命令行工具，执行如下命令进行查询真机设备的apiversion。

```
hdc shell 
getprop hw_sc.build.os.apiversion // 返回数值是6或者6以上
```

2）准备HarmonyOS应用的开发环境，请参考 [搭建开发环境流程]( https://developer.harmonyos.com/cn/docs/documentation/doc-guides/installation_process-0000001071425528 )；

3）准备项目签名文件，并配置签名信息，请参考 [申请发布证书和Profile文件/配置签名信息](https://developer.harmonyos.com/cn/docs/documentation/doc-guides/publish_app-0000001053223745#section178461193713)

4）DevEco Studio打开本工程，并将下列文件bundleName由com.example.distschedule修改为自己申请的包名；

```
./netconfig/src/main/js/default/pages/index/index.js
./entry/src/config.json
./netconfig/src/main/config.json
```

5）真机运行应用请参考 [本地真机运行]( https://developer.harmonyos.com/cn/docs/documentation/doc-guides/run_phone_tablat-0000001064774652 )；

![安装成功](https://images.gitee.com/uploads/images/2021/1115/162915_f8a32bcd_2322605.gif "install.gif")



##### 2.配网步骤

1）给应用配置配置快服务意图，[ 设备碰一碰配置指南](../../dev/docs/NFC_label_definition)

2）提前准备好智能设备端，且让设备端进入配网模式，可参考[设备端无感配网介绍](../../dev/docs/net_config#3%E6%93%8D%E4%BD%9C%E8%AF%B4%E6%98%8E)；

3）打开手机NFC功能，打开应用并登陆账号，手机碰一碰智能设备，等待拉起配网页面后开始给设备端配网；

4）在设备端配网成功后，进入设备控制页可控制设备开关机等；

![配网](https://gitee.com/openharmony-sig/knowledge_demo_smart_home/raw/master/dev/docs/quick_start/resource/gif_001.gif "net_config.gif")

### 新智能设备接入

参考 [数字管家设备接入指南](./docs/数字管家设备接入指南.md)，根据文档编写页面、处理相关逻辑和对接服务端接口，即可将已开发完成的智能设备快速接入到数字管家应用端。


### 约束限制

1. 提前准好已实名认证的开发者联盟账号 ，具体 [实名认证](https://developer.huawei.com/consumer/cn/doc/start/itrna-0000001076878172)

2. 开发工具：DevEco Studio 3.0 Beta1  及以上，SDK 请配置 API Version 6 2.2.0.3 版本以上，具体 [DevEcoStudio下载](https://developer.harmonyos.com/cn/develop/deveco-studio#download/)

3. 打开手机**设置 > 应用和服务 > 应用管理** (点击右上角图标，点击显示系统程序选项)，搜索“智慧生活基础服务”，检测智慧生活基础服务的版本是否为12.0.2.306及以上。如果不是，请打开华为手机的“智慧生活”APP，点击“**我的 > 设置 > 检查应用更新**”，更新“智慧生活基础服务”。

4. 更多资料请登录HarmonyOS应用开发官网： https://developer.harmonyos.com/cn/

