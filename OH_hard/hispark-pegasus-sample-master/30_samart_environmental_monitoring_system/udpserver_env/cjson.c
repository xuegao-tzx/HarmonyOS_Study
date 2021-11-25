/*
 * Copyright (c) 2020 HiHope Community.
 * Description: cJSON init 
 * Author: HiSpark Product Team
 * Create: 2020-5-20
 */

#include <stdint.h>
#include <cJSON.h>
#include <stdlib.h>
#include <hi_mem.h>
#include "config_params.h"
#include "wifi_device_config.h"
#include "kv_store.h"
#include "reset.h"
#include "stdbool.h"
#include "cmsis_os2.h"
#include "ohos_init.h"

#define SYS_REBOOT_CAUSE_USR_NORMAL_REBOOT 5

extern float g_humidity;
extern float g_temperature;
extern float g_gasValuetemp;
extern int sendMessage;

void DoReboot(void* arg)
{
    (void)arg;
    osDelay(200);
    printf("start Reboot device\n\r");
    RebootDevice(SYS_REBOOT_CAUSE_USR_NORMAL_REBOOT);
}

static void Reboot(void)
{
    osThreadAttr_t attr = {0};

    attr.name = "DoReboot";
    attr.stack_size = 4096;
    attr.priority = osPriorityNormal;

    if (osThreadNew(DoReboot, NULL, &attr) == NULL) {
        printf("[Reboot] create DoReboot failed!\n");
    }
}

char *cJSONReport(void) {
    cJSON* cjson_test = NULL;
    cJSON* cjson_temp = NULL;
    cJSON* cjson_humi = NULL;
    cJSON* cjson_gas = NULL;
    char* str = NULL;

    /* 创建一个JSON数据对象(链表头结点) */
    cjson_test = cJSON_CreateObject();

    /* 添加一条浮点类型的JSON数据(添加一个链表节点) */
    cJSON_AddNumberToObject(cjson_test, "temp", g_temperature);
    cJSON_AddNumberToObject(cjson_test, "humi", g_humidity);
    cJSON_AddNumberToObject(cjson_test, "gas", g_gasValuetemp);

    str = cJSON_Print(cjson_test);
    printf("%s\n", str);
    return str;
}

int cJSONParseAP(char *message) {
    cJSON* cjson_test = NULL;
    cJSON* cjson_ssid = NULL;
    cJSON* cjson_psk = NULL;

    cjson_test = cJSON_Parse(message);
    if(cjson_test == NULL)
    {
        printf("parse fail.\n");
        return -1;
    }

    /* 依次根据名称提取JSON数据（键值对） */
    cjson_ssid = cJSON_GetObjectItem(cjson_test, "hotspot_ssid");
    cjson_psk = cJSON_GetObjectItem(cjson_test, "hotspot_psk");

    printf("ssid: %s\n", cjson_ssid->valuestring);
    printf("psk:%s\n", cjson_psk->valuestring);

    int ret = UtilsSetValue(PARAM_HOTSPOT_SSID_KEY, cjson_ssid->valuestring);
    if (ret < 0) {
        printf("SSID value saved filed, ret is %d\r\n", ret);
        
    }
    ret = UtilsSetValue(PARAM_HOTSPOT_PSK_KEY, cjson_psk->valuestring);
    if (ret < 0) {
        printf("KEY value saved filed, ret is %d\r\n", ret);
    }

    cJSON_Delete(cjson_test);

    if (CheckKvStoreResult() == true) {
        Reboot();
    }

    return ret;
}

int cJSONParseSTA(char *message) {
    cJSON* cjson_test_sta = NULL;
    cJSON* cjson_temp_max = NULL;
    cJSON* cjson_temp_min = NULL;
    cJSON* cjson_humi_max = NULL;
    cJSON* cjson_humi_min = NULL;
    cJSON* cjson_gas_value = NULL;
    cJSON* cjson_update = NULL;

    cjson_test_sta = cJSON_Parse(message);
    if(cjson_test_sta == NULL)
    {
        printf("parse fail.\n");
        return -1;
    }

    /* 依次根据名称提取JSON数据（键值对） */
    cjson_temp_max = cJSON_GetObjectItem(cjson_test_sta, "temp_max");
    cjson_temp_min = cJSON_GetObjectItem(cjson_test_sta, "temp_min");
    cjson_humi_max = cJSON_GetObjectItem(cjson_test_sta, "humi_max");
    cjson_humi_min = cJSON_GetObjectItem(cjson_test_sta, "humi_min");
    cjson_gas_value = cJSON_GetObjectItem(cjson_test_sta, "gas_value");
    cjson_update = cJSON_GetObjectItem(cjson_test_sta, "update");

    
    int ret = 0;
    if (cjson_temp_max != NULL) {
        printf("temp_max: %s\n", cjson_temp_max->valuestring);
        ret = UtilsSetValue(PARAM_TEMP_MAX, cjson_temp_max->valuestring);
        if (ret < 0) {
            printf("temp_max value saved filed, ret is %d\r\n", ret);     
        }
    }
    if (cjson_temp_min != NULL) {
        printf("temp_min: %d\n", cjson_temp_min->valuestring);
        ret = UtilsSetValue(PARAM_TEMP_MIN, cjson_temp_min->valuestring);
        if (ret < 0) {
            printf("temp_min value saved filed, ret is %d\r\n", ret);
        }
    }
    if (cjson_humi_max != NULL) {
        printf("humi_max: %s\n", cjson_humi_max->valuestring);
        ret += UtilsSetValue(PARAM_HUMI_MAX, cjson_humi_max->valuestring);
        if (ret < 0) {
            printf("humi_max value saved filed, ret is %d\r\n", ret);
        }
    }
    if (cjson_humi_min != NULL) {
        printf("humi_min: %s\n", cjson_humi_min->valuestring);
        ret = UtilsSetValue(PARAM_HUMI_MIN, cjson_humi_min->valuestring);
        if (ret < 0) {
            printf("humi_min value saved filed, ret is %d\r\n", ret);
        }
    }
    if (cjson_gas_value != NULL) {
        printf("gas_value: %s\n", cjson_gas_value->valuestring);
        ret = UtilsSetValue(PARAM_GAS_VALUE, cjson_gas_value->valuestring);
        if (ret < 0) {
            printf("gas_value value saved filed, ret is %d\r\n", ret);
        }
    }
    if (cjson_update != NULL) {
        printf("update: %s\n", cjson_update->valueint);
        sendMessage = 1;
    }

    printf("parse mesgsta success\n");

    cJSON_Delete(cjson_test_sta);

    return ret;
}
