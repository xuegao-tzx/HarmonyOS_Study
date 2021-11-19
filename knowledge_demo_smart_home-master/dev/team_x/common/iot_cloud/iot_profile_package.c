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
#include "cJSON.h"

#define IOT_PROFILE_KEY_SERVICES         "services"
#define IOT_PROFILE_KEY_SERVICEID        "service_id"
#define IOT_PROFILE_KEY_PROPERTIES       "properties"
#define IOT_PROFILE_KEY_EVENTTIME        "event_time"
#define IOT_PROFILE_KEY_SERVICES         "services"

// format the report data to json string mode
static cJSON  *FormatValue(IotProfileKV  *kv)
{
    cJSON  *ret = NULL;
    switch (kv->type)
    {
        case IOT_PROFILE_KEY_DATATYPE_INT:
            ret = cJSON_CreateNumber((double)(*(int *)kv->value));
            break;
        case IOT_PROFILE_KEY_DATATYPE_LONG:
            ret = cJSON_CreateNumber((double)(*(long *)kv->value));
            break;
        case IOT_PROFILE_KEY_DATATYPE_FLOAT:
            ret = cJSON_CreateNumber((double)(*(float *)kv->value));
            break;
        case IOT_PROFILE_KEY_DATATYPE_DOUBLE:
            ret = cJSON_CreateNumber((*(double *)kv->value));
            break;
        case IOT_PROFILE_KEY_DATATYPE_STRING:
            ret = cJSON_CreateString((const char *)kv->value);
            break;
        default:
            break;
    }

    return ret;
}


static cJSON *FormatProperty(IotProfileKV *propertyLst)
{
    cJSON *rootObj = NULL;
    cJSON *propertyObj = NULL;
    IotProfileKV *property = NULL;
    // build a root node
    rootObj = cJSON_CreateObject();
    if (rootObj == NULL) {
        goto EXIT_MEM;
    }
    // add all the property to the properties
    property = propertyLst;
    while (NULL != property) {
        propertyObj = FormatValue(property);
        if (propertyObj == NULL) {
            goto EXIT_MEM;
        }

        cJSON_AddItemToObject(rootObj, property->key, propertyObj);
        property = property->nxt;
    }

    // OK, now we return it
    return rootObj;

EXIT_MEM:
    if (NULL != rootObj) {
        cJSON_Delete(rootObj);
        rootObj = NULL;
    }
    return rootObj;

}

static cJSON *FormatService(IotProfileService *service)
{
    cJSON *rootObj;
    cJSON *serviceID;
    cJSON *propertyObj;
    cJSON *eventTimeObj;

    // build a rootObj node
    rootObj = cJSON_CreateObject();
    if (NULL == rootObj) {
        goto EXIT_MEM;
    }
    // add the serviceID node to the rootObj node
    serviceID = cJSON_CreateString(service->serviceID);
    if (NULL == serviceID) {
        goto EXIT_MEM;
    }
    cJSON_AddItemToObjectCS(rootObj, IOT_PROFILE_KEY_SERVICEID, serviceID);

    // add the propertyObj node to the rootObj
    propertyObj = FormatProperty(service->propertyLst);
    if (NULL == propertyObj) {
        goto EXIT_MEM;
    }
    cJSON_AddItemToObjectCS(rootObj, IOT_PROFILE_KEY_PROPERTIES, propertyObj);
    // add the event time (optional) to the rootObj
    if (NULL != service->eventTime) {
        eventTimeObj = cJSON_CreateString(service->eventTime);
        if (NULL == eventTimeObj) {
            goto EXIT_MEM;
        }
        cJSON_AddItemToObjectCS(rootObj, IOT_PROFILE_KEY_EVENTTIME, eventTimeObj);
    }
    // OK, now we return it
    return rootObj;

EXIT_MEM:
    if (NULL != rootObj) {
        cJSON_Delete(rootObj);
        rootObj = NULL;
    }
    return rootObj;
}

static cJSON *FormatServices(IotProfileService *serviceLst)
{
    cJSON *rootObj = NULL;
    cJSON * serviceObj;
    IotProfileService *service;

    // create the rootObj array node
    rootObj = cJSON_CreateArray();
    if (NULL == rootObj) {
        goto EXIT_MEM;
    }
    service = serviceLst;
    while (NULL != service) {
        serviceObj = FormatService(service);
        if (NULL == serviceObj) {
            goto EXIT_MEM;
        }

        cJSON_AddItemToArray(rootObj, serviceObj);
        service = service->nxt;
    }

    // now we return the rootObj
    return rootObj;

EXIT_MEM:
    if (NULL != rootObj) {
        cJSON_Delete(rootObj);
        rootObj = NULL;
    }
    return rootObj;
}

char *IoTProfilePackage(IotProfileService *serviceLst)
{
    char *ret = NULL;
    cJSON *rootObj;
    cJSON *servicesObj;

    // create the rootObj node
    rootObj = cJSON_CreateObject();
    if (NULL == rootObj) {
        goto EXIT_MEM;
    }
    // create the services array node to the rootObj
    servicesObj = FormatServices(serviceLst);
    if (NULL == servicesObj) {
        goto EXIT_MEM;
    }
    cJSON_AddItemToObjectCS(rootObj, IOT_PROFILE_KEY_SERVICES, servicesObj);
    // OK, now we make it to a buffer
    ret = cJSON_PrintUnformatted(rootObj);
    cJSON_Delete(rootObj);
    return ret;

EXIT_MEM:
    if (NULL != rootObj) {
        cJSON_Delete(rootObj);
    }
    return ret;
}


static cJSON *NotifyGetMsgObj(const char *enString, const char *chString)
{
    cJSON *objRoot;
    cJSON *objEn, *objCh;
    char *ret = NULL;

    objRoot = cJSON_CreateObject();
    if (objRoot == NULL) {
        printf("[%s|%d][ERROR] cJSON_CreateObject \n", __func__,__LINE__);
        return NULL;
    }

    objEn = cJSON_CreateString(enString);
    if (objEn == NULL) {
        printf("[%s|%d][ERROR] cJSON_CreateString %s\n", __func__,__LINE__,enString);
        goto MSG_ERR;
    }
    cJSON_AddItemToObjectCS(objRoot, "msg_en", objEn);

    objCh = cJSON_CreateString(chString);
    if (objCh == NULL) {
        printf("[%s|%d][ERROR] cJSON_CreateString %s\n", __func__,__LINE__,chString);
        goto MSG_ERR;
    }
    cJSON_AddItemToObjectCS(objRoot, "msg_zh", objCh);

    return objRoot;
MSG_ERR:
    if (objRoot) {
        cJSON_Delete(objRoot);
    }
    return NULL;
}

static cJSON *NotifyGetParamObj(int type, const char *enString, const char *chString)
{
    cJSON *objRoot;
    cJSON *objtype;
    cJSON *objmsg;

    objRoot = cJSON_CreateObject();
    if (objRoot == NULL) {
        printf("[%s|%d][ERROR] cJSON_CreateObject \n", __func__,__LINE__);
        return NULL;
    }

    objtype = cJSON_CreateNumber(type);
    if (objtype == NULL) {
        printf("[%s|%d][ERROR] cJSON_CreateNumber \n", __func__,__LINE__);
        goto PARAM_ERR;
    }
    cJSON_AddItemToObjectCS(objRoot, "AlarmType", objtype);

    objmsg = NotifyGetMsgObj(enString, chString);
    if (objmsg == NULL) {
        goto PARAM_ERR;
        printf("[%s|%d][ERROR] NotifyGetMsgObj \n", __func__,__LINE__);
    }
    cJSON_AddItemToObjectCS(objRoot, "ALarmMsg", objmsg);

    return objRoot;
PARAM_ERR:
    if (objRoot) {
        cJSON_Delete(objRoot);
    }
    return NULL;
}
char *IotNotificationPackage(int type, const char *enString, const char *chString)
{
    char *ret = NULL;
    cJSON *rootObj;
    cJSON *serviceidObjs;
    cJSON *propertiesObj;
    cJSON *objtype;
    cJSON *objmsg;
    cJSON *objEn, *objCh;

    // create the rootObj node
    rootObj = cJSON_CreateObject();
    if (NULL == rootObj) {
        printf("[%s|%d][ERROR] cJSON_CreateObject \n", __func__,__LINE__);
        goto EXIT_MEM;
    }
    // create the services array node to the rootObj
    serviceidObjs = cJSON_CreateString("DeviceAlarm");
    if (serviceidObjs == NULL) {
        printf("[%s|%d][ERROR] cJSON_CreateString DeviceAlarm \n", __func__,__LINE__);
        goto EXIT_MEM;
    }
    cJSON_AddItemToObjectCS(rootObj, "serviceId", serviceidObjs);

    propertiesObj = NotifyGetParamObj(type, enString, chString);
    if (propertiesObj == NULL) {
        printf("[%s|%d][ERROR] cJSON_CreateObject \n", __func__,__LINE__);
        goto EXIT_MEM;
    }
    cJSON_AddItemToObjectCS(rootObj, "properties", propertiesObj);

    // OK, now we make it to a buffer
    ret = cJSON_PrintUnformatted(rootObj);
    cJSON_Delete(rootObj);
    printf("ret = %s \n", ret);
    return ret;
EXIT_MEM:
    if (NULL != rootObj) {
        cJSON_Delete(rootObj);
    }
    return NULL;
}
