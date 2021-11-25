# HarmonyOS 兼容版 paho.mqtt.embedded-c

### 特性

1. 支持 HarmonyOS liteos-m 内核，包括 Hi3861 开发板；
2. 支持 HarmonyOS liteos-a 内核，包括 Hi3516, Hi3518 开发板；
3. 测试程序支持 Linux 平台；

### 编译

**编译到开发板：**

1. 克隆本项目到 HarmonyOS 源码中，例如 `third_party/paho.mqtt.embedded-c`；
2. 在产品 json 文件中添加组件依赖，例如在`applications`下添加一行：`{ "name": "mqtt", "dir": "//third_party/paho.mqtt.embedded-c:app", "features":[] }`；
3. 使用 HarmonyOS 的 build.py 脚本进行编译；


**编译到Linux主机：**

1. 准备cmake，可以使用apt安装或从cmake官网下载；
2. 创建并跳转到build目录，`mkdir build && cd build`
3. 执行`cmake ..`命令，生成Makefile；
4. 执行`make`命令，生成测试程序；

### 测试

**Hi3861开发板测试指南**

我们为liteos-m内核的鸿蒙设备添加了几条AT命令，用于测试MQTT客户端功能，
具体参考`MQTTClient-C/samples/ohos/mqtt_test_cmsis.c`文件。

AT测试命令的功能和参数说明如下：
1. `AT+MQTT_CONN=host,port[clientId,username,password]`命令，用于连接到MQTT服务器，参数说明如下所述：
    * `host`和`port`参数为必填参数，用于指定MQTT服务器的主机和端口
    * `clientId`,`username`和`password`为可选参数，用于指定MQTT CONNECT消息的附加参数；
2. `AT+MQTT_DISC`命令，无参数，用于断开和MQTT服务器之间的连接；
3. `AT+MQTT_TEST`命令，无参数，用于测试订阅和发送，首先向服务端订阅即将发布的消息，再向服务端发布10条消息；
4. `AT+MQTT_SUB=topic`命令，用于向MQTT服务器订阅一个主题；
    * `topic`为具体的主题字符串；
5. `AT+MQTT_PUB=topic,payload`命令，用于向MQTT服务器发布一个主题；
    * `topic`为具体的主题字符串；
    * `payload`为消息的文本内容；

**注：**
1. `=`是AT命令与参数列表的分隔符
2. `,`是参数列表中各个参数的分隔符


**Hi3516开发板和Linux主机测试指南**

使用方式：
```sh
$ ./mqtt_test
Usage ./mqtt_test host [port] [clientId] [username] [password]
```

参数说明：

* `host`和`port`参数为必填参数，用于指定MQTT服务器的主机和端口
* `clientId`,`username`和`password`为可选参数，用于指定MQTT CONNECT消息的附加参数；


--------

**以下为原始README.md内容**

# Eclipse Paho MQTT C/C++ client for Embedded platforms

This repository contains the source code for the [Eclipse Paho](http://eclipse.org/paho) MQTT C/C++ client library for Embedded platorms.

It is dual licensed under the EPL and EDL (see about.html and notice.html for more details).  You can choose which of these licenses you want to use the code under.  The EDL allows you to embed the code into your application, and distribute your application in binary or source form without contributing any of your code, or any changes you make back to Paho.  See the EDL for the exact conditions.

There are three sub-projects:

1. MQTTPacket - simple de/serialization of MQTT packets, plus helper functions
2. MQTTClient - high(er) level C++ client, plus
3. MQTTClient-C - high(er) level C client (pretty much a clone of the C++ client)

The *MQTTPacket* directory contains the lowest level C library with the smallest requirements.  This supplies simple serialization
and deserialization routines.  They serve as a base for the higher level libraries, but can also be used on their own
It is mainly up to you to write and read to and from the network.

The *MQTTClient* directory contains the next level C++ library.  This networking code is contained in separate classes so that you can plugin the
network of your choice.  Currently there are implementations for Linux, Arduino and mbed.  ARM mbed was the first platform for which this was written,
where the conventional language choice is C++, which explains the language choice.  I have written a starter [Porting Guide](http://modelbasedtesting.co.uk/2014/08/25/porting-a-paho-embedded-c-client/).

The *MQTTClient-C* directory contains a C equivalent of MQTTClient, for those platforms where C++ is not supported or the convention.  As far
as possible it is a direct translation from *MQTTClient*.

## Build requirements / compilation

CMake builds for the various packages have been introduced, along with Travis-CI configuration for automated build & testing.  The basic
method of building on Linux is:

```
mkdir build.paho
cd build.paho
cmake ..
make
```

The travis-build.sh file has the full build and test sequence for Linux.


## Usage and API

See the samples directories for examples of intended use.  Doxygen config files for each package are available in the doc directory.

## Runtime tracing

The *MQTTClient* API has debug tracing for MQTT packets sent and received - turn this on by setting the MQTT_DEBUG preprocessor definition.


## Reporting bugs

This project uses GitHub Issues here: [github.com/eclipse/paho.mqtt.embedded-c/issues](https://github.com/eclipse/paho.mqtt.embedded-c/issues) to track ongoing development and issues.

## More information

Discussion of the Paho clients takes place on the [Eclipse Mattermost Paho channel](https://mattermost.eclipse.org/eclipse/channels/paho) and the [Eclipse paho-dev mailing list](https://dev.eclipse.org/mailman/listinfo/paho-dev).

General questions about the MQTT protocol are discussed in the [MQTT Google Group](https://groups.google.com/forum/?hl=en-US&fromgroups#!forum/mqtt).

More information is available via the [MQTT community](http://mqtt.org).
