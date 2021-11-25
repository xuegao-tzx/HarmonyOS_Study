# HiSpark WiFi-IoT 鸿蒙套件样例开发--等待（Delay）

![hihope_illustration](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/hihope_illustration.png)

[HiSpark WiFi-IoT鸿蒙开发套件](https://item.taobao.com/item.htm?spm=a1z10.1-c-s.w5003-23341819265.1.bf644a82Da9PZK&id=622343426064&scene=taobao_shop) 首发于HDC 2020，是首批支持HarmonyOS 2.0的开发套件，亦是鸿蒙官方推荐套件，由润和软件HiHope量身打造，已在鸿蒙社区和广大鸿蒙开发者中得到广泛应用。

![wifi_iot](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/wifi_iot.png)

## 一、Delay API

| API名称      | 说明                 |
| ------------ | -------------------- |
| osDelay      | 等待指定的ticks      |
| osDelayUntil | 等待到指定的时钟周期 |

## 二、代码分析

使用`osDelay`让线程等待100个时钟周期，然后通过`osKernelGetTickCount`获取当前的时钟周期tick, 在此基础上增加100个时钟周期，之后调用`osDelayUntil`让线程等待tick + 100个时钟周期后恢复运行

```c
void rtosv2_delay_main(void *arg) {
    (void)arg;

    printf("[Delay Test] Current system tick: %d.\r\n", osKernelGetTickCount());
    osStatus_t status = osDelay(100);
    printf("[Delay Test] osDelay, status: %d.\r\n", status);
    printf("[Delay Test] Current system tick: %d.\r\n", osKernelGetTickCount());

    uint32_t tick = osKernelGetTickCount();
    tick += 100;
    status = osDelayUntil(tick);
    printf("[Delay Test] osDelayUntil, status: %d.\r\n", status);
    printf("[Delay Test] Current system tick: %d.\r\n", osKernelGetTickCount());
}
```



## 三、如何编译

1. 将此目录下的 `delay.c` 和 `BUILD.gn` 复制到openharmony源码的`applications\sample\wifi-iot\app\iothardware`目录下，
2. 修改openharmony源码的`applications\sample\wifi-iot\app\BUILD.gn`文件，将其中的 `features` 改为：

```
    features = [
        "iothardware:delay_demo",
    ]
```

   3.在openharmony源码顶层目录执行：`python build.py wifiiot`

## 四、运行结果

```
[Delay Test]  Current system tick: 248.
[Delay Test]  osDelay, status: 0.
[Delay Test]  Current system tick: 348.
[Delay Test]  osDelayUntil, status: 0.
[Delay Test]  Current system tick: 448.
```

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

