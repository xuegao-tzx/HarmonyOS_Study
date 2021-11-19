/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "iot_cloud.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include <cJSON.h>
#include <dtls_al.h>
#include <mqtt_al.h>
#include <oc_mqtt_al.h>
#include <oc_mqtt_profile.h>
#include <queue.h>


// we use this function to send the data here
extern int OcMqttReportString(char *deviceID, const char *jsonString);
extern int OcMqttMsgUpString(char *deviceID, const char *jsonString);
typedef enum {
    EN_MSG_TYPE_CMD = 0,
    EN_MSG_TYPE_REPORT,
    EN_MSG_TYPE_CONN,
    EN_MSG_TYPE_DISCONN,
    EN_MSG_TYPE_NOTIFY,
}CloudMsgType;

typedef struct
{
    char *requestID;
    char *payload;
}CloudCommandMsg;

typedef struct {
    char *jsonString;
}CloudReportMsg;

typedef struct {
    CloudMsgType msgType;
    union {
        CloudCommandMsg cmd;
        CloudReportMsg report;
        CloudReportMsg notify;
    }msg;
}ApplicationMsg;

typedef struct
{
    queue_t                     *applicationMsgQueue;
    int                          connected;
    int                          motor;
    osThreadId_t                 mainTaskID;
}CloudController;


#define CONFIG_APP_LIFETIME       60     ///< seconds
#define CONFIG_QUEUE_TIMEOUT      (5*1000)
static CloudController  g_cloudController;

static void HuaweiIoTSDKInit(void)
{
    dtls_al_init();
    mqtt_al_init();
    oc_mqtt_init();
}

/**
 * @brief This function used to report the data to the cloud
 *
*/
static void DealReportMsg(CloudReportMsg *reportMsg)
{
    OcMqttReportString(NULL, reportMsg->jsonString);
    return;
}

static void DealNotificationMsg(CloudReportMsg *notifyMsg)
{
    OcMqttMsgUpString(NULL, notifyMsg->jsonString);
}

/**
 * @brief This function used to deal with message received from the cloud
 *        we package the received data to the queue to do next step
*/
static int CloudMsgRcvCallBack(oc_mqtt_profile_msgrcv_t *msg)
{
    int    ret = 0;
    char  *buf = NULL;
    int    bufLen;
    ApplicationMsg *applicationMsg = NULL;

    if ((NULL == msg) || (msg->request_id == NULL) || (msg->type != EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_COMMANDS)) {
        RaiseLog(LOG_LEVEL_WARN,"Parameter is wrong format\n");
        return ret;
    }

    bufLen = sizeof(ApplicationMsg) + strlen(msg->request_id) + 1 + msg->msg_len + 1;
    buf = malloc(bufLen);
    if (NULL == buf) {
        RaiseLog(LOG_LEVEL_ERR,"No memory for the command buffer\n");
        return ret;
    }
    applicationMsg = (ApplicationMsg *)buf;
    buf += sizeof(ApplicationMsg);

    applicationMsg->msgType = EN_MSG_TYPE_CMD;
    applicationMsg->msg.cmd.requestID = buf;
    bufLen = strlen(msg->request_id);
    buf += bufLen + 1;
    memcpy(applicationMsg->msg.cmd.requestID, msg->request_id, bufLen);
    applicationMsg->msg.cmd.requestID[bufLen] = '\0';
    bufLen = msg->msg_len;
    applicationMsg->msg.cmd.payload = buf;
    memcpy(applicationMsg->msg.cmd.payload, msg->msg, bufLen);
    applicationMsg->msg.cmd.payload[bufLen] = '\0';
    RaiseLog(LOG_LEVEL_INFO,"GetCommand:reqID:%s payload:%s \n", \
            applicationMsg->msg.cmd.requestID,  applicationMsg->msg.cmd.payload);

    ret = queue_push(g_cloudController.applicationMsgQueue,applicationMsg,10);
    if (ret != 0){
        free(applicationMsg);
        RaiseLog(LOG_LEVEL_ERR,"Push the message to the queue failed");
    }
    return ret;
}

static void DealCommandMsg(CloudCommandMsg *cmd)
{
    int ret = -1;
    oc_mqtt_profile_cmdresp_t cmdresp;

    ret = CLOUD_CommandCallBack((const char *)(cmd->payload));
    ///< do the response
    cmdresp.paras = NULL;
    cmdresp.request_id = cmd->requestID;
    cmdresp.ret_code = (ret == 0 ? 0 : 1);
    cmdresp.ret_name = NULL;
    (void)oc_mqtt_profile_cmdresp(NULL, &cmdresp);
    return;
}

/**
 * @brief this is the cloud main task entry
 *        we deal all the message in the queue
*/
static int CloudMainTaskEntry(void *arg)
{
    ApplicationMsg *applicationMsg;
    uint32_t ret ;
    // receive the message from the queue ,maybe receive from the clould, or maybe from the local
    while (1) {
        applicationMsg = NULL;
        (void)queue_pop(g_cloudController.applicationMsgQueue,(void **)&applicationMsg,0xFFFFFFFF);
        if (applicationMsg != NULL) {
            RaiseLog(LOG_LEVEL_INFO,"GetMsgType:%d",applicationMsg->msgType);
            switch (applicationMsg->msgType){
                case EN_MSG_TYPE_CMD:
                    DealCommandMsg(&applicationMsg->msg.cmd);
                    break;
                case EN_MSG_TYPE_REPORT:
                    DealReportMsg(&applicationMsg->msg.report);
                    break;
                case EN_MSG_TYPE_NOTIFY:
                    DealNotificationMsg(&applicationMsg->msg.notify);
                    break;
                default:
                    break;
            }
            free(applicationMsg);
        }
    }
    return 0;
}


int CLOUD_Init(void)
{
    int ret = -1;
    /* create a queue to buffer the data */
    g_cloudController.applicationMsgQueue = queue_create("queue_rcvmsg",10,1);
    if(NULL ==  g_cloudController.applicationMsgQueue){
        printf("Create receive msg queue failed");
        return ret;
    }
    /* initialize the iot sdk */
    HuaweiIoTSDKInit();
    /* create a task to deal the send message or received message */
    osThreadAttr_t attr;
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.name = "IoTCloudMain";
    attr.stack_size = 1024*8;
    attr.priority = 24;
    if ((g_cloudController.mainTaskID = osThreadNew((osThreadFunc_t)CloudMainTaskEntry, NULL, &attr)) == NULL) {
        RaiseLog(LOG_LEVEL_ERR,"Could not create the iotcloud thread");
        return ret;
    }
    ret = 0;
    return ret;
}

int CLOUD_Deinit(void)
{
    int ret = -1;
    osThreadTerminate(g_cloudController.mainTaskID);
    g_cloudController.mainTaskID = NULL;
    queue_delete( g_cloudController.applicationMsgQueue);
    g_cloudController.applicationMsgQueue = NULL;
    return ret;
}

int CLOUD_Connect(const char *deviceID, const char *devicePwd, \
    const char *serverIP, const char *serverPort)
{
    int ret;
    oc_mqtt_profile_connect_t  connect_para;
    (void) memset( &connect_para, 0, sizeof(connect_para));

    connect_para.boostrap = 0;
    connect_para.device_id = (char *)deviceID;
    connect_para.device_passwd = (char *)devicePwd;
    connect_para.server_addr = (char *)serverIP;
    connect_para.server_port = (char *)serverPort;
    connect_para.life_time =  CONFIG_APP_LIFETIME;
    connect_para.rcvfunc = CloudMsgRcvCallBack;
    connect_para.security.type = EN_DTLS_AL_SECURITY_TYPE_NONE;
    ret = oc_mqtt_profile_connect(&connect_para);
    if ((ret == (int)en_oc_mqtt_err_ok)) {
        g_cloudController.connected = 1;
        ret = 0;
    }
    else {
        RaiseLog(LOG_LEVEL_ERR,"Could not connect to the huaweiIotPlatform:deviceID:%s devicePwd:%s serverIP:%s serverPort:%s", \
            deviceID, devicePwd, serverIP, serverPort);
        ret = -1;
    }
    return ret;
}

int CLOUD_Disconnect(void)
{
    int ret;
    ret = oc_mqtt_profile_disconnect();
    if (ret == (int)en_oc_mqtt_err_ok) {
        return 0;
    } else {
        return -1;
    }
}

int CLOUD_ReportMsg(const char *jsonString)
{
    int ret = -1;
    ApplicationMsg *applicationMsg = NULL;

    if (jsonString == NULL) {
        RaiseLog(LOG_LEVEL_WARN,"Parameter jsonString is NULL");
        return ret;
    }

    applicationMsg = malloc(sizeof(ApplicationMsg) + strlen(jsonString) + 1);
    if (NULL != applicationMsg) {
        applicationMsg->msgType = EN_MSG_TYPE_REPORT;
        applicationMsg->msg.report.jsonString = (char *)applicationMsg + sizeof(ApplicationMsg);
        strcpy(applicationMsg->msg.report.jsonString, jsonString);
        if (0 != queue_push(g_cloudController.applicationMsgQueue, applicationMsg, CONFIG_QUEUE_TIMEOUT)) {
            free(applicationMsg);
            RaiseLog(LOG_LEVEL_ERR,"Could not push message to the message queue");
        } else {
            ret = 0;
        }
    } else {
        RaiseLog(LOG_LEVEL_ERR,"Could not get the memory for the application message");
    }
    return ret;
}

int CLOUD_ReportNotification(int type, const char *enString, const char *chString)
{
    int ret = -1;
    ApplicationMsg *applicationMsg = NULL;
    char *jsonString = NULL;
    if (type >= NOTIFY_TYPE_LAST || enString == NULL || chString == NULL) {
        RaiseLog(LOG_LEVEL_WARN,"Parameter failed!\n");
        return -1;
    }

    jsonString = IotNotificationPackage(type, enString, chString);
    if (jsonString == NULL) {
        RaiseLog(LOG_LEVEL_WARN,"IotNotificationPackage jsonString failed! \n");
        return ret;
    }

    applicationMsg = malloc(sizeof(ApplicationMsg) + strlen(jsonString) + 1);
    if (NULL != applicationMsg) {
        applicationMsg->msgType = EN_MSG_TYPE_NOTIFY;
        applicationMsg->msg.report.jsonString = (char *)applicationMsg + sizeof(ApplicationMsg);
        strcpy(applicationMsg->msg.report.jsonString, jsonString);
        if (0 != queue_push(g_cloudController.applicationMsgQueue, applicationMsg, CONFIG_QUEUE_TIMEOUT)) {
            free(applicationMsg);
            RaiseLog(LOG_LEVEL_ERR,"Could not push message to the message queue");
        } else {
            ret = 0;
        }
    } else {
        RaiseLog(LOG_LEVEL_ERR,"Could not get the memory for the application message");
    }

    free(jsonString);

    return ret;
}