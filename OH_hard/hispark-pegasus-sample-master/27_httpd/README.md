HTTP Server for HarmonyOS with liteos-m kernel
===============================================

**目的：** 用于在鸿蒙开发板上支持HTTP服务器的功能，目前仅支持`Hi3861`开发板。

## 说明

**相关说明：**

1. 本项目是从LwIP中抽取的HTTP服务器代码；
2. `Hi3861 SDK`中已经包含了一份预编译的lwip，但没有开启HTTP服务器功能（静态库无法修改）；

## 编译

**如何编译：**

1. 将httpd代码下载到openharmony源码顶层目录：
    * `mkdir third_party/httpd && git clone https://gitee.com/hihopeorg/httpd.git third_party/httpd`
2. 修改openharmony的`build/lite/product/wifiiot.json`文件：
    * 将其中的`//applications/sample/wifi-iot/app`替换为`//third_party/httpd:app`；
3. 在openharmony源码的顶层目录，执行`python build.py wifiiot`；

### HTTP 服务器特性

目前已测试通过的功能有：

1. 通过HTTP访问静态资源文件
2. 通过cgi回调函数处理表单和页面跳转

**静态资源文件**：

* 本服务器实现不支持实际的服务端本地文件系统访问；
* 取而代之的是，一种简单的虚拟文件系统：
    * `src/http/fs.c` 文件提供了一套文件系统接口，使用数组直接存储文件数据
    * `src/http/fsdata.c`文件是由`makefsdata`命令生成的虚拟文件系统元数据文件
    * `makefsdata` 命令行工具，用于将一个目录下的静态资源文件转换为`fsdata.c`文件
