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
#include "iot_profile.h"
#include "cJSON.h"
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief we make the data to a json string and send it to the iot cloud engine
 *
 */
int IotProfile_Report(int beepStatus, float mq5, float threshold)
{
    int ret = -1;
    cJSON *root;
    char *jsonString;
    IotProfileService    service;
    IotProfileKV  kvBeepStatus;
    IotProfileKV  kvThreshold;
    IotProfileKV  kvMq5;
    /* package the data */
    service.eventTime = NULL;
    service.serviceID = "GasDetection";
    service.propertyLst = &kvBeepStatus;
    service.nxt = NULL;

    kvBeepStatus.key = "BeepStatus";
    kvBeepStatus.value = beepStatus ? "ON" : "OFF";
    kvBeepStatus.type = IOT_PROFILE_KEY_DATATYPE_STRING;
    kvBeepStatus.nxt = &kvMq5;

    kvMq5.key = "MQ5";
    kvMq5.value = &mq5;
    kvMq5.type = IOT_PROFILE_KEY_DATATYPE_FLOAT;
    kvMq5.nxt = &kvThreshold;

    kvThreshold.key = "Threshold";
    kvThreshold.value = &threshold;
    kvThreshold.type = IOT_PROFILE_KEY_DATATYPE_FLOAT;
    kvThreshold.nxt = NULL;

    jsonString = IoTProfilePackage(&service);
    if (NULL != jsonString) {
        RaiseLog(LOG_LEVEL_INFO, "jsonString:%s", jsonString);
        ret = CLOUD_ReportMsg(jsonString);
        free(jsonString);
    } else {
        RaiseLog(LOG_LEVEL_ERR, "format the report message error");
    }
    return ret;
}


/**
 * @brief deal the message received from the queue
 *
 */
static int DealSetBeep(cJSON *objCmd)
{
    int ret = -1;
    cJSON *objParas = NULL;
    cJSON *objPara = NULL;
    CommandParamSetBeep setBeepParam;

    if ((objParas = cJSON_GetObjectItem(objCmd, "paras")) == NULL) {
        RaiseLog(LOG_LEVEL_ERR, "Could not get paras in json");
        return ret;
    }

    if ((objPara = cJSON_GetObjectItem(objParas, "BeepStatus")) != NULL) {
        if (0 == strcmp(cJSON_GetStringValue(objPara), "ON")) {
            setBeepParam.status = CN_BOARD_SWITCH_ON;
        }
        else {
            setBeepParam.status = CN_BOARD_SWITCH_OFF;
        }

        ret = IotProfile_CommandCallback(CLOUD_COMMAND_SETBEEP, &setBeepParam);
    } else {
        RaiseLog(LOG_LEVEL_ERR, "Para not exist");
    }
    return ret;
}

static int DealSetThreshold(cJSON *objCmd)
{
    int ret = -1;
    cJSON *objParas = NULL;
    cJSON *objPara = NULL;
    CommandParamSetThreshold setThresholdParam;

    if ((objParas = cJSON_GetObjectItem(objCmd, "paras")) == NULL) {
        RaiseLog(LOG_LEVEL_ERR, "Could not get paras in json");
        return ret;
    }

    if ((objPara = cJSON_GetObjectItem(objParas, "Threshold")) != NULL) {
        setThresholdParam.threshold = (float) cJSON_GetNumberValue(objPara);
        ret = IotProfile_CommandCallback(CLOUD_COMMAND_SETTHRESHOLD, &setThresholdParam);
    } else {
        RaiseLog(LOG_LEVEL_ERR, "Para not exist");
    }
    return ret;
}

int CLOUD_CommandCallBack(const char *jsonString)
{
    cJSON *objRoot = NULL;
    cJSON *objCmdName = NULL;
    int ret = -1;
    if (jsonString == NULL) {
        return ret;
    }

    if ((objRoot = cJSON_Parse(jsonString)) == NULL) {
        RaiseLog(LOG_LEVEL_ERR, "could not parse the payload\r\n");
        goto EXIT_JSONPARSE;
    }

    if ((objCmdName = cJSON_GetObjectItem(objRoot, "command_name")) == NULL) {
        RaiseLog(LOG_LEVEL_ERR, "%s:could not get the cmd name from json\r\n");
        goto EXIT_CMDOBJ;
    }

    if (0 == strcmp(cJSON_GetStringValue(objCmdName), "SetBeep")) {
        ret = DealSetBeep(objRoot);
    } else if (0 == strcmp(cJSON_GetStringValue(objCmdName), "SetThreshold")) {
        ret = DealSetThreshold(objRoot);
    } else {
        RaiseLog(LOG_LEVEL_ERR, "unresolved command:%d\r\n", cJSON_GetStringValue(objCmdName));
    }

EXIT_CMDOBJ:
EXIT_JSONPARSE:
    if (objRoot != NULL) {
        cJSON_Delete(objRoot);
    }
    return ret;
}




