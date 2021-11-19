# TodoList应用代码介绍

### 简介

TodoList应用是基于OpenHarmony SDK开发的安装在L1(3516)设备上的应用；应用主要功能是以列表的形式，展示需要完成的日程；通过本demo可以学习到JS UI 框架List使用；

### 代码结构

本demo包括entry模块，如下图是详细代码结构说明：

entry模块：

![输入图片说明](https://images.gitee.com/uploads/images/2021/1022/164618_9841180a_9290617.png "todoList代码结构.png")

### 安装部署

##### 1.代码编译运行步骤

1）下载此项目，[链接](https://gitee.com/openharmony-sig/knowledge_demo_smart_home/tree/master/FA/Contacts )。

2）开发环境搭建，开发工具：DevEco Studio 3.0 Beta1，SDK 请配置请参考[配置OpenHarmonySDK](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/configuring-openharmony-sdk.md) 。

3）导入OpenHarmony工程：OpenHarmony应用开发，只能通过导入Sample工程的方式来创建一个新工程，具体可参考[导入Sample工程创建一个新工程](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/import-sample-to-create-project.md)

4）OpenHarmony应用运行在真机设备上，需要对应用进行签名，请参考[OpenHarmony应用签名](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/configuring-openharmony-app-signature.md)

5）需要使用hdc工具将OpenHarmony的hap包推送到真机设备上进行安装，具体可参考[安装运行OpenHarmony应用](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/installing-openharmony-app.md)。

### 约束限制

1. 提前准好已实名认证的开发者联盟账号 ，具体[参考](https://developer.huawei.com/consumer/cn/)
2. 本示例需在真机运行，请提前准备好搭载OpenHarmony的L1设备。
3. 更多应用编译和签名资料，请[参考](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/deveco-studio-overview.md#/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/configuring-openharmony-sdk.md)
4. 更多开发资料请登录应用开发官网： https://developer.harmonyos.com/cn/