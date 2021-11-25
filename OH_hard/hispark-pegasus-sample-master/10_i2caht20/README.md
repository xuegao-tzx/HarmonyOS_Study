# HiSpark WiFi-IoT 鸿蒙套件样例开发--I2C（i2c_aht20）

![hihope_illustration](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/hihope_illustration.png)

[HiSpark WiFi-IoT鸿蒙开发套件](https://item.taobao.com/item.htm?spm=a1z10.1-c-s.w5003-23341819265.1.bf644a82Da9PZK&id=622343426064&scene=taobao_shop) 首发于HDC 2020，是首批支持HarmonyOS 2.0的开发套件，亦是鸿蒙官方推荐套件，由润和软件HiHope量身打造，已在鸿蒙社区和广大鸿蒙开发者中得到广泛应用。

![wifi_iot](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/wifi_iot.png)

## 一、I2C API

| API名称                                                      | 说明                            |
| ------------------------------------------------------------ | ------------------------------- |
| I2cInit (WifiIotI2cIdx id, unsigned int baudrate)            | 用指定的波特速率初始化I2C设备   |
| I2cDeinit (WifiIotI2cIdx id)                                 | 取消初始化I2C设备               |
| I2cWrite (WifiIotI2cIdx id, unsigned short deviceAddr, const WifiIotI2cData *i2cData) | 将数据写入I2C设备               |
| I2cRead (WifiIotI2cIdx id, unsigned short deviceAddr, const WifiIotI2cData *i2cData) | 从I2C设备中读取数据             |
| I2cWriteread (WifiIotI2cIdx id, unsigned short deviceAddr, const WifiIotI2cData *i2cData) | 向I2C设备发送数据并接收数据响应 |
| I2cRegisterResetBusFunc (WifiIotI2cIdx id, WifiIotI2cFunc pfn) | 注册I2C设备回调                 |
| I2cSetBaudrate (WifiIotI2cIdx id, unsigned int baudrate)     | 设置I2C设备的波特率             |

## 二、Aht20 API

本样例提供了一个鸿蒙AHT20数字温湿度传感器驱动库，其功能如下：

* 使用Harmony OS的IoT硬件接口;
* 接口简洁易于使用、易于移植;
* 内置了测试程序，可直接进行测试;

| API名称                                                   | 说明                    |
| --------------------------------------------------------- | ----------------------- |
| uint32_t AHT20_Calibrate(void)                            | 校准，成功返回0         |
| uint32_t AHT20_StartMeasure(void)                         | 触发测量，成功返回0     |
| uint32_t AHT20_GetMeasureResult(float* temp, float* humi) | 读取测量结果，成功返回0 |

## 三、如何编译

1. 将此目录复制到openharmony源码的根目录下

2. 修改openharmony源码的`build/lite/product/wifiiot.json`文件：

   将`//applications/sample/wifi-iot/app`替换为`//10_i2caht20:app`保存；

3. 在openharmony源码目录下执行：`python build.py wifiiot`

## 四、编译错误解决

本项目代码使用了鸿蒙IoT硬件子系统的I2C API接口，需要连接到hi3861的I2C相关接口；默认情况下，Hi3861的I2C编译配置没有打开，编译时会有如下错误：

```txt
riscv32-unknown-elf-ld: ohos/libs/libhal_iothardware.a(hal_wifiiot_i2c.o): in function `.L0 ':
hal_wifiiot_i2c.c:(.text.HalI2cWrite+0x12): undefined reference to `hi_i2c_write'
riscv32-unknown-elf-ld: hal_wifiiot_i2c.c:(.text.HalI2cInit+0x12): undefined reference to `hi_i2c_init'
scons: *** [output/bin/Hi3861_wifiiot_app.out] Error 1
BUILD FAILED!!!!
```

**解决方法**

需要修改vendor\hisi\hi3861\hi3861\build\config\usr_config.mk文件：
`# CONFIG_I2C_SUPPORT is not set`行，修改为：`CONFIG_I2C_SUPPORT=y`

## 五、运行结果

烧录文件后，按下reset按键，程序开始运行，串口工具会输出测量后的温湿度数据



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



