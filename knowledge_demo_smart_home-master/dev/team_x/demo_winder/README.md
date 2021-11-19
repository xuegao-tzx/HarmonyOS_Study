

# 智能风扇设备端代码介绍

## 简介：
   	本Demo是基于HiSpark Wi-Fi IoT 开发套件开发，通过模拟智能风扇设备跟手机FA通信，基于NAN通道实现了无感配网、近场秒控、设备消息推送到FA等三大功能点。
   	特别说明：设备端代码基于开源版本code_1.1.0

## 代码编译
 1） 拉取相关代码后，分别拷贝hals 目录和winder 目录到OpenHarmony的源码中。目录为applications/sample/wifi-iot/app

可见winder代码目录和 hals目录结构如下：

winder    

```
├── BUILD.gn
├── include
│   ├── code_tab.h // 字母和汉字编码
│   ├── common.h 	 // 设备初始化相关接口
│   ├── defines.h  // 通用宏及调试宏定义
│   ├── keypad.h 	 // 按键相关接口
│   ├── led.h	 // LED等控制相关接口
│   ├── netcfg.h   // 无感配网注册相关接口
│   ├── network_config_service.h 无感配网相关头文件。
│   ├── oled.h 	// OLED 相关接口
│   ├── pwm.h 	// PWM 相关接口
│   ├── sensor.h  // 传感器相关接口
│   └── winder.h  // 风扇相关宏定义
├── libs
│   ├── libhilinkadapter_3861.a // 无感配网相关库文件。
│   └── libnetcfgdevicesdk.a // 无感配网相关库文件。
└── src
    ├── common.c // 设备相关初始化操作
    ├── keypad.c // 按键驱动模块
    ├── led.c    // LED灯相关控制实现
    ├── netcfg.c // NAN相关操作和联网动作
    ├── oled.c   // 与OLED 模块相关
    ├── pwm.c    // PWM相关控制实现
    ├── sensor.c // 传感器相关
    └── winder.c // 风扇主函数入口，及相关控制逻辑的实现
```

hals

```
├── BUILD.gn
├── include
│   └── peripheral_hal.h // 适配层接口
└── src
    └── peripheral_hal.c // 适配层接口实现
```

2）修改hi3861_adapter/hals/communication/wifi_lite/wifiservice/source/wifi_device.c，注释掉下面的代码，解决程序死锁问题。（正在提交PR修改gitee仓上的源码）

    if (LockWifiEventLock() != WIFI_SUCCESS) {
        return;
    }


    if (UnlockWifiEventLock() != WIFI_SUCCESS) {
        return;
    }


3） 修改app/BUILD.gn 文件

    	配置./applications/sample/wifi-iot/app/BUILD.gn文件，在features字段中增加索引，使目标模块参与编译。features字段指定业务模块的路径和目标，features字段配置如下。
```  
    import("//build/lite/config/component/lite_component.gni")
    
    lite_component("app") {
    features = [
        "winder",
    ]
    deps = [ "//applications/sample/wifi-iot/app/hals:hi3861_hals" ]
}

```

4） 打开三方驱动中的编译宏
    打开：device/hisilicon/hispark_pegasus/sdk_liteos/build/config/usr_config.mk
	找到：CONFIG_I2C_SUPPORT is not set，修改为CONFIG_I2C_SUPPORT=y

5） 编译后烧录bin 文件
	编译命令：1、hb set // 设置工程目录和选择相应目标 wifiiot_hispark_pegasus@hisilicon

​					   2、hb build -f 
​	烧录步骤：请参考 “Hi3861开发板第一个示例程序.md” 镜像烧录章节。使用DevEco Device Tool工具进行Hi3861源码的开发、编译和烧录可以参考[源码开发、编译、烧录文档](https://device.harmonyos.com/cn/docs/documentation/guide/onehop-dev-hi3861-0000001160429954)。

​			