/*
 * Copyright (c) 2021 KaiHong Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.view.test.slice;


import cn.hutool.json.JSONUtil;
import com.view.test.ResourceTable;
import com.view.test.bean.ServicesDTO;
import com.view.test.bean.TestParkBean;
import com.view.test.utils.ToolUtils;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.agp.components.*;

import ohos.app.dispatcher.task.TaskPriority;
import ohos.data.DatabaseHelper;
import ohos.data.preferences.Preferences;
import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.multimodalinput.event.KeyEvent;
import org.apache.qpid.jms.JmsConnection;
import org.apache.qpid.jms.JmsConnectionFactory;
import org.apache.qpid.jms.JmsConnectionListener;
import org.apache.qpid.jms.JmsQueue;
import org.apache.qpid.jms.message.JmsInboundMessageDispatch;
import org.apache.qpid.jms.transports.TransportOptions;
import org.apache.qpid.jms.transports.TransportSupport;

import javax.jms.*;
import javax.naming.Context;
import javax.naming.InitialContext;

import java.net.URI;

import java.util.Hashtable;
import java.util.List;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicBoolean;


public class MainAbilitySlice extends AbilitySlice {


    private static final HiLogLabel LABEL_LOG = new HiLogLabel(HiLog.LOG_APP, 0x11101, "MainAbilitySlice");


    /**
     * 异步线程池，参数可以根据业务特点作调整，也可以用其他异步方式来处理。
     */
    private final static ExecutorService executorService = new ThreadPoolExecutor(Runtime.getRuntime().availableProcessors(),
            Runtime.getRuntime().availableProcessors() * 2, 60,
            TimeUnit.SECONDS, new LinkedBlockingQueue<>(5000));

    /**
     * 连接凭证接入键值
     */
    private static String accessKey = "goy7cYSR";

    /**
     * 连接凭证接入码
     */
    private static String password = "euuottdbwwEYTNILsbk2lsPX9Xxbm3yt";

    /**
     * 队列名，可以使用默认队列DefaultQueue
     */
    private static String queueName = "Park1234";
//    private static String queueName = "TestPark";

    /**
     * 按照qpid-jms的规范，组装连接URL。
     */
    private static String connectionUrl = "amqps://a16103bd0e.iot-amqps.cn-north-4.myhuaweicloud.com:5671?amqp.vhost=default&amqp.idleTimeout=8000&amqp.saslMechanisms=PLAIN";

//    private static String connectionUrl = "amqps://${UUCID}.iot-amqps.cn-north-4.myhuaweicloud.com:5671?amqp.vhost=default&amqp.idleTimeout=8000&amqp.saslMechanisms=PLAIN";

    private static Connection connection;
    private static Session session;
    private static MessageConsumer consumer;
    private static  MessageProducer producer;
    private static long lastReconnectTime = 0;
    private static AtomicBoolean isReconnecting = new AtomicBoolean(false);


    //页面组件
    Image mImageBack = null;
    Text mTextP = null;
    StackLayout mSlHasCar = null;
    Text mTextCar = null;
    Text mTextCarInTime = null;
    Text mTextCarOutTime = null;

    boolean mIsPark = false;
    String mEventTime = null;

    //最近驶入时间
    String mLastCarInTime = null;

    //最后驶离时间
    String mLastCarOutTime = null;

    private final static String FILE_NAME = "data";
    private final static String LAST_CAR_IN_TIME = "car_in_time";
    private final static String LAST_CAR_OUT_TIME = "car_out_time";


    DatabaseHelper databaseHelper = null;
    Preferences preferences = null;

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_main);


        databaseHelper = new DatabaseHelper(this);// context入 参类型为ohos.app.Context
        preferences = databaseHelper.getPreferences(FILE_NAME);

        //获取之前保存的数据，显示时间
        if (preferences != null) {
            mLastCarInTime = preferences.getString(LAST_CAR_IN_TIME, "");
            mLastCarOutTime = preferences.getString(LAST_CAR_OUT_TIME, "");
        }


        initComponent();

        getGlobalTaskDispatcher(TaskPriority.DEFAULT).asyncDispatch(new Runnable() {
            @Override
            public void run() {
                amqpConnect();
            }
        });


    }


    @Override
    public boolean onKeyDown(int keyCode, KeyEvent keyEvent) {
        HiLog.info(LABEL_LOG, "=============================onKeyDown keycode = " + keyCode);
        if (2 == keyCode) {
            shutdown();
        }
        return super.onKeyDown(keyCode, keyEvent);
    }

    @Override
    protected void onStop() {
        super.onStop();
    }

    private void initComponent() {
        mImageBack = (Image) findComponentById(ResourceTable.Id_back);
        mImageBack.setClickedListener(new Component.ClickedListener() {
            @Override
            public void onClick(Component component) {
                shutdown();
                terminate();

            }
        });
        mTextP = (Text) findComponentById(ResourceTable.Id_text_not_has_car);
        mSlHasCar = (StackLayout) findComponentById(ResourceTable.Id_sl_has_car);

        mTextCar = (Text) findComponentById(ResourceTable.Id_tx_car);

        mTextCarInTime = (Text) findComponentById(ResourceTable.Id_tx_car_in_time);
        mTextCarInTime.setText(mLastCarInTime);

        mTextCarOutTime = (Text) findComponentById(ResourceTable.Id_tx_car_out_time);
        mTextCarOutTime.setText(mLastCarOutTime);

        if (mIsPark) {
            hasCarDisplay();
        } else {
            noCarDisplay();
        }
    }

    private void hasCarDisplay() {
        mTextP.setVisibility(Component.INVISIBLE);
        mSlHasCar.setVisibility(Component.VISIBLE);
        mTextCar.setText(ResourceTable.String_string_has_car);
        mTextCarInTime.setText(mLastCarInTime);
        mTextCarOutTime.setText(mLastCarOutTime);

        //驶入时间，驶离时间，写入持久化
        HiLog.info(LABEL_LOG, "汽车驶入时间：" + mLastCarInTime);
        HiLog.info(LABEL_LOG, "汽车驶离时间：" + mLastCarOutTime);
        ToolUtils.writeToSP(preferences, LAST_CAR_IN_TIME, mLastCarInTime);
        ToolUtils.writeToSP(preferences, LAST_CAR_OUT_TIME, mLastCarOutTime);
    }

    private void noCarDisplay() {
        mTextP.setVisibility(Component.VISIBLE);
        mSlHasCar.setVisibility(Component.INVISIBLE);
        mTextCar.setText(ResourceTable.String_string_not_has_car);
        mTextCarInTime.setText(mLastCarInTime);



        mTextCarOutTime.setText(mLastCarOutTime);

        //驶入时间，驶离时间，写入持久化
        HiLog.info(LABEL_LOG, "汽车驶入时间：" + mLastCarInTime);
        HiLog.info(LABEL_LOG, "汽车驶离时间：" + mLastCarOutTime);
        ToolUtils.writeToSP(preferences, LAST_CAR_IN_TIME, mLastCarInTime);
        ToolUtils.writeToSP(preferences, LAST_CAR_OUT_TIME, mLastCarOutTime);
    }


    void amqpConnect() {
        HiLog.info(LABEL_LOG, "amqpTest");
        try {
            createConsumer();
        } catch (Exception e) {
            //异常情况，重新连接
//            HiLog.error(LABEL_LOG, "amqpTest exception will reconnect");
            reconnect();
            e.printStackTrace();
        }

        // 处理消息有两种方式
        // 1，主动拉数据（推荐），参照receiveMessage()
//        receiveMessage();

        //使用第二种方式：

        // 2, 添加监听，参照consumer.setMessageListener(messageListener), 服务端主动推数据给客户端，但得考虑接受的数据速率是客户能力能够承受住的
        try {
            consumer.setMessageListener(messageListener);
        } catch (JMSException e) {
            e.printStackTrace();
        }
        // 防止主程序退出,这里休眠了60s,60s后程序会结束
        // Thread.sleep( 60 * 1000L);
        // shutdown();


    }

    private void createConsumer() throws Exception {
        HiLog.info(LABEL_LOG, "createConsumer");
        long timeStamp = System.currentTimeMillis();
        //UserName组装方法，请参见文档：AMQP客户端接入说明。
        String userName = "accessKey=" + accessKey + "|timestamp=" + timeStamp + "|";
        HiLog.info(LABEL_LOG, "userName = " + userName);
        HiLog.info(LABEL_LOG, "password = " + password);
        Hashtable<String, String> hashtable = new Hashtable<>();
        hashtable.put("connectionfactory.HwConnectionURL", connectionUrl);
        hashtable.put(Context.INITIAL_CONTEXT_FACTORY, "org.apache.qpid.jms.jndi.JmsInitialContextFactory");
        Context context = new InitialContext(hashtable);
        JmsConnectionFactory cf = (JmsConnectionFactory) context.lookup("HwConnectionURL");

        //信任服务端
        TransportOptions to = new TransportOptions();
        to.setTrustAll(true);
        cf.setSslContext(TransportSupport.createJdkSslContext(to));


        // 创建连接
        Connection connection = cf.createConnection(userName, password);

        ((JmsConnection) connection).addConnectionListener(myJmsConnectionListener);


        connection.setExceptionListener(exceptionListener);
        connection.start();
        HiLog.info(LABEL_LOG, "createConsumer connection.start");

        // 创建 Session
        // Session.CLIENT_ACKNOWLEDGE: 收到消息后，需要手动调用message.acknowledge()。
        // Session.AUTO_ACKNOWLEDGE: SDK自动ACK（推荐）。
        session = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
        HiLog.info(LABEL_LOG, "createConsumer connection.createSession");

        // 创建 consumer
        consumer = session.createConsumer(new JmsQueue(queueName));


        HiLog.info(LABEL_LOG, "createConsumer end");
    }

    private void receiveMessage() {
        HiLog.info(LABEL_LOG, "receiveMessage");
        while (true) {
            try {
                if (consumer != null) {
                    Message message = consumer.receive(1000);
                    if (message != null) {
                        HiLog.error(LABEL_LOG, "receiveMessage 222 msg : " + message.getBody(String.class));
                    }

                    // 建议异步处理收到的消息，确保receiveMessage函数里没有耗时逻辑。
                    executorService.execute(() -> processMessage(message));
                } else {
                    Thread.sleep(1000L);
                }
            } catch (Exception e) {
                HiLog.error(LABEL_LOG, "receiveMessage Exception e");
                System.out.println("receiveMessage hand an exception: " + e.getMessage());
                e.printStackTrace();
                try {
                    Thread.sleep(1000L);
                } catch (InterruptedException interruptedException) {
                    interruptedException.printStackTrace();
                }
            }
        }
    }

    /**
     * 客户端断开需要重连
     */
    private void reconnect() {
        if (isReconnecting.compareAndSet(false, true)) {
            while (true) {
                try {
                    // 防止重连次数太多，重连时间间隔15s
                    if (System.currentTimeMillis() - lastReconnectTime < 15 * 1000L) {
                        Thread.sleep(15 * 1000L);
                    }
                    shutdown();
                    createConsumer();
                    lastReconnectTime = System.currentTimeMillis();
                    isReconnecting.set(false);
                    break;
                } catch (Exception e) {
                    lastReconnectTime = System.currentTimeMillis();
                    System.out.println("reconnect hand an exception: " + e.getMessage());
                    e.printStackTrace();
                }
            }
        }

    }

    /**
     * 在这里处理您收到消息后的具体业务逻辑。
     */
    private void processMessage(Message message) {
        if (null == message) {
//            HiLog.info(LABEL_LOG, "processMessage message is null");
            return;
        }

        List<ServicesDTO> list = null;

        try {
            String body = message.getBody(String.class);
            //接收到的数据
            String content = new String(body);
            System.out.println("receive an message, the content is " + content);

            TestParkBean bean = JSONUtil.toBean(content, TestParkBean.class);
            if (null != bean.getNotify_data() && null != bean.getNotify_data().getBody()) {
                list = bean.getNotify_data().getBody().getServices();
                if (null != list && list.size() > 0) {
                    String dateStr = list.get(0).getEvent_time();
                    // 时间上报时间"2020-01-23 12:23:56"
                    mEventTime = ToolUtils.getDateStr(dateStr);
                    ServicesDTO.PropertiesDTO propertiesDTO = list.get(0).getProperties();

                    HiLog.info(LABEL_LOG, "park value = " + propertiesDTO.getIsPark() + "  mIsPark = " + mIsPark);

                    if (0 == propertiesDTO.getIsPark() && mIsPark) {
                        mIsPark = false;
                        mLastCarOutTime = mEventTime;
                        HiLog.info(LABEL_LOG, "last car out time = " + mLastCarOutTime);

                    } else if (propertiesDTO.getIsPark() > 0 && !mIsPark){
                        mIsPark = true;
                        mLastCarInTime = mEventTime;
                        HiLog.info(LABEL_LOG, "last car in time = " + mLastCarInTime);
                    }
                }
            } else {
                HiLog.error(LABEL_LOG, "processMessage message is exception!");
            }

            //处理数据结束后，需要更新UI
            getUITaskDispatcher().asyncDispatch(new Runnable() {
                @Override
                public void run() {
                    if (mIsPark) {
                        hasCarDisplay();
                    } else {
                        noCarDisplay();
                    }
                }
            });


        } catch (Exception e) {
            System.out.println("processMessage occurs error: " + e.getMessage());
            e.printStackTrace();
        }
    }

    private static void shutdown() {
        HiLog.info(LABEL_LOG, "shutdown");
        if (consumer != null) {
            try {
                consumer.close();
            } catch (JMSException e) {
                e.printStackTrace();
            }
        }
        if (session != null) {
            try {
                session.close();
            } catch (JMSException e) {
                e.printStackTrace();
            }
        }
        if (connection != null) {
            try {
                connection.setExceptionListener(null);
                connection.close();
            } catch (JMSException e) {
                e.printStackTrace();
            }
        }
    }

    private MessageListener messageListener = new MessageListener() {
        @Override
        public void onMessage(Message message) {
            try {
                HiLog.info(LABEL_LOG, "onMessage message = " + message.getBody(String.class));
                // 建议异步处理收到的消息，确保onMessage函数里没有耗时逻辑。
                // 如果业务处理耗时过程过长阻塞住线程，可能会影响SDK收到消息后的正常回调。
                executorService.submit(() -> processMessage(message));
            } catch (Exception e) {
                System.out.println("submit task occurs exception: " + e.getMessage());
                e.printStackTrace();
            }
        }
    };

    private ExceptionListener exceptionListener = new ExceptionListener() {
        @Override
        public void onException(JMSException e) {
            e.printStackTrace();
            System.out.println("connection has an exception:" + e);
            // 链接发生异常，需要重连
            reconnect();
        }
    };

    private static JmsConnectionListener myJmsConnectionListener = new JmsConnectionListener() {
        /**
         * 连接成功建立。
         */
        @Override
        public void onConnectionEstablished(URI remoteURI) {
            HiLog.error(LABEL_LOG, "onConnectionEstablished, remoteUri:" + remoteURI);
            System.out.println("onConnectionEstablished, remoteUri:" + remoteURI);
        }

        /**
         * 尝试过最大重试次数之后，最终连接失败。
         */
        @Override
        public void onConnectionFailure(Throwable error) {
            HiLog.error(LABEL_LOG, "onConnectionFailure, " + error.getMessage());
            System.out.println("onConnectionFailure, " + error.getMessage());
        }

        /**
         * 连接中断。
         */
        @Override
        public void onConnectionInterrupted(URI remoteURI) {
            HiLog.error(LABEL_LOG, "onConnectionInterrupted, remoteUri:" + remoteURI);
            System.out.println("onConnectionInterrupted, remoteUri:" + remoteURI);
        }

        /**
         * 连接中断后又自动重连上。
         */
        @Override
        public void onConnectionRestored(URI remoteURI) {
            HiLog.error(LABEL_LOG, "onConnectionRestored, remoteUri:" + remoteURI);
            System.out.println("onConnectionRestored, remoteUri:" + remoteURI);
        }

        @Override
        public void onInboundMessage(JmsInboundMessageDispatch envelope) {
            System.out.println("onInboundMessage, " + envelope);
        }

        @Override
        public void onSessionClosed(Session session, Throwable cause) {
            System.out.println("onSessionClosed, session=" + session + ", cause =" + cause);
        }

        @Override
        public void onConsumerClosed(MessageConsumer consumer, Throwable cause) {
            System.out.println("MessageConsumer, consumer=" + consumer + ", cause =" + cause);
        }

        @Override
        public void onProducerClosed(MessageProducer producer, Throwable cause) {
            System.out.println("MessageProducer, producer=" + producer + ", cause =" + cause);
        }
    };

    @Override
    protected void onResult(int requestCode, Intent resultIntent) {
        super.onResult(requestCode, resultIntent);

    }

    @Override
    public void onActive() {
        super.onActive();
        HiLog.error(LABEL_LOG, "onActive()");
    }

    @Override
    public void onForeground(Intent intent) {
        super.onForeground(intent);
    }
}
