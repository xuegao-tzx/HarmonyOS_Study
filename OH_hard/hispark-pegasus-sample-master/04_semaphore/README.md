# HiSpark WiFi-IoT 鸿蒙套件样例开发--信号量（Semaphore）

![hihope_illustration](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/hihope_illustration.png)

[HiSpark WiFi-IoT鸿蒙开发套件](https://item.taobao.com/item.htm?spm=a1z10.1-c-s.w5003-23341819265.1.bf644a82Da9PZK&id=622343426064&scene=taobao_shop) 首发于HDC 2020，是首批支持HarmonyOS 2.0的开发套件，亦是鸿蒙官方推荐套件，由润和软件HiHope量身打造，已在鸿蒙社区和广大鸿蒙开发者中得到广泛应用。

![wifi_iot](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/wifi_iot.png)

## 一、Semaphore API

| API名称             | 说明                                                         |
| ------------------- | ------------------------------------------------------------ |
| osSemaphoreNew      | 创建并初始化一个信号量                                       |
| osSemaphoreGetName  | 获取一个信号量的名字                                         |
| osSemaphoreAcquire  | 获取一个信号量的令牌，若获取不到，则会超时返回               |
| osSemaphoreRelease  | 释放一个信号量的令牌，但是令牌的数量不超过初始定义的的令牌数 |
| osSemaphoreGetCount | 获取当前的信号量令牌数                                       |
| osSemaphoreDelete   | 删除一个信号量                                               |

## 二、代码分析

`osSemaphoreAcquire`获取共享资源的访问权限，若获取失败，则等待；访问成功后，可以通过`osSemaphoreRelease`释放对共享资源的访问

本样例为经典的消费者与生产者问题，需要确保仓库满时，生产者需要进入等待状态，产品消费完时，消费者需要进入等待状态

```c
void producer_thread(void *arg) {
    (void)arg;
    empty_id = osSemaphoreNew(BUFFER_SIZE, BUFFER_SIZE, NULL);
    filled_id = osSemaphoreNew(BUFFER_SIZE, 0U, NULL);
    while(1) {
        osSemaphoreAcquire(empty_id, osWaitForever);
        product_number++;
        printf("[Semp Test]%s produces a product, now product number: %d.\r\n", osThreadGetName(osThreadGetId()), product_number);
        osDelay(4);
        osSemaphoreRelease(filled_id);
    }
}

void consumer_thread(void *arg) {
    (void)arg;
    while(1){
        osSemaphoreAcquire(filled_id, osWaitForever);
        product_number--;
        printf("[Semp Test]%s consumes a product, now product number: %d.\r\n", osThreadGetName(osThreadGetId()), product_number);
        osDelay(3);
        osSemaphoreRelease(empty_id);
    }
}
```

由于消费产品的速度大于生产速度，所以定义了三个生产者，两个消费者

```c
void rtosv2_semp_main(void *arg) {
    (void)arg;
    empty_id = osSemaphoreNew(BUFFER_SIZE, BUFFER_SIZE, NULL);
    filled_id = osSemaphoreNew(BUFFER_SIZE, 0U, NULL);
 
    osThreadId_t ptid1 = newThread("producer1", producer_thread, NULL);
    osThreadId_t ptid2 = newThread("producer2", producer_thread, NULL);
    osThreadId_t ptid3 = newThread("producer3", producer_thread, NULL);
    osThreadId_t ctid1 = newThread("consumer1", consumer_thread, NULL);
    osThreadId_t ctid2 = newThread("consumer2", consumer_thread, NULL);

    osDelay(50);

    osThreadTerminate(ptid1);
    osThreadTerminate(ptid2);
    osThreadTerminate(ptid3);
    osThreadTerminate(ctid1);
    osThreadTerminate(ctid2);

    osSemaphoreDelete(empty_id);
    osSemaphoreDelete(filled_id);
}
```



## 三、如何编译

1. 将此目录下的 `semp.c` 和 `BUILD.gn` 复制到openharmony源码的`applications\sample\wifi-iot\app\iothardware`目录下，
2. 修改openharmony源码的`applications\sample\wifi-iot\app\BUILD.gn`文件，将其中的 `features` 改为：

```
    features = [
        "iothardware:semp_demo",
    ]
```

   3.在openharmony源码顶层目录执行：`python build.py wifiiot`

## 四、运行结果

截取部分运行结果

```
[Semp Test] osThreadNew(consumer1) success, thread id: 0xe8910.
[Semp Test] osThreadNew(consumer2) success, thread id: 0xe871c.
[Semp Test] osThreadNew(producer1) success, thread id: 0xe84c4.
[Semp Test] [Semp Test] producer1 produces a product, now product number: 1.
[Semp Test] producer2 produces a product, now product number: 2.
osThreadNew(producer2) success, thread id: 0xe8974.
[Semp Test] osThreadNew(producer3) success, thread id: 0xe89d8.
[Semp Test] producer3 produces a product, now product number: 3.
[Semp Test] producer1 produces a product, now product number: 4.
[Semp Test] consumer1 consumes a product, now product number: 3.
[Semp Test] producer2 produces a product, now product number: 4.
[Semp Test] consumer2 consumes a product, now product number: 3.
[Semp Test] consumer1 consumes a product, now product number: 2.
[Semp Test] producer3 produces a product, now product number: 3.
[Semp Test] consumer2 consumes a product, now product number: 2.
[Semp Test] producer1 produces a product, now product number: 3.
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

