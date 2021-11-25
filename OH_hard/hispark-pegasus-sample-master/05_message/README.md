# HiSpark WiFi-IoT 鸿蒙套件样例开发--消息队列（MessageQueue）

![hihope_illustration](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/hihope_illustration.png)

[HiSpark WiFi-IoT鸿蒙开发套件](https://item.taobao.com/item.htm?spm=a1z10.1-c-s.w5003-23341819265.1.bf644a82Da9PZK&id=622343426064&scene=taobao_shop) 首发于HDC 2020，是首批支持HarmonyOS 2.0的开发套件，亦是鸿蒙官方推荐套件，由润和软件HiHope量身打造，已在鸿蒙社区和广大鸿蒙开发者中得到广泛应用。

![wifi_iot](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/wifi_iot.png)

## 一、MessageQueue API

| API名称                   | 说明                                                         |
| ------------------------- | ------------------------------------------------------------ |
| osMessageQueueNew         | 创建和初始化一个消息队列                                     |
| osMessageQueueGetName     | 返回指定的消息队列的名字                                     |
| osMessageQueuePut         | 向指定的消息队列存放1条消息，如果消息队列满了，那么返回超时  |
| osMessageQueueGet         | 从指定的消息队列中取得1条消息，如果消息队列为空，那么返回超时 |
| osMessageQueueGetCapacity | 获得指定的消息队列的消息容量                                 |
| osMessageQueueGetMsgSize  | 获得指定的消息队列中可以存放的最大消息的大小                 |
| osMessageQueueGetCount    | 获得指定的消息队列中当前的消息数                             |
| osMessageQueueGetSpace    | 获得指定的消息队列中还可以存放的消息数                       |
| osMessageQueueReset       | 将指定的消息队列重置为初始状态                               |
| osMessageQueueDelete      | 删除指定的消息队列                                           |

## 二、代码分析

`osMessageQueueNew`创建一个消息队列

发送者每次将自己`count`的值与线程ID发送，并将`count`加1；

接受者从消息队列中获取一条信息，然后将其打印输出

```c
void sender_thread(void *arg) {
    static int count=0;
    message_entry sentry;
    (void)arg;
    while(1) {
        sentry.tid = osThreadGetId();
        sentry.count = count;
        printf("[Message Test] %s send %d to message queue.\r\n", osThreadGetName(osThreadGetId()), count);
        osMessageQueuePut(qid, (const void *)&sentry, 0, osWaitForever);
        count++;
        osDelay(5);
    }
}

void receiver_thread(void *arg) {
    (void)arg;
    message_entry rentry;
    while(1) {
        osMessageQueueGet(qid, (void *)&rentry, NULL, osWaitForever);
        printf("[Message Test] %s get %d from %s by message queue.\r\n", osThreadGetName(osThreadGetId()), rentry.count, osThreadGetName(rentry.tid));
        osDelay(3);
    }
}
```

主程序创建了三个消息发送者和两个消息接收者，然后调用相关的API确认消息队列的装填

```c
void rtosv2_msgq_main(void *arg) {
    (void)arg;

    qid = osMessageQueueNew(QUEUE_SIZE, sizeof(message_entry), NULL);

    osThreadId_t ctid1 = newThread("recevier1", receiver_thread, NULL);
    osThreadId_t ctid2 = newThread("recevier2", receiver_thread, NULL);
    osThreadId_t ptid1 = newThread("sender1", sender_thread, NULL);
    osThreadId_t ptid2 = newThread("sender2", sender_thread, NULL);
    osThreadId_t ptid3 = newThread("sender3", sender_thread, NULL);

    osDelay(20);
    uint32_t cap = osMessageQueueGetCapacity(qid);
    printf("[Message Test] osMessageQueueGetCapacity, capacity: %d.\r\n",cap);
    uint32_t msg_size =  osMessageQueueGetMsgSize(qid);
    printf("[Message Test] osMessageQueueGetMsgSize, size: %d.\r\n",msg_size);
    uint32_t count = osMessageQueueGetCount(qid);
    printf("[Message Test] osMessageQueueGetCount, count: %d.\r\n",count);
    uint32_t space = osMessageQueueGetSpace(qid);
    printf("[Message Test] osMessageQueueGetSpace, space: %d.\r\n",space);

    osDelay(80);
    osThreadTerminate(ctid1);
    osThreadTerminate(ctid2);
    osThreadTerminate(ptid1);
    osThreadTerminate(ptid2);
    osThreadTerminate(ptid3);
    osMessageQueueDelete(qid);
}
```



## 三、如何编译

1. 将此目录下的 `message.c` 和 `BUILD.gn` 复制到openharmony源码的`applications\sample\wifi-iot\app\iothardware`目录下，
2. 修改openharmony源码的`applications\sample\wifi-iot\app\BUILD.gn`文件，将其中的 `features` 改为：

```
    features = [
        "iothardware:message_demo",
    ]
```

   3.在openharmony源码顶层目录执行：`python build.py wifiiot`

## 四、运行结果

截取部分运行结果

```
[Message Test] osThreadNew(recevier1) success, thread id: 0xe89d8.
[Message Test] osThreadNew(recevier2) success, thread id: 0xe8974.
[Message Test] osThreadNew(sender1) success, thread id: 0xe84c4.
[Message Test] os[Message Test] sender1 send 0 to message queue.
[Message Test] sender2 send 1 to message queue.
ThreadNew(sender2) success, thread id: 0xe871c.
[Message Test] osThreadNew(sender3) success, thread id: 0xe8910.
[Message Test] recevier1 get 0 from sender1 by message queue.
[Message Test] recevier2 get 1 from sender2 by message queue.
[Message Test] sender3 send 2 to message queue.
[Message Test] sender1 send 3 to message queue.
[Message Test] sender2 send 4 to message queue.
....
[Message Test] sender3 send 11 to message queue.
[Message Test] recevier2 get 11 from sender3 by message queue.
[Message Test] sender1 send 12 to message queue.
[Message Test] sender2 send 13 to message queue.
[Message Test] recevier1 get 12 from sender1 by message queue.
[Message Test] osMessageQueueGetCapacity, capacity: 3.
[Message Test] osMessageQueueGetMsgSize, size: 8.
[Message Test] osMessageQueueGetCount, count: 1.
[Message Test] osMessageQueueGetSpace, space: 2.
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

