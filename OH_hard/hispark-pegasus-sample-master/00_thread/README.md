# HiSpark WiFi-IoT 鸿蒙开发套件样例开发--线程（Thread）

![hihope_illustration](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/hihope_illustration.png)

[HiSpark WiFi-IoT鸿蒙开发套件](https://item.taobao.com/item.htm?spm=a1z10.1-c-s.w5003-23341819265.1.bf644a82Da9PZK&id=622343426064&scene=taobao_shop) 首发于HDC 2020，是首批支持HarmonyOS 2.0的开发套件，亦是鸿蒙官方推荐套件，由润和软件HiHope量身打造，已在鸿蒙社区和广大鸿蒙开发者中得到广泛应用。

![wifi_iot](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/wifi_iot.png)

## 一、Thread API

| API名称               | 说明                                                   |
| --------------------- | ------------------------------------------------------ |
| osThreadNew           | 创建一个线程并将其加入活跃线程组中                     |
| osThreadGetName       | 返回指定线程的名字                                     |
| osThreadGetId         | 返回当前运行线程的线程ID                               |
| osThreadGetState      | 返回当前线程的状态                                     |
| osThreadSetPriority   | 设置指定线程的优先级                                   |
| osThreadGetPriority   | 获取当前线程的优先级                                   |
| osThreadYield         | 将运行控制转交给下一个处于READY状态的线程              |
| osThreadSuspend       | 挂起指定线程的运行                                     |
| osThreadResume        | 恢复指定线程的运行                                     |
| osThreadDetach        | 分离指定的线程（当线程终止运行时，线程存储可以被回收） |
| osThreadJoin          | 等待指定线程终止运行                                   |
| osThreadExit          | 终止当前线程的运行                                     |
| osThreadTerminate     | 终止指定线程的运行                                     |
| osThreadGetStackSize  | 获取指定线程的栈空间大小                               |
| osThreadGetStackSpace | 获取指定线程的未使用的栈空间大小                       |
| osThreadGetCount      | 获取活跃线程数                                         |
| osThreadEnumerate     | 获取线程组中的活跃线程数                               |

### osThreadNew()

```c
osThreadId_t osThreadNew(osThreadFunc_t	func, void *argument,const osThreadAttr_t *attr )	
```

> **注意** :不能在中断服务调用该函数

**参数：**

| 名字     | 描述                             |
| :------- | :------------------------------- |
| func     | 线程函数.                        |
| argument | 作为启动参数传递给线程函数的指针 |
| attr     | 线程属性                         |

### osThreadTerminate()

```c
osStatus_t osThreadTerminate (osThreadId_t thread_id)
```

| 名字      | 描述                                                 |
| --------- | ---------------------------------------------------- |
| thread_id | 指定线程id，该id是由osThreadNew或者osThreadGetId获得 |

## 二、代码分析

创建线程，创建成功则打印线程名字和线程ID

```
osThreadId_t newThread(char *name, osThreadFunc_t func, void *arg) {
    osThreadAttr_t attr = {
        name, 0, NULL, 0, NULL, 1024*2, osPriorityNormal, 0, 0
    };
    osThreadId_t tid = osThreadNew(func, arg, &attr);
    if (tid == NULL) {
        printf("osThreadNew(%s) failed.\r\n", name);
    } else {
        printf("osThreadNew(%s) success, thread id: %d.\r\n", name, tid);
    }
    return tid;
}
```

该函数首先会打印自己的参数，然后对全局变量count进行循环+1操作，之后会打印count的值

```
void threadTest(void *arg) {
    static int count = 0;
    printf("%s\r\n",(char *)arg);
    osThreadId_t tid = osThreadGetId();
    printf("threadTest osThreadGetId, thread id:%p\r\n", tid);
    while (1) {
        count++;
        printf("threadTest, count: %d.\r\n", count);
        osDelay(20);
    }
}
```

主程序rtosv2_thread_main创建线程并运行，并使用上述API进行相关操作，最后终止所创建的线程。

```
void rtosv2_thread_main(void *arg) {
    (void)arg;
    osThreadId_t tid=newThread("test_thread", threadTest, "This is a test thread.");

    const char *t_name = osThreadGetName(tid);
    printf("[Thread Test]osThreadGetName, thread name: %s.\r\n", t_name);

    osThreadState_t state = osThreadGetState(tid);
    printf("[Thread Test]osThreadGetState, state :%d.\r\n", state);

    osStatus_t status = osThreadSetPriority(tid, osPriorityNormal4);
    printf("[Thread Test]osThreadSetPriority, status: %d.\r\n", status);

    osPriority_t pri = osThreadGetPriority (tid);   
    printf("[Thread Test]osThreadGetPriority, priority: %d.\r\n", pri);

    status = osThreadSuspend(tid);
    printf("[Thread Test]osThreadSuspend, status: %d.\r\n", status);  

    status = osThreadResume(tid);
    printf("[Thread Test]osThreadResume, status: %d.\r\n", status);

    uint32_t stacksize = osThreadGetStackSize(tid);
    printf("[Thread Test]osThreadGetStackSize, stacksize: %d.\r\n", stacksize);

    uint32_t stackspace = osThreadGetStackSpace(tid);
    printf("[Thread Test]osThreadGetStackSpace, stackspace: %d.\r\n", stackspace);

    uint32_t t_count = osThreadGetCount();
    printf("[Thread Test]osThreadGetCount, count: %d.\r\n", t_count);  

    osDelay(100);
    status = osThreadTerminate(tid);
    printf("[Thread Test]osThreadTerminate, status: %d.\r\n", status);
}
```

## 三、如何编译

1. 将此目录下的 `thread.c` 和 `BUILD.gn` 复制到openharmony源码的`applications\sample\wifi-iot\app\iothardware`目录下，
2. 修改openharmony源码的`applications\sample\wifi-iot\app\BUILD.gn`文件，将其中的 `features` 改为：

```
    features = [
        "iothardware:thread_demo",
    ]
```

   3.在openharmony源码顶层目录执行：`python build.py wifiiot`

## 四、运行结果

```
[Thread Test] osThreadNew(test_thread) success.
[Thread Test] osThreadGetName, thread name: test_thread.
[Thread Test] osThreadGetState, state :1.
[Thread Test] This is a test thread.  <-testThread log
[Thread Test] threadTest osThreadGetId, thread id:0xe8544
[Thread Test] threadTest, count: 1.   <-testThread log
[Thread Test] osThreadSetPriority, status: 0.
[Thread Test] osThreadGetPriority, priority: 28.
[Thread Test] osThreadSuspend, status: 0.
[Thread Test] osThreadResume, status: 0.
[Thread Test] osThreadGetStackSize, stacksize: 2048.
[Thread Test] osThreadGetStackSpace, stackspace: 1144.
[Thread Test] osThreadGetCount, count: 12.
[Thread Test] threadTest, count: 2.   <-testThread log
[Thread Test] threadTest, count: 3.   <-testThread log
[Thread Test] threadTest, count: 4.   <-testThread log
[Thread Test] threadTest, count: 5.   <-testThread log
[Thread Test] threadTest, count: 6.   <-testThread log
[Thread Test] osThreadTerminate, status: 0.
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

