/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.example.distschedule.listener;

import com.alibaba.fastjson.JSONObject;
import com.example.distschedule.AddDeviceAbility;
import com.example.distschedule.database.AlarmBean;
import com.example.distschedule.database.Dbhelper;
import com.example.distschedule.toast.XToastUtils;
import com.example.distschedule.utils.DeviceInfoUtil;
import com.example.distschedule.utils.PreferencesUtil;
import com.j256.ormlite.dao.Dao;
import com.rabbitmq.client.*;
import com.socks.library.KLog;
import ohos.aafwk.content.Intent;
import ohos.aafwk.content.Operation;
import ohos.agp.utils.Color;
import ohos.app.Context;
import ohos.event.intentagent.IntentAgent;
import ohos.event.intentagent.IntentAgentConstant;
import ohos.event.intentagent.IntentAgentHelper;
import ohos.event.intentagent.IntentAgentInfo;
import ohos.event.notification.NotificationHelper;
import ohos.event.notification.NotificationRequest;
import ohos.event.notification.NotificationSlot;
import ohos.global.icu.text.SimpleDateFormat;
import ohos.global.icu.util.TimeZone;
import ohos.rpc.RemoteException;

import java.io.IOException;
import java.sql.SQLException;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.concurrent.TimeoutException;

public class DeviceStateListener {
    private static final String exchangeName = "deviceproperty_exchange";//监听队列名称
    private static final String rabbitmqHost = "81.71.17.188";
    private static final int rabbitmqPort = 5672;
    private static final String rabbitmqUser = "harmonyos";
    private static final String rabbitmqPassword = "harmonyos";
    private static DeviceStateListener instance;
    public Consumer consumer;
    private Context context;
    private Channel channel;

    public static DeviceStateListener getInstance() {
        if (instance == null) {
            instance = new DeviceStateListener();
        }
        return instance;
    }


    private void DeviceStateListener() {

    }

    public void listen(Context context) {
        this.context = context;
        ConnectionFactory factory = new ConnectionFactory();//MQ采用工厂模式来完成连接的创建
        //2.在工厂对象中设置连接信息(ip,port,virtualhost,username,password)
        factory.setHost(rabbitmqHost);//设置MQ安装的服务器ip地址
        factory.setPort(rabbitmqPort);//设置端口号
        //factory.setVirtualHost("/");//设置虚拟主机名称
        //MQ通过用户来管理
        factory.setUsername(rabbitmqUser);//设置用户名称
        factory.setPassword(rabbitmqPassword);//设置用户密码

        //3.通过工厂对象获取连接
        Connection connection = null;
        try {
            connection = factory.newConnection();

            //创建Channel
            channel = connection.createChannel();
        } catch (IOException | TimeoutException e) {
            e.printStackTrace();
        }
        KLog.i("create consumer");
        //获取Consumer
        consumer = new DefaultConsumer(channel) {
            @Override
            public void handleDelivery(String consumerTag, Envelope envelope, AMQP.BasicProperties properties, byte[] body) throws IOException {
                //将消息转换成String类型然后打印
                String msg = new String(body);
                KLog.i(msg);
                JSONObject jsonObject = JSONObject.parseObject(msg);
                int alarmType = jsonObject.getJSONObject("notify_data").getJSONObject("body").getJSONObject("content").getJSONObject("properties").getInteger("AlarmType");
                String alarmMsg = jsonObject.getJSONObject("notify_data").getJSONObject("body").getJSONObject("content").getJSONObject("properties").getJSONObject("ALarmMsg").getString("msg_zh");
                String alarmTime = jsonObject.getString("event_time");//20210913T023603Z
                String deviceId = jsonObject.getJSONObject("notify_data").getJSONObject("header").getString("device_id");
                String title = "紧急告警";
                if (alarmType == 0) {
                    title = "一般通知";
                } else if (alarmType == 1) {
                    title = "告警通知";
                }

                NotificationSlot slot = new NotificationSlot("slot_001", "slot_default", NotificationSlot.LEVEL_HIGH); // 创建notificationSlot对象
                slot.setDescription("NotificationSlotDescription");
                slot.setEnableVibration(true); // 设置振动提醒
                slot.setEnableLight(true); // 设置开启呼吸灯提醒
                slot.setLedLightColor(Color.RED.getValue());// 设置呼吸灯的提醒颜色
                try {
                    NotificationHelper.addNotificationSlot(slot);

                    int notificationId = 1;
                    NotificationRequest request = new NotificationRequest(notificationId);
                    request.setSlotId(slot.getId());

                    NotificationRequest.NotificationNormalContent content = new NotificationRequest.NotificationNormalContent();
                    content.setTitle(title)
                            .setText(alarmMsg);
                    NotificationRequest.NotificationContent notificationContent = new NotificationRequest.NotificationContent(content);
                    request.setContent(notificationContent); // 设置通知的内容
                    request.setIntentAgent(getIntentAgent(context, alarmType, alarmMsg, deviceId, alarmTime));
                    // 插入到数据库
                    insertAlarm(context, alarmType, alarmMsg, deviceId, alarmTime, PreferencesUtil.getUserId(context));
                    NotificationHelper.publishNotification(request);

                } catch (RemoteException ex) {
                    KLog.e("Exception occurred during addNotificationSlot invocation.");
                }
            }
        };
        String queueName = PreferencesUtil.getUserId(context) + "_" + DeviceInfoUtil.getDeviceId(context);
        // 声明一个队列
        try {
            channel.queueDeclare(queueName, false, false, false, null);
            channel.queueBind(queueName, exchangeName, PreferencesUtil.getUserId(context));
            channel.basicConsume(queueName, true, consumer);
        } catch (IOException e) {
            KLog.i(e.getMessage());
            e.printStackTrace();
        }
    }

    private void insertAlarm(Context context, int alarmType, String alarmMsg, String deviceId, String alarmTime, String userId) {
        String time;
        SimpleDateFormat df = null;
        Date after = null;
        try {
            df = new SimpleDateFormat("yyyyMMdd'T'HHmmss'Z'");
            df.setTimeZone(TimeZone.getTimeZone("UTC"));
            after = df.parse(alarmTime);
            df.applyPattern("yyyy-MM-dd HH:mm:ss");
            df.setTimeZone(TimeZone.getDefault());
            time = df.format(after);
        } catch (ParseException e) {
            e.printStackTrace();
            KLog.w(e.getMessage());
            time = alarmTime;
        }
        AlarmBean alarm = new AlarmBean(alarmType, deviceId, alarmMsg, time, userId);
        try {
            Dbhelper dbhelper = Dbhelper.getInstance(context);
            Dao alarmDao = dbhelper.getDao(AlarmBean.class);
            alarmDao.createIfNotExists(alarm);
        } catch (SQLException exception) {
            exception.printStackTrace();
            XToastUtils.warning("告警数据保存失败");
        }
    }


    private IntentAgent getIntentAgent(Context context, int alarmType, String alarmMsg, String deviceId, String alarmTime) {
        Intent intent = new Intent();
        // 指定要启动的Ability的BundleName和AbilityName字段
        // 将Operation对象设置到Intent中
        Operation operation = new Intent.OperationBuilder()
                .withDeviceId("")
                .withBundleName(context.getBundleName())
                .withAction("action.alarm")
                .withAbilityName(AddDeviceAbility.class.getName())
                .build();
        intent.setOperation(operation);
        intent.setParam("type", alarmType);
        intent.setParam("alarmMsg", alarmMsg);
        intent.setParam("deviceId", deviceId);
        intent.setParam("alarmTime", alarmTime);
        List<Intent> intentList = new ArrayList<>();
        intentList.add(intent);
        // 定义请求码
        int requestCode = 200;
        // 设置flags
        List<IntentAgentConstant.Flags> flags = new ArrayList<>();
        flags.add(IntentAgentConstant.Flags.UPDATE_PRESENT_FLAG);
        // 指定启动一个有页面的Ability
        IntentAgentInfo paramsInfo = new IntentAgentInfo(requestCode,
                IntentAgentConstant.OperationType.START_ABILITY, flags, intentList, null);
        // 获取IntentAgent实例
        return IntentAgentHelper.getIntentAgent(context, paramsInfo);
    }
}
