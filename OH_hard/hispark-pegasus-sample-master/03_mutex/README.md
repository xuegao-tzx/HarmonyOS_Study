# HiSpark WiFi-IoT 鸿蒙套件样例开发--互斥锁（Mutex）

![hihope_illustration](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/hihope_illustration.png)

[HiSpark WiFi-IoT鸿蒙开发套件](https://item.taobao.com/item.htm?spm=a1z10.1-c-s.w5003-23341819265.1.bf644a82Da9PZK&id=622343426064&scene=taobao_shop) 首发于HDC 2020，是首批支持HarmonyOS 2.0的开发套件，亦是鸿蒙官方推荐套件，由润和软件HiHope量身打造，已在鸿蒙社区和广大鸿蒙开发者中得到广泛应用。

![wifi_iot](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/wifi_iot.png)

## 一、Mutex API

| API名称         | 说明                                                     |
| --------------- | -------------------------------------------------------- |
| osMutexNew      | 创建并初始化一个互斥锁                                   |
| osMutexGetName  | 获得指定互斥锁的名字                                     |
| osMutexAcquire  | 获得指定的互斥锁的访问权限，若互斥锁已经被锁，则返回超时 |
| osMutexRelease  | 释放指定的互斥锁                                         |
| osMutexGetOwner | 获得指定互斥锁的所有者线程                               |
| osMutexDelete   | 删除指定的互斥锁                                         |

## 二、代码分析

全局变量`g_test_value`若同时被多个线程访问，会将其加1，然后判断其奇偶性，并输出日志，如果没有互斥锁保护，线程会被中断导致错误，所以需要创建互斥锁来保护多线程共享区域

```c
void number_thread(void *arg) {
    osMutexId_t *mid = (osMutexId_t *)arg;
    while(1) {
        if (osMutexAcquire(*mid, 100) == osOK) {
            g_test_value++;
            if (g_test_value % 2 == 0) {
                printf("[Mutex Test] %s gets an even value %d.\r\n", osThreadGetName(osThreadGetId()), g_test_value);
            } else {
                printf("[Mutex Test] %s gets an odd value %d.\r\n",  osThreadGetName(osThreadGetId()), g_test_value);
            }
            osMutexRelease(*mid);
            osDelay(5);
        }
    }
}
```

创建三个线程访问全局变量`g_test_value` ，同时创建一个互斥锁共所有线程使用

```c
void rtosv2_mutex_main(void *arg) {
    (void)arg;
    osMutexAttr_t attr = {0};

    osMutexId_t mid = osMutexNew(&attr);
    if (mid == NULL) {
        printf("[Mutex Test] osMutexNew, create mutex failed.\r\n");
    } else {
        printf("[Mutex Test] osMutexNew, create mutex success.\r\n");
    }

    osThreadId_t tid1 = newThread("Thread_1", number_thread, &mid);
    osThreadId_t tid2 = newThread("Thread_2", number_thread, &mid);
    osThreadId_t tid3 = newThread("Thread_3", number_thread, &mid);

    osDelay(13);
    osThreadId_t tid = osMutexGetOwner(mid);
    printf("[Mutex Test] osMutexGetOwner, thread id: %p, thread name: %s.\r\n", tid, osThreadGetName(tid));
    osDelay(17);

    osThreadTerminate(tid1);
    osThreadTerminate(tid2);
    osThreadTerminate(tid3);
    osMutexDelete(mid);
}
```



## 三、如何编译

1. 将此目录下的 `mutex.c` 和 `BUILD.gn` 复制到openharmony源码的`applications\sample\wifi-iot\app\iothardware`目录下，
2. 修改openharmony源码的`applications\sample\wifi-iot\app\BUILD.gn`文件，将其中的 `features` 改为：

```
    features = [
        "iothardware:mutex_demo",
    ]
```

   3.在openharmony源码顶层目录执行：`python build.py wifiiot`

## 四、运行结果

截取部分运行结果

```
[Mutex Test]  osMutexNew, create mutex success.
[Mutex Test]  osThreadNew(Thread_1) success, thread id: 0xe84c4.
RTOSV2.0_TES[Mutex Test]  Thread_1 gets an odd value 1.
[Mutex Test]  Thread_2 gets an even value 2.
T: osThreadNew(Thread_2) success, thread id: 0xe871c.
[Mutex Test]  osThreadNew(Thread_3) success, thread id: 0xe8910.
[Mutex Test]  Thread_3 gets an odd value 3.
[Mutex Test]  Thread_1 gets an even value 4.
[Mutex Test]  Thread_2 gets an odd value 5.
[Mutex Test]  Thread_3 gets an even value 6.
[Mutex Test]  Thread_1 gets an odd value 7.
[Mutex Test]  Thread_2 gets an even value 8.
[Mutex Test]  Thread_3 gets an odd value 9.
[Mutex Test]  osMutexGetOwner, thread id: 0xe8910, thread name: Thread_3.
[Mutex Test]  Thread_1 gets an even value 10.
[Mutex Test]  Thread_2 gets an odd value 11.
[Mutex Test]  Thread_3 gets an even value 12.
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

