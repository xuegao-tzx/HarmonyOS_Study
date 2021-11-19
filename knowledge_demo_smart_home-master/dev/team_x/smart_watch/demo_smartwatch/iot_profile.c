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
#include "kv_store.h"


#define    PHONE_BOOK_FARTHER        "farther"
#define    PHONE_BOOK_MOTHER        "mother"
#define    PHONE_NUMBER_LEN         11
#define    KV_BUFF_MAX_LEN               32


struct{
    char fatherPhoneNumber[PHONE_NUMBER_LEN+1];
    char motherPhoneNumber[PHONE_NUMBER_LEN+1];
}g_phoneBook;

int Iot_getStartTime(void)
{
    return IOT_ScheduleGetStartTime();
}

void IotProfile_init(void)
{
    IotPhoneNumber_get("mother", g_phoneBook.motherPhoneNumber, KV_BUFF_MAX_LEN);
    IotPhoneNumber_get("father", g_phoneBook.fatherPhoneNumber, KV_BUFF_MAX_LEN);
}

char *IotProfile_GetFatherPhoneNumber(void)
{
    if (strlen(g_phoneBook.fatherPhoneNumber) == PHONE_NUMBER_LEN) {
        return &g_phoneBook.fatherPhoneNumber;
    } else {
        return NULL;
    }
}

char *IotProfile_GetMohterPhoneNumber(void)
{
    if (strlen(g_phoneBook.motherPhoneNumber) == PHONE_NUMBER_LEN) {
        return &g_phoneBook.motherPhoneNumber;
    } else {
        return NULL;
    }
}


static void IotPhoneNumber_set(const char *name, const char *phoneNumber)
{
    if ( !(strncmp(name, PHONE_BOOK_FARTHER, strlen(PHONE_BOOK_FARTHER)) != 0  ||\
        strncmp(name, PHONE_BOOK_MOTHER, strlen(PHONE_BOOK_MOTHER) ) != 0 ) ) {
        printf("IotPhoneNumber_get param error!!\n");
        return;
    }
    if (name == NULL || phoneNumber == NULL) {
        return;
    }

    int ret = UtilsSetValue(name, phoneNumber);
    if (ret != 0) {
        printf("ret = %d\n", ret);
        RaiseLog(LOG_LEVEL_ERR, "store phoneNumber failed! \n");
        return;
    }

    RaiseLog(LOG_LEVEL_INFO, "store phoneNumber success! \n");
}


int IotPhoneNumber_get(char *name, char *phoneNumber, int size)
{
    char phone_Number[24];

    if ( !(strncmp(name, PHONE_BOOK_FARTHER, strlen(PHONE_BOOK_FARTHER)) != 0  ||\
         strncmp(name, PHONE_BOOK_MOTHER, strlen(PHONE_BOOK_MOTHER)) != 0 ) ) {
        printf("IotPhoneNumber_get param error!!\n");
        return;
    }

    if (name == NULL || phoneNumber == NULL) {
        return;
    }

    int retval = UtilsGetValue(name, phoneNumber, size);
    if (retval <= 0) {
        RaiseLog(LOG_LEVEL_ERR, "no such ssid stored! \n");
        printf("retval =%d\n", retval);
        return 0;
    }

    printf("phoneNumber =%s\n", phoneNumber);
    return 1;
}

static int IotPhoneNumber_del(const char *contacts)
{
    int retval = UtilsDeleteValue(contacts);
    if (retval <= 0) {
        RaiseLog(LOG_LEVEL_ERR, "no such ssid stored! \n");
        return 0;
    }
    return 1;
}

int Iot_getTotalPhoneNumber(void)
{
    int TotalPhoneNumber = 0;

    if (strlen(g_phoneBook.fatherPhoneNumber) == PHONE_NUMBER_LEN) {
        TotalPhoneNumber += 1;
    }

    if (strlen(g_phoneBook.motherPhoneNumber) == PHONE_NUMBER_LEN) {
        TotalPhoneNumber += 1;
    }
    return TotalPhoneNumber;
}


/**
 * @brief deal the message received from the queue
 *
 */
static int DealDeletePhoneNumber(cJSON *objCmd)
{
    int ret = -1;
    cJSON *objParas = NULL;
    cJSON *objParaPhoneNumber = NULL;
    cJSON *objParaName = NULL;

    if ((objParas = cJSON_GetObjectItem(objCmd, "paras")) == NULL) {
        RaiseLog(LOG_LEVEL_ERR, "Could not get paras in json");
        return ret;
    }
    
    objParaName = cJSON_GetObjectItem(objParas, "name");

    if (objParaName != NULL) {      
        char *lName = cJSON_GetStringValue(objParaName);

        if (lName != NULL) {
            printf("phone lName = %s\n", lName);

            IotPhoneNumber_del(lName);

            if (strncmp(lName, "father", strlen("father"))==0) {
                memset_s(g_phoneBook.fatherPhoneNumber, PHONE_NUMBER_LEN + 1,0, PHONE_NUMBER_LEN + 1);
            } else if (strncmp(lName, "mother", strlen("mother")) == 0) {
                memset_s(g_phoneBook.motherPhoneNumber, PHONE_NUMBER_LEN + 1, 0,PHONE_NUMBER_LEN + 1);
            }
            ret = 1;
        }
    } else {
        RaiseLog(LOG_LEVEL_ERR, "Para not exist");
    }
    return ret;
}



/**
 * @brief we make the data to a json string and send it to the iot cloud engine
 *
 */
int IotProfile_Report(const char *utcDateTime, const char *logitude , const char *latitude, const char *satellitesinview)
{

    int ret = -1;

    cJSON *root;
    char *jsonString;
    IotProfileService    service;
    IotProfileKV  kvUtcDateTime;
    IotProfileKV  kvLatitude;
    IotProfileKV  kvLogitude;
    IotProfileKV  kvSatellitesinview;
    IotProfileKV  kvFatherPhoneNumber;
    IotProfileKV  kvMohterPhoneNumber;

    /* package the data */
    service.eventTime = NULL;
    service.serviceID = "SmartWatch";
    service.propertyLst = &kvUtcDateTime;
    service.nxt = NULL;


    kvUtcDateTime.key = "UTCdatetime";
    if (utcDateTime == NULL) {
        kvUtcDateTime.value = "";
    } else {
        kvUtcDateTime.value = utcDateTime;
    }
    kvUtcDateTime.type = IOT_PROFILE_KEY_DATATYPE_STRING;
    kvUtcDateTime.nxt = &kvLogitude;

    kvLogitude.key = "logitude";
    if (logitude == NULL) {
        kvLogitude.value = "";
    } else {
        kvLogitude.value = logitude;
    }
    kvLogitude.type = IOT_PROFILE_KEY_DATATYPE_STRING;
    kvLogitude.nxt = &kvLatitude;

    kvLatitude.key = "latitude";
    if (latitude == NULL) {
        kvLatitude.value = "";
    } else {
        kvLatitude.value = latitude;
    }
    kvLatitude.value = latitude;
    kvLatitude.type = IOT_PROFILE_KEY_DATATYPE_STRING;
    kvLatitude.nxt = &kvSatellitesinview;

    kvSatellitesinview.key = "satellitesinview";
    if (satellitesinview == NULL) {
        kvSatellitesinview.value = "";
    } else {
        kvSatellitesinview.value = satellitesinview;
    }
    kvSatellitesinview.type = IOT_PROFILE_KEY_DATATYPE_INT;
    kvSatellitesinview.nxt = &kvFatherPhoneNumber;


    kvFatherPhoneNumber.key = "father";
    if(strlen(g_phoneBook.fatherPhoneNumber) == PHONE_NUMBER_LEN){
    }else{
        kvFatherPhoneNumber.value = "";
    }
    kvFatherPhoneNumber.type = IOT_PROFILE_KEY_DATATYPE_STRING;
    kvFatherPhoneNumber.nxt = &kvMohterPhoneNumber;
    kvFatherPhoneNumber.value = g_phoneBook.fatherPhoneNumber;

    
    kvMohterPhoneNumber.key = "mother";
    if (strlen(g_phoneBook.motherPhoneNumber) == PHONE_NUMBER_LEN) { 
        kvMohterPhoneNumber.value = g_phoneBook.motherPhoneNumber;
    } else {
        kvMohterPhoneNumber.value = "";
    }
    kvMohterPhoneNumber.type = IOT_PROFILE_KEY_DATATYPE_STRING;
    kvMohterPhoneNumber.nxt = NULL;


    jsonString = IoTProfilePackage(&service);
    if ( NULL != jsonString) {
        RaiseLog(LOG_LEVEL_INFO,"jsonString:%s", jsonString);

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
static int DealAddPhoneNumber(cJSON *objCmd)
{
    int ret = -1;
    cJSON *objParas = NULL;
    cJSON *objParaPhoneNumber = NULL;
    cJSON *objParaName = NULL;

    if ((objParas = cJSON_GetObjectItem(objCmd, "paras")) == NULL) {
        RaiseLog(LOG_LEVEL_ERR, "Could not get paras in json");
        return ret;
    }
    objParaName = cJSON_GetObjectItem(objParas, "name");
    objParaPhoneNumber = cJSON_GetObjectItem(objParas, "PhoneNumber");

    if (objParaName != NULL && objParaPhoneNumber != NULL) {      
        char *lPhoneNumber = cJSON_GetStringValue(objParaPhoneNumber);
        char *lName = cJSON_GetStringValue(objParaName);

        if (lPhoneNumber != NULL && lName != NULL) {
            printf("phone number = %s\n", lPhoneNumber);

            if (PHONE_NUMBER_LEN == strlen(lPhoneNumber)) {
                IotPhoneNumber_set(lName,lPhoneNumber);
                if (strncmp(lName, "father", strlen("father"))==0) {
                    strncpy(g_phoneBook.fatherPhoneNumber, lPhoneNumber,PHONE_NUMBER_LEN);
                } else if (strncmp(lName,"mother",strlen("mother")) == 0) {
                    strncpy(g_phoneBook.motherPhoneNumber, lPhoneNumber,PHONE_NUMBER_LEN);
                }
                printf("g_phoneBook.fatherPhoneNumber=%s\n", g_phoneBook.fatherPhoneNumber);
            }else {
                return ret;
            }
        }
        ret = 1;
    } else {
        RaiseLog(LOG_LEVEL_ERR, "Para not exist");
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
    int ret = -1;
    cJSON *objParaCommand = NULL;
    cJSON *objParas = NULL;
    cJSON *objPara = NULL;
    CommandParamSetShedule setSheduleParam;
    memset(&setSheduleParam, 0x00, sizeof(CommandParamSetShedule));

    if ((objParas = cJSON_GetObjectItem(objCmd, "paras")) == NULL) {
        RaiseLog(LOG_LEVEL_ERR, "Paras not exist");
        return ret;
    }

    if ((objPara = cJSON_GetObjectItem(objParas, "Day")) != NULL) {
        char *weekdays = cJSON_GetStringValue(objPara);
        setSheduleParam.num = GetScheduleWeekDays(weekdays, setSheduleParam.day, CN_MAX_WEEKDAY);
    } else {
        printf("exit at Day\n");
        return ret;
    }

    if ((objPara = cJSON_GetObjectItem(objParas, "StartHour")) != NULL) {
        setSheduleParam.startHour = cJSON_GetNumberValue(objPara);
    } else {
        printf("exit at StartHour\n");
        return ret;
    }

    if ((objPara = cJSON_GetObjectItem(objParas, "StartMinute")) != NULL) {
        setSheduleParam.startMinute = cJSON_GetNumberValue(objPara);
    } else {
        printf("exit at StartMinute\n");
        return ret;
    }

    if ((objPara = cJSON_GetObjectItem(objParas, "DurationMinutes")) != NULL) {
        setSheduleParam.duration = cJSON_GetNumberValue(objPara);
    } else {
        return ret;
    }

    if ((objPara = cJSON_GetObjectItem(objParas, "ScheduleID")) != NULL) {
        strcpy(setSheduleParam.scheduleID, cJSON_GetStringValue(objPara));
    } else {
        printf("exit at ScheduleID\n");
        return ret;
    }

    if ((objPara = cJSON_GetObjectItem(objParas, "Option")) != NULL) {
        setSheduleParam.option = *(cJSON_GetStringValue(objPara));
    } else {
         printf("exit at Option\n");
        return ret;
    }

    if ((objPara = cJSON_GetObjectItem(objParas, "Command")) != NULL) {
        if ((objParaCommand = cJSON_GetObjectItem(objPara, "name")) != NULL) {
            char *message = cJSON_GetStringValue(objParaCommand);

            if (strncmp(message, "起床", strlen("起床")) == 0) {
                printf("DealSetShedule 1111\n");
                setSheduleParam.showMsgInfo.msgType = 2;
            } else if (strncmp(message, "睡觉", strlen("睡觉")) == 0) {
                setSheduleParam.showMsgInfo.msgType = 3;
            } else if (strncmp(message, "写作业", strlen("写作业")) == 0) {
                setSheduleParam.showMsgInfo.msgType = 4;
            } else if (strncmp(message, "喝水", strlen("喝水")) == 0) {
                setSheduleParam.showMsgInfo.msgType = 5;
            } else if (strncmp(message, "吃药", strlen("吃药")) == 0) {
                setSheduleParam.showMsgInfo.msgType = 6;
            } else {
                setSheduleParam.showMsgInfo.msgType = 1;
            }
        } else {
            printf("exit at Command -- MessageType\n");
            return ret;
        }
    } else {
        printf("exit at Command\n");
        return ret;
    }

    ret = IotProfile_CommandCallback(CLOUD_COMMAND_SETSHEDULE, &setSheduleParam);

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

    char *retChar = cJSON_GetStringValue(objCmdName);

    printf("CLOUD_CommandCallBackcmd =%s.\n", retChar);

    if (0 == strcmp(cJSON_GetStringValue(objCmdName), "AddPhoneNumber")) {
        ret = DealAddPhoneNumber(objRoot);
    } else if (0 == strcmp(cJSON_GetStringValue(objCmdName), "DeletePhoneNumber")) {
        ret = DealDeletePhoneNumber(objRoot);
    } else if (0 == strcmp(cJSON_GetStringValue(objCmdName), "SetSchedule")) {
         ret = DealSetShedule(objRoot);
    } else {
        RaiseLog(LOG_LEVEL_ERR,"unresolved command:%d\r\n", cJSON_GetStringValue(objCmdName));
    }

EXIT_CMDOBJ:
EXIT_JSONPARSE:
    if (objRoot != NULL) {
        cJSON_Delete(objRoot);
    }
    return ret;
}




