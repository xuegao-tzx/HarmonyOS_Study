SNTP client for HarmonyOS
===============================================

**目的：** 在鸿蒙开发板上支持SNTP客户端功能，目前仅支持`Hi3861`开发板。

**相关说明：**

1. 本项目是从LwIP中抽取的SNTP代码；
3. `Hi3861 SDK`中已经包含了一份预编译的lwip，但没有开启SNTP功能（静态库无法修改）；

**如何编译：**

1. 将httpd代码下载到openharmony源码顶层目录：
    * `mkdir third_party/sntp && git clone https://gitee.com/hihopeorg/sntp.git third_party/sntp`
2. 修改openharmony的`build/lite/product/wifiiot.json`文件：
    * 将其中的`//applications/sample/wifi-iot/app`替换为`//third_party/sntp:app`；
3. 在openharmony源码的顶层目录，执行`python build.py wifiiot`；

