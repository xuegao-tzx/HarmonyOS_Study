# HiSpark WiFi-IoT 鸿蒙套件样例开发--envrionment_demo

![hihope_illustration](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/hihope_illustration.png)

[HiSpark WiFi-IoT鸿蒙开发套件](https://item.taobao.com/item.htm?spm=a1z10.1-c-s.w5003-23341819265.1.bf644a82Da9PZK&id=622343426064&scene=taobao_shop) 首发于HDC 2020，是首批支持HarmonyOS 2.0的开发套件，亦是鸿蒙官方推荐套件，由润和软件HiHope量身打造，已在鸿蒙社区和广大鸿蒙开发者中得到广泛应用。

![wifi_iot](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/wifi_iot.png)



## 一、环境监测板上外设与主控芯片（Pegasus）引脚的对应关系

* 蜂鸣器——PWM控制声音的评率和音量
  - GPIO09: PWM0
* MQ2 燃气传感器——ADC读取模拟值
  - GPIO11: ADC5
* AHT20 温湿度传感器——I2C接口通信，地址 0x38
  - GPIO13: I2C0_SDA
  - GPIO14: I2C0_SCL

## 二、如何编译

1. 将此目录下的文件 复制到openharmony源码的`applications\sample\wifi-iot\app\iothardware`目录下，
2. 修改openharmony源码的`applications\sample\wifi-iot\app\BUILD.gn`文件，将其中的 `features` 改为：
```python
    features = [
        "iothardware:envrionment_demo",
    ]
```
3. 在openharmony源码顶层目录执行：`python build.py wifiiot`

### 报错解决

1. 编译过程中报错：undefined reference to `hi_pwm_init` 等几个 `hi_pwm_`开头的函数，
	* **原因：** 因为默认情况下，hi3861_sdk中，PWM的CONFIG选项没有打开
	* **解决：** 修改`vendor\hisi\hi3861\hi3861\build\config\usr_config.mk`文件中的`CONFIG_PWM_SUPPORT`行：
	  * `# CONFIG_PWM_SUPPORT is not set`修改为`CONFIG_PWM_SUPPORT=y`
2. 编译过程中报错：undefined reference to `hi_i2c_init` 等几个 `hi_i2c_`开头的函数，
   - **原因：** 因为默认情况下，hi3861_sdk中，I2C的CONFIG选项没有打开
   - **解决：** 修改`vendor\hisi\hi3861\hi3861\build\config\usr_config.mk`文件中的`CONFIG_I2C_SUPPORT`行：
     - `# CONFIG_I2C_SUPPORT is not set`修改为`CONFIG_I2C_SUPPORT=y`

## 三、运行结果

烧录文件后，按下reset按键，程序开始运行：

* 蜂鸣器：倒计时响三声

* oled屏：显示temp、humi、gas数值

  


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