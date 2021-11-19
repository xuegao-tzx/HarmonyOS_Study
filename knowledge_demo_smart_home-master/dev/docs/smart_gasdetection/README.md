# 智能可燃气体检测系统

## 一、 介绍

本项目是基于BearPi套件开发的智能可燃气体检测Demo，该系统内主要由小熊派单板套件和和MQ5可燃气体检测传感器组成。
智能可燃气体检测系统可以通过云和手机建立连接，可以在手机上控制感应的阈值，传感器感知到的可燃气体浓度超过阈值之后，将会通过云传给手机，并报警。

#### 1. 交互流程

&nbsp;<img src="./resource/1.png" alt="产品示意图" style="zoom: 80%;" />

#### 2. 实物简介
&nbsp;<img src="./resource/4.jpg" alt="产品示意图" style="zoom: 80%;" />

我们使用小熊派bearpi_hm_nano开发底板+ MQ5传感器拓展板来模拟智能可燃气体检测设备。 

RESET按键控制设备重启；

RESET按键组合控制按键可以清除已保存的配网信息，具体操作：先按下控制按键F2，然后按下RESET按键；

通过NFC模块实现设备碰一碰快速配网；

#### 3. 实物操作体验

&nbsp;<img src="./resource/zhenggexiao.gif" alt="实物操作" style="zoom:67%;" />

## 二、快速上手

#### 1. 硬件准备  

| 硬件名称                    | 购买链接                                                     | 数量 |
| :-------------------------- | ------------------------------------------------------------ | ---- |
| 小熊派bearpi_hm_nano 开发板 | [购买链接](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22244473708.15.305b5f69P3rzYw&id=633296694816) | x1   |
| 可燃气体传感器              | [购买链接](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.46ba2e8dPxSo3L&id=573784216895&_u=61g8rvagedc5&mt=) | x1   |
| 预装HarmonyOS手机           |                                                              | x1   |

小熊派nano 通用底板与可燃气体传感器连接如实物图所示，具体细节如下：

小熊派nano 通用底板 V3.3 Pin 接可燃气体传感器 VCC Pin;

小熊派nano 通用底板 GND Pin 接可燃气体传感器 GND Pin;

小熊派nano 通用底板 GPIO_13 Pin 接可燃气体传感器 AO Pin;

 	

#### 2. Linux编译服务器基础环境准备

本次搭建的开发基础环境由windows 工作台和Linux  编译服务器组成。windows 工作台可以通过samba 服务或ssh 方式访问Linux编译服务器。其中windows 工作台用来烧录和代码编辑，Linux编译服务器用来编译OpenHarmony代码，为了简化步骤，Linux编译服务器推荐安装Ubuntu20.04。

###### 安装和配置Python

1) 打开Linux终端。

2) 如果使用ubuntu20.04，python版本无需切换。输入如下命令，查看python版本号，需要使用python3.7以上版本,否则参考 [系统基础环境搭建](https://gitee.com/openharmony/docs/blob/OpenHarmony_1.0.1_release/zh-cn/device-dev/quick-start/%E6%90%AD%E5%BB%BA%E7%B3%BB%E7%BB%9F%E5%9F%BA%E7%A1%80%E7%8E%AF%E5%A2%83.md)。

   ```
   python3 --version
   ```

3) 安装并升级Python包管理工具（pip3）。

```
sudo apt-get install python3-setuptools python3-pip -y
sudo pip3 install --upgrade pip
```



###### 安装hb

1. 运行如下命令安装hb

   ```
   python3 -m pip install --user ohos-build
   ```

2) 设置环境变量

   ```
   vim ~/.bashrc
   ```

   将以下命令拷贝到.bashrc文件的最后一行，保存并退出。

   ```
   export PATH=~/.local/bin:$PATH
   ```

   执行如下命令更新环境变量。

   ```
   source ~/.bashrc
   ```

3) 执行"hb -h"，有打印以下信息即表示安装成功.

   ```
   usage: hb
   
   OHOS build system
   
   positional arguments:
     {build,set,env,clean}
       build               Build source code
       set                 OHOS build settings
       env                 Show OHOS build env
       clean               Clean output
   
   optional arguments:
     -h, --help            show this help message and exit
   ```



#### 3. Hi3861开发环境准备

在Linux编译服务器上搭建好基础开发环境后，需要安装OpenHarmony 编译Hi3861 平台特有的开发环境。

###### 安装编译依赖基础软件

```
sudo apt-get install -y build-essential gcc g++ make zlib* libffi-dev
```

###### 安装Scons

1) 打开Linux 终端。

2) 运行如下命令，安装Scons安装包。

   ```
   python3 -m pip install scons
   ```

3) 运行如下命令，查看是否安装成功。如果安装成功，查询结果下图所示。

   ```
   scons -v
   ```

   下图 Scons安装成功界面，版本要求3.0.4以上

   &nbsp;![5](./resource/3.png)



###### 安装python模块

```
sudo pip3 install setuptools kconfiglib pycryptodome ecdsa six --upgrade --ignore-installed six
```

######  安装gcc_riscv32（WLAN模组类编译工具链）

1) 打开Linux终端。
2) 下载gcc_riscv32镜像，[下载链接](https://repo.huaweicloud.com/harmonyos/compiler/gcc_riscv32/7.3.0/linux/gcc_riscv32-linux-7.3.0.tar.gz)。

3) 设置环境变量

   将压缩包解压到根目录

   ```
   tar -xvf gcc_riscv32-linux-7.3.0.tar.gz -C ~
   ```

   设置环境变量。

   ```
   vim ~/.bashrc
   ```

   将以下命令拷贝到.bashrc文件的最后一行，保存并退出。

   ```
   export PATH=~/gcc_riscv32/bin:$PATH
   ```

   生效环境变量。	

   ```
   source ~/.bashrc
   ```

4) 在命令行中输入如下命令，如果能正确显示编译器版本号，表明编译器安装成功。

   ```
   riscv32-unknown-elf-gcc -v
   ```

#### 4. 源码下载&编译准备

##### 1.码云工具下载

```
curl https://gitee.com/oschina/repo/raw/fork_flow/repo-py3 > /usr/local/bin/repo
chmod a+x /usr/local/bin/repo
pip3 install -i https://pypi.tuna.tsinghua.edu.cn/simple requests
```

##### 2.代码下载

###### 1) OpenHarmony代码下载

```
#特别注意：请下载OpenHarmony 1.0.1 版本，后续会更新支持OpenHarmony其他版本
repo init -u git@gitee.com:openharmony/manifest.git -b OpenHarmony_1.0.1_release --no-repo-verify
repo sync -c
repo forall -c 'git lfs pull'
```

###### 2) 设备侧代码下载

具体仓库地址：https://gitee.com/openharmony-sig/knowledge_demo_smart_home/

下载方式1：直接下载仓库对应zip 包文件

下载方式2：fork knowledge_demo_smart_home仓库后，使用git 命令下载

```
git clone git@gitee.com:xxxxxxx/knowledge_demo_smart_home.git  // 其中xxxxxxx为fork后相关账号名字。
```

###### 3) 编译前准备

1. 代码拷贝

   ```
   假设knowledge_demo_smart_home和源码OpenHarmony1.01均存放在你的ubuntu用户根目录下，命令如下：
   cp -rfa  ~/knowledge_demo_smart_home/dev/device/bearpi  ~/OpenHarmony1.01/device/ 
   cp -rfa  ~/knowledge_demo_smart_home/dev/team_x  ~/OpenHarmony1.01/vendor/
   cp -rfa  ~/knowledge_demo_smart_home/dev/third_party/iot_link  ~/OpenHarmony1.01/third_party/
   ```
   

整合并修改完成后的目录结构如下图：

&nbsp;![代码目录结构](./resource/2.png)

#### 5. 编译&烧录

```
编译命令：
hb set  // 如果是第一次编译，Input code path 命令行中键入"./" 指定OpenHarmony工程编译根目录后 回车，
如下图所示，使用键盘上下键选中智能可燃气体检测设备
```

&nbsp;![5](./resource/5.jpg)

```
hb build // 如果需要全量编译，可以添加-f 选项
```

详细的代码编译及固件烧录步骤，请参考 [编译和烧录指南](../build_and_burn/README.md)

#### 6. 设备配网

1) 在设备上电前需准备好安装了数字管家应用的HarmonyOS手机，详情见[数字管家应用开发](../../../FA/DistSchedule/README_zh.md), 并在设置中开启手机的NFC功能；
2) 写设备NFC标签，详细操作见设备[NFC标签指导文档](../NFC_label_definition/README.md);
3) 烧录完成后，上电。开发者在观察开发板上状态LED灯以８Hz的频率闪烁时，将手机上半部靠近开发板NFC标签处；
4) 无需任何操作手机将自动拉起数字管家应用并进入配网状态，配网过程中无需输入热点账号密码。 具体无感配网相关流程参考 [无感配网指南](../net_config/README.md) 
&nbsp;<img src="./resource/wuganxiao.gif" alt="实物操作" style="zoom:67%;" />

#### 7. 操作体验

1）阈值设置

配网完成后，数字管家应用会自动进入可燃气体传感器的控制页面，通过控制页面设置可燃气体传感器告警阈值。

2）可燃气体超标告警

当检测到的可燃气体数值，超过预先设定的阈值时，会通知数字管家应用告警， 并且告警消息还会推送同一家庭账号的所有成员。

&nbsp;<img src="./resource/caozuoxiao.gif" alt="实物操作" style="zoom:67%;" />
