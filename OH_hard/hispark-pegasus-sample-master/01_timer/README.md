# HiSpark WiFi-IoT 鸿蒙套件样例开发--软定时器（Timer）

![hihope_illustration](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/hihope_illustration.png)

[HiSpark WiFi-IoT鸿蒙开发套件](https://item.taobao.com/item.htm?spm=a1z10.1-c-s.w5003-23341819265.1.bf644a82Da9PZK&id=622343426064&scene=taobao_shop) 首发于HDC 2020，是首批支持HarmonyOS 2.0的开发套件，亦是鸿蒙官方推荐套件，由润和软件HiHope量身打造，已在鸿蒙社区和广大鸿蒙开发者中得到广泛应用。

![wifi_iot](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/wifi_iot.png)

## 一、Timer API

| API名称          | 说明                     |
| ---------------- | ------------------------ |
| osTimerNew       | 创建和初始化定时器       |
| osTimerGetName   | 获取指定的定时器名字     |
| osTimerStart     | 启动或者重启指定的定时器 |
| osTimerStop      | 停止指定的定时器         |
| osTimerIsRunning | 检查一个定时器是否在运行 |
| osTimerDelete    | 删除定时器               |

### osTimerNew()

```c
osTimerId_t osTimerNew (osTimerFunc_t func, osTimerType_t type, void *argument, const osTimerAttr_t *attr)
```

**参数：**

| 名字     | 描述                                                         |
| :------- | :----------------------------------------------------------- |
| func     | 定时器回调函数.                                              |
| type     | 定时器类型，osTimerOnce表示单次定时器，ostimer周期表示周期性定时器. |
| argument | 定时器回调函数的参数                                         |
| attr     | 定时器属性                                                   |

## 二、代码分析

定时器的回调函数

```c
void cb_timeout_periodic(void *arg) {
    (void)arg;
    times++;
}
```

使用osTimerNew创建一个100个时钟周期调用一次回调函数cb_timeout_periodic定时器，每隔100个时钟周期检查一下全局变量times是否小于3，若不小于3则停止时钟周期

```c
void timer_periodic(void) {
    osTimerId_t periodic_tid = osTimerNew(cb_timeout_periodic, osTimerPeriodic, NULL, NULL);
    if (periodic_tid == NULL) {
        printf("[Timer Test] osTimerNew(periodic timer) failed.\r\n");
        return;
    } else {
        printf("[Timer Test] osTimerNew(periodic timer) success, tid: %p.\r\n",periodic_tid);
    }
    osStatus_t status = osTimerStart(periodic_tid, 100);
    if (status != osOK) {
        printf("[Timer Test] osTimerStart(periodic timer) failed.\r\n");
        return;
    } else {
        printf("[Timer Test] osTimerStart(periodic timer) success, wait a while and stop.\r\n");
    }

    while(times < 3) {
        printf("[Timer Test] times:%d.\r\n",times);
        osDelay(100);
    }

    status = osTimerStop(periodic_tid);
    printf("[Timer Test] stop periodic timer, status :%d.\r\n", status);
    status = osTimerDelete(periodic_tid);
    printf("[Timer Test] kill periodic timer, status :%d.\r\n", status);
}

```



## 三、如何编译

1. 将此目录下的 `timer.c` 和 `BUILD.gn` 复制到openharmony源码的`applications\sample\wifi-iot\app\iothardware`目录下，
2. 修改openharmony源码的`applications\sample\wifi-iot\app\BUILD.gn`文件，将其中的 `features` 改为：

```
    features = [
        "iothardware:timer_demo",
    ]
```

   3.在openharmony源码顶层目录执行：`python build.py wifiiot`

## 四、运行结果

```
[Timer Test] osTimerNew(periodic timer) success, tid: 0xe9b4c.
[Timer Test] osTimerStart(periodic timer) success, wait a while and stop.
[Timer Test] times:0.
[Timer Test] times:1.
[Timer Test] times:2.
[Timer Test] stop periodic timer, status :0.
[Timer Test] kill periodic timer, status :0.
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

