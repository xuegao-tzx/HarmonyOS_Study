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

#ifndef __APP_DEMO_UART_H__
#define __APP_DEMO_UART_H__

#include <hi_types_base.h>
#include <hi_early_debug.h>
#include <hi_task.h>
#include <hi_uart.h>
#include <stdbool.h>
#include "cmsis_os2.h"

#define WRITE_BY_INT
#define UART_DEMO_TASK_STAK_SIZE 1024*6
#define UART_DEMO_TASK_PRIORITY  25
#define DEMO_UART_NUM            HI_UART_IDX_1
#define UART_BUFF_SIZE           256

typedef enum{
    BUTTON_OFF,     /* 按键状态：关 */
    BUTTON_ON       /* 按键状态：开 */
}BUTTON_STS;

enum{
    GSM_NULL = 0,
    GSM_RING,
    GSM_HANG_UP
};
enum{
    E_PHONEBOOK_FATHER = 0,
    E_PHONEBOOK_MOTHER
};

typedef struct 
{
    char UTCdatetime[19]; /* UTC date & Time */
    char latitude[11]; /* Latitude */
    char logitude[12]; /* Longitude */
    char satellitesinview[3]; /* GPS Satellites in View */
}GGPS_INFO;

typedef struct 
{
    char GNSSrunstatus[2]; /* GNSS run status:0 = GNSS off and 1 = GNSS on */
    char Fixstatus[2]; /* Fix status:0 = Not fixed positon and 1 = Fixed positon */
    char UTCdatetime[19]; /* UTC date & Time */
    char latitude[11]; /* Latitude */
    char logitude[12]; /* Longitude */
    char altitude[9]; /* MSL Altitude */
    char speedOTG[7]; /* Speed Over Ground */
    char course[7]; /* Course Over Ground */
    char fixmode[2]; /* Fix Mode */
    char Reserved1[2]; /* Reserved1 */
    char HDOP[5]; /* HDOP */
    char PDOP[5]; /* PDOP */
    char VDOP[5]; /* VDOP */
    char Reserved2[2]; /* Reserved2 */
    char satellitesinview[3]; /* GPS Satellites in View */
    char GNSSsatellitesused[3]; /* GNSS Satellites Used */
    char GLONASSsatellitesused[3]; /* GLONASS Satellites in View */
    char Reserved3[2]; /* Reserved3 */
    char CN0max[3]; /* C/N0 max */
    char HPA[7]; /* HPA */
    char VPA[7]; /* VPA */
}GGPS_DATA;

bool GsmGetRingSts(void);
bool GsmGetHungUpSts(void);
void GsmSetHungUpSts(bool value);
bool GsmGetConnectSts(void);
void GsmSetConnectSts(bool value);
bool GsmGetDailSts(void);
void GsmSetDailSts(bool value);
void GsmSetPhoneBookItem(int item);
int GsmGetPhoneBookItem(void);

void uart_task_create(void);

#endif