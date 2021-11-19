/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
package com.example.distschedule.listener;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import com.example.distschedule.exception.DistscheduleDeviceException;
import com.example.distschedule.service.DeviceService;
import org.apache.commons.lang3.StringUtils;
import org.apache.qpid.jms.JmsConnection;
import org.apache.qpid.jms.JmsConnectionFactory;
import org.apache.qpid.jms.JmsConnectionListener;
import org.apache.qpid.jms.JmsQueue;
import org.apache.qpid.jms.message.JmsInboundMessageDispatch;
import org.apache.qpid.jms.transports.TransportOptions;
import org.apache.qpid.jms.transports.TransportSupport;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;

import javax.annotation.PostConstruct;
import javax.jms.*;
import javax.naming.Context;
import javax.naming.InitialContext;
import java.net.URI;
import java.util.Hashtable;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * 接收设备告警消息
 */
@Component
public class DeviceMsgListener {
    protected static final Logger LOGGER = LoggerFactory.getLogger("DeviceMsgLog");

    @Autowired
    private RabbitTemplate rabbitTemplate;

    @Autowired
    private DeviceService deviceService;

    @Value("${spring.rabbitmq.exchange.deviceproperty}")
    private String devicePropertyExchange;

    /**
     * 异步线程池，参数可以根据业务特点作调整，也可以用其他异步方式来处理。
     */
    public final ExecutorService executorService = new ThreadPoolExecutor(Runtime.getRuntime().availableProcessors(),
            Runtime.getRuntime().availableProcessors() * 2, 60,
            TimeUnit.SECONDS, new LinkedBlockingQueue<>(5000));
    /**
     * 连接凭证接入键值
     */
    @Value("${cloud.iot.amqp.accessKey}")
    private String accessKey = "";

    /**
     * 连接凭证接入码
     */
    @Value("${cloud.iot.amqp.password}")
    private String password;

    /**
     * 按照qpid-jms的规范，组装连接URL。
     */
    @Value("${cloud.iot.amqp.connectionUrl}")
    private String connectionUrl;


    /**
     * 队列名，可以使用默认队列 DefaultQueue
     */
    @Value("${cloud.iot.amqp.queueMsgName}")
    private String queueName = "";

    private Connection connection;
    private Session session;
    private MessageConsumer consumer;
    private long lastReconnectTime = 0;
    private AtomicBoolean isReconnecting = new AtomicBoolean(false);

    @PostConstruct
    private void initIOTAmqpListener() throws Exception {
        LOGGER.info("Init " + this.getClass().getSimpleName());

        createConsumer();

        // 处理消息有两种方式
        // 1，主动拉数据（推荐），参照receiveMessage()
        new Thread(() -> {
            receiveMessage();
        }).start();
        LOGGER.info("End Init " + this.getClass().getSimpleName());

        // 2, 添加监听，参照consumer.setMessageListener(messageListener), 服务端主动推数据给客户端，但得考虑接受的数据速率是客户能力能够承受住的
        // consumer.setMessageListener(messageListener);
        // 防止主程序退出,这里休眠了60s,60s后程序会结束
        // Thread.sleep( 60 * 1000L);
        // shutdown();
    }

    private void createConsumer() throws Exception {
        long timeStamp = System.currentTimeMillis();
        //UserName组装方法，请参见文档：AMQP客户端接入说明。
        String userName = "accessKey=" + accessKey + "|timestamp=" + timeStamp;
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

        // 创建 Session
        // Session.CLIENT_ACKNOWLEDGE: 收到消息后，需要手动调用message.acknowledge()。
        // Session.AUTO_ACKNOWLEDGE: SDK自动ACK（推荐）。
        session = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
        connection.start();
        connection.setExceptionListener(exceptionListener);
        // 创建 consumer
        consumer = session.createConsumer(new JmsQueue(getQueueName()));
    }

    private void receiveMessage() {
        while (true) {
            try {
                if (consumer != null) {
                    Message message = consumer.receive();
                    // 建议异步处理收到的消息，确保receiveMessage函数里没有耗时逻辑。
                    executorService.execute(() -> processMessage(message));
                } else {
                    Thread.sleep(1000L);
                }
            } catch (Exception e) {
                LOGGER.error("receiveMessage hand an exception: " + e.getMessage());
                e.printStackTrace();
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
                    LOGGER.info("reconnect hand an exception: " + e.getMessage());
                    e.printStackTrace();
                }
            }
        }
    }

    private void shutdown() {
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

    private ExceptionListener exceptionListener = new ExceptionListener() {
        @Override
        public void onException(JMSException e) {
            e.printStackTrace();
            LOGGER.info("connection has an exception:" + e);
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
            LOGGER.info("onConnectionEstablished, remoteUri:" + remoteURI);
        }

        /**
         * 尝试过最大重试次数之后，最终连接失败。
         */
        @Override
        public void onConnectionFailure(Throwable error) {
            LOGGER.info("onConnectionFailure, " + error.getMessage());
        }

        /**
         * 连接中断。
         */
        @Override
        public void onConnectionInterrupted(URI remoteURI) {
            LOGGER.info("onConnectionInterrupted, remoteUri:" + remoteURI);
        }

        /**
         * 连接中断后又自动重连上。
         */
        @Override
        public void onConnectionRestored(URI remoteURI) {
            LOGGER.info("onConnectionRestored, remoteUri:" + remoteURI);
        }

        @Override
        public void onInboundMessage(JmsInboundMessageDispatch envelope) {
            LOGGER.info("onInboundMessage, " + envelope);
        }

        @Override
        public void onSessionClosed(Session session, Throwable cause) {
            LOGGER.info("onSessionClosed, session=" + session + ", cause =" + cause);
        }

        @Override
        public void onConsumerClosed(MessageConsumer consumer, Throwable cause) {
            LOGGER.info("MessageConsumer, consumer=" + consumer + ", cause =" + cause);
        }

        @Override
        public void onProducerClosed(MessageProducer producer, Throwable cause) {
            LOGGER.info("MessageProducer, producer=" + producer + ", cause =" + cause);
        }
    };

    protected String getQueueName() {
        return this.queueName;
    }

    /**
     * 在这里处理您收到消息后的具体业务逻辑。
     */
    protected void processMessage(Message message) {
        try {
            //{"resource":"device.status","event":"update","event_time":"20210901T012225Z","notify_data":{"header":{"app_id":"6989a30bea954354b662920f369f4503","device_id":"612779690ad1ed028666ec69_madixin","node_id":"madixin","product_id":"612779690ad1ed028666ec69","gateway_id":"612779690ad1ed028666ec69_madixin"},"body":{"status":"ONLINE","last_online_time":"20210901T012225Z"}}}
            String body = message.getBody(String.class);
            String content = new String(body);
            LOGGER.info("DeviceMsgListener receive an message, the content is " + content);
            JSONObject contentBody = JSON.parseObject(content);
            //1.获取device_id和上报属性
            String deviceId = contentBody.getJSONObject("notify_data").getJSONObject("header").getString("device_id");
            String strbody = contentBody.getJSONObject("notify_data").getString("body");
            if (StringUtils.isBlank(deviceId) || StringUtils.isBlank(strbody)) {
                LOGGER.error("Invalid DeviceMsgListener deviceId= " + deviceId + ",strbody=" + strbody);
                return;
            }
            //2.根据device_id查询user_id
            try {
                List<String> userIds = deviceService.getUsersByDeviceId(deviceId);
                //3.使用user_id为routine_key路由，给rabbitmq发送消息
                for (String userId : userIds) {
                    rabbitTemplate.convertAndSend(this.devicePropertyExchange, userId, content);
                }
            } catch (DistscheduleDeviceException e) {
                LOGGER.error("DeviceMsgListener error:" + e.getErrorCode().getDescription() + " , deviceId = " + deviceId);
            }
        } catch (Exception e) {
            LOGGER.info("DeviceMsgListener processMessage occurs error: " + e.getMessage());
            e.printStackTrace();
        }
    }
}
