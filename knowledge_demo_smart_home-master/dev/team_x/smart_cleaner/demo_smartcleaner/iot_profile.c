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
int IotProfile_Report(int mainLedStatus, int pwmLedDutyCycle, int lightIntensity)
{
    int ret = -1;
    cJSON *root;
    char *jsonString;
    IotProfileService    service;
    IotProfileKV  kvMainLedStatus;

    /* package the data */
    service.eventTime = NULL;
    service.serviceID = "CleanData";
    service.propertyLst = &kvMainLedStatus;
    service.nxt = NULL;

    kvMainLedStatus.key = "CleanerStatus";
    kvMainLedStatus.value = mainLedStatus ? "ON" : "OFF";
    kvMainLedStatus.type = IOT_PROFILE_KEY_DATATYPE_STRING;
    kvMainLedStatus.nxt = NULL;

    jsonString = IoTProfilePackage(&service);
    if ( NULL != jsonString) {
        RaiseLog(LOG_LEVEL_INFO,"jsonString:%s", jsonString);

        ret = CLOUD_ReportMsg(jsonString);
        free(jsonString);
    } else {
        RaiseLog(LOG_LEVEL_ERR,"format the report message error");
    }
    return ret;
}


/**
 * @brief deal the message received from the queue
 *
 */
static int DealSetMainLed(cJSON *objCmd)
{
    int ret = -1;
    cJSON *objParas = NULL;
    cJSON *objPara = NULL;
    CommandParamSetLamp setLedParam;
    printf("-----------------DealSetMainLed\r\n");
    if ((objParas = cJSON_GetObjectItem(objCmd, "paras")) == NULL) {
        RaiseLog(LOG_LEVEL_ERR,"Could not get paras in json");
        return ret;
    }

    if ((objPara = cJSON_GetObjectItem(objParas, "GoTOClean")) != NULL) {
        printf("-----------------GoTOClean\r\n");
        if (0 == strcmp(cJSON_GetStringValue(objPara), "ON")) {
            setLedParam.status = CN_BOARD_SWITCH_ON;
        }
        else {
            setLedParam.status = CN_BOARD_SWITCH_OFF;
        }

        ret = IotProfile_CommandCallback(CLOUD_COMMAND_SETLED, &setLedParam);
    } else {
        RaiseLog(LOG_LEVEL_ERR,"Para not exist");
    }
    return ret;
}

static int DealSetAuxiliaryLed(cJSON *objCmd)
{
    int ret = -1;
    cJSON *objParas = NULL;
    cJSON *objPara = NULL;
    CommandParamSetDutyCycle setDutyCycleParam;

    if ((objParas = cJSON_GetObjectItem(objCmd, "paras")) == NULL) {
        RaiseLog(LOG_LEVEL_ERR,"Could not get paras in json");
        return ret;
    }

    if ((objPara = cJSON_GetObjectItem(objParas, "DutyCycle")) != NULL) {
        setDutyCycleParam.dutyCycle = cJSON_GetNumberValue(objPara);
        ret = IotProfile_CommandCallback(CLOUD_COMMAND_SETDUTCYCLE, &setDutyCycleParam);
    } else {
        RaiseLog(LOG_LEVEL_ERR,"Para not exist");
    }
    return ret;
}

static int GetScheduleWeekDays(const char *weekdaystr, int *weekday, int length)
{
    int num = 0;
    char *ps, *pe;

    ps = weekdaystr;
    for (int i = 0; i < length; i++) {
        char day[2] = {0};
        pe = strchr(ps, ',');
        if (pe == NULL) {
            num = i + 1;
            weekday[i] = atoi(ps);
            break;
        }

        day[0] = *ps;
        ps = pe + 1;
        weekday[i] = atoi(day);
    }

    return num;
}
static int DealSetShedule(cJSON *objCmd)
{
    printf("zhangyuhang  debug for DealSetShedule %d\n", __LINE__);
    
    int ret = -1;
    cJSON *objParas = NULL;
    cJSON *objPara = NULL;
    CommandParamSetShedule setSheduleParam;
    memset(&setSheduleParam, 0x00, sizeof(CommandParamSetShedule));

    if ((objParas = cJSON_GetObjectItem(objCmd, "paras")) == NULL) {
        RaiseLog(LOG_LEVEL_ERR,"Paras not exist");
        return ret;
    }

    printf("zhangyuhang  debug for DealSetShedule %d\n", __LINE__);

    if ((objPara = cJSON_GetObjectItem(objParas, "Day")) != NULL) {
        char *weekdays = cJSON_GetStringValue(objPara);
        setSheduleParam.num = GetScheduleWeekDays(weekdays, setSheduleParam.day, CN_MAX_WEEKDAY);
    } else {
        return ret;
    }
  printf("zhangyuhang  debug for DealSetShedule %d\n", __LINE__);
    if ((objPara = cJSON_GetObjectItem(objParas, "StartHour")) != NULL) {
        setSheduleParam.startHour = cJSON_GetNumberValue(objPara);
    } else {
        return ret;
    }
printf("zhangyuhang  debug for DealSetShedule %d\n", __LINE__);
    if ((objPara = cJSON_GetObjectItem(objParas, "StartMinute")) != NULL) {
        setSheduleParam.startMinute = cJSON_GetNumberValue(objPara);
    } else {
        return ret;
    }

    if ((objPara = cJSON_GetObjectItem(objParas, "DurationMinutes")) != NULL) {
        setSheduleParam.duration = cJSON_GetNumberValue(objPara);
    } else {
        return ret;
    }
printf("zhangyuhang  debug for DealSetShedule %d\n", __LINE__);
    if ((objPara = cJSON_GetObjectItem(objParas, "ScheduleID")) != NULL) {
        strcpy(setSheduleParam.scheduleID, cJSON_GetStringValue(objPara));
    } else {
        return ret;
    }
printf("zhangyuhang  debug for DealSetShedule %d\n", __LINE__);
    if ((objPara = cJSON_GetObjectItem(objParas, "Option")) != NULL) {
        setSheduleParam.option = *(cJSON_GetStringValue(objPara));
    } else {
        return ret;
    }


    if ((objPara = cJSON_GetObjectItem(objParas, "Command")) != NULL) {
        cJSON *cmd;
        if ((cmd = cJSON_GetObjectItem(objPara, "GoTOClean")) != NULL) {
            if (0 == strcmp(cJSON_GetStringValue(cmd), "ON")) {
                setSheduleParam.shedulecmd.cmd = CN_BOARD_SWITCH_ON;
            }
            else {
                setSheduleParam.shedulecmd.cmd = CN_BOARD_SWITCH_OFF;
            }
        }
    }

    ret = IotProfile_CommandCallback(CLOUD_COMMAND_SETSHEDULE, &setSheduleParam);

    return ret;
}



int CLOUD_CommandCallBack(const char *jsonString)
{
    cJSON *objRoot = NULL;
    cJSON *objCmdName = NULL;
    int ret = -1;

    printf("jsonString is %s\r\n",jsonString);
    if (jsonString == NULL) {
        return ret;
    }

    if ((objRoot = cJSON_Parse(jsonString)) == NULL) {
        RaiseLog(LOG_LEVEL_ERR,"could not parse the payload\r\n");
        goto EXIT_JSONPARSE;
    }

    if ((objCmdName = cJSON_GetObjectItem(objRoot, "command_name")) == NULL) {
        RaiseLog(LOG_LEVEL_ERR,"%s:could not get the cmd name from json\r\n");
        goto EXIT_CMDOBJ;
    }

    if (0 == strcmp(cJSON_GetStringValue(objCmdName), "CleanCmd")) {
        printf("-----------------jsonString\r\n");
        ret = DealSetMainLed(objRoot);
    } else if(0 == strcmp(cJSON_GetStringValue(objCmdName), "SetLampDutyCycle")) {
        ret = DealSetAuxiliaryLed(objRoot);
    } else if(0 == strcmp(cJSON_GetStringValue(objCmdName), "SetSchedule")) {
        ret = DealSetShedule(objRoot);
    }else {
        RaiseLog(LOG_LEVEL_ERR,"unresolved command:%d\r\n", cJSON_GetStringValue(objCmdName));
    }

EXIT_CMDOBJ:
EXIT_JSONPARSE:
    if (objRoot != NULL) {
        cJSON_Delete(objRoot);
    }
    return ret;
}




