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

#include "app_gsm.h"
#include <stdio.h>
#include "app_button.h"
#include "iot_profile.h"
#include "app_phonePage.h"


#define PHONE_NUMB_LEN  11
#define GNSINF_MSG_MAX_LEN  148
#define UART_WAIT_COUNT_MAX   30
#define CONFIG_TASK_UART_PRIOR           20                        /* default task priority */
#define UART_BAUD_RATE                   115200
#define UART_DATA_BITS                   8
#define UART_STOP_BITS                   1

uint32_t g_uart_demo_task_id = 0;
uint8_t g_uart_buff[UART_BUFF_SIZE] = {0};
uint32_t g_ReceivedDatalen = 0;

static uint8_t g_isGpsPowerOn = false;
static uint8_t g_phoneBookSelect;

typedef struct
{
    bool isRing;
    bool isHungUp;
    bool connectSts;
    bool isDailSts;
} GSMController;

typedef struct
{
    bool isReadBusy;
} UartController;

static GSMController g_gsmController;
static UartController g_uartController;

struct
{
    uint8_t lastKeySts;
    uint8_t lastKeyLongPressSts;
} keyLastSts;


const uint32_t MAX_MSG_LENGTH = 256;


uint8_t g_isInitSim800 = 0;

static GGPS_DATA g_gpsData;

extern int GetMessageShowSts(void);


void GsmContolInit(void)
{
    GsmSetRingSts(false);
    GsmSetHungUpSts(false);
    GsmSetConnectSts(false);
    GsmSetDailSts(false);
    keyLastSts.lastKeySts = false;
    g_phoneBookSelect = 0;
}

bool GsmGetRingSts(void)
{
    return g_gsmController.isRing;
}
void GsmSetRingSts(bool value)
{
    g_gsmController.isRing = value;
}

bool GsmGetHungUpSts(void)
{
    return g_gsmController.isHungUp;
}

void GsmSetHungUpSts(bool value)
{
    if(value != true && value != false){
        return;
    }
    g_gsmController.isHungUp = value;
}

bool GsmGetConnectSts(void)
{
    return g_gsmController.connectSts;
}
void GsmSetConnectSts(bool value)
{
    g_gsmController.connectSts = value;
}

bool GsmGetDailSts(void)
{
    return g_gsmController.isDailSts;
}

void GsmSetDailSts(bool value)
{
    g_gsmController.isDailSts = value;
}

void GsmSetPhoneBookItem(int item)
{
    if(g_phoneBookSelect < 0 || g_phoneBookSelect > 1)
    {
        printf("appSetPhoneBookItem error !!\n");
        return;
    }
    g_phoneBookSelect = item;
}

int GsmGetPhoneBookItem(void)
{
    if(g_phoneBookSelect >= 0 && g_phoneBookSelect <= 1)
    {
        return g_phoneBookSelect;
    }
}


/* input:AT+CGNSINF Command Response
* output:struct GGPS_DATA
*/
static void GPS_CGNSINF_Analyze(char *origin, GGPS_DATA *gpsdata)
{
    int counter = 0;
    char tmp[150] = {0};
    char *lptr = NULL;
    char *localptr = NULL;

    lptr = (char *)strstr(origin, "+CGNSINF");
    if (lptr == NULL)
    {
        return;
    } else {
        lptr += 10;
    }
        

    while (*lptr != '\0')
    {
        if (*lptr == ',' && *(lptr + 1) == ',')
        {
            tmp[counter] = *lptr;
            counter++;
            tmp[counter] = '0';
        } else if (*lptr == '\r' && *(lptr + 1) == '\n' && counter < 148)
        {
            tmp[counter] = '0';
            tmp[counter + 1] = ',';
            tmp[counter + 2] = 0;
            break;
        } else {
            tmp[counter] = *lptr;
        }
            

        lptr++;
        counter++;

        /* avoid array out of range */
        if (counter >= GNSINF_MSG_MAX_LEN){
            return;
        }
            
    }
    /* Clear struct data */
    memset(gpsdata, 0, sizeof( GGPS_DATA));

    localptr = (char *)strtok(tmp, ",");
    if (localptr == NULL)
    {
        return;
    }
    strncpy(gpsdata->GNSSrunstatus, localptr, sizeof(gpsdata->GNSSrunstatus));

    localptr = (char *)strtok(NULL, ",");
    if (localptr == NULL)
    {
        return;
    }
    strncpy(gpsdata->Fixstatus, localptr, sizeof(gpsdata->Fixstatus));

    localptr = (char *)strtok(NULL, ",");
    if (localptr == NULL)
    {
        return;
    }
    strncpy(gpsdata->UTCdatetime, localptr, sizeof(gpsdata->UTCdatetime));

    localptr = (char *)strtok(NULL, ",");
    if (localptr == NULL)
    {
        return;
    }
    strncpy(gpsdata->latitude, localptr, sizeof(gpsdata->latitude));

    localptr = (char *)strtok(NULL, ",");
    if (localptr == NULL)
    {
        return;
    }
    strncpy(gpsdata->logitude, localptr, sizeof(gpsdata->logitude));

    localptr = (char *)strtok(NULL, ",");
    if (localptr == NULL)
    {
        return;
    }
    strncpy(gpsdata->altitude, localptr, sizeof(gpsdata->altitude));

    localptr = (char *)strtok(NULL, ",");
    if (localptr == NULL)
    {
        return;
    }
    strncpy(gpsdata->speedOTG, localptr, sizeof(gpsdata->speedOTG));

    localptr = (char *)strtok(NULL, ",");
    if (localptr == NULL)
    {
        return;
    }
    strncpy(gpsdata->course, localptr, sizeof(gpsdata->course));

    localptr = (char *)strtok(NULL, ",");
    if (localptr == NULL)
    {
        return;
    }
    strncpy(gpsdata->fixmode, localptr, sizeof(gpsdata->fixmode));

    localptr = (char *)strtok(NULL, ",");
    if (localptr == NULL)
    {
        return;
    }
    strncpy(gpsdata->Reserved1, localptr, sizeof(gpsdata->Reserved1));

    localptr = (char *)strtok(NULL, ",");
    if (localptr == NULL)
    {
        return;
    }
    strncpy(gpsdata->HDOP, localptr, sizeof(gpsdata->HDOP));

    localptr = (char *)strtok(NULL, ",");
    if (localptr == NULL)
    {
        return;
    }
    strncpy(gpsdata->PDOP, localptr, sizeof(gpsdata->PDOP));

    localptr = (char *)strtok(NULL, ",");
    if (localptr == NULL)
    {
        return;
    }
    strncpy(gpsdata->VDOP, localptr, sizeof(gpsdata->VDOP));

    localptr = (char *)strtok(NULL, ",");
    if (localptr == NULL)
    {
        return;
    }
    strncpy(gpsdata->Reserved2, localptr, sizeof(gpsdata->Reserved2));

    localptr = (char *)strtok(NULL, ",");
    if (localptr == NULL)
    {
        return;
    }
    strncpy(gpsdata->satellitesinview, localptr, sizeof(gpsdata->satellitesinview));

    localptr = (char *)strtok(NULL, ",");
    if (localptr == NULL)
    {
        return;
    }
    strncpy(gpsdata->GNSSsatellitesused, localptr, sizeof(gpsdata->GNSSrunstatus));

    localptr = (char *)strtok(NULL, ",");
    if (localptr == NULL)
    {
        return;
    }
    strncpy(gpsdata->GLONASSsatellitesused, localptr, sizeof(gpsdata->GLONASSsatellitesused));

    localptr = (char *)strtok(NULL, ",");
    if (localptr == NULL)
    {
        return;
    }
    strncpy(gpsdata->Reserved3, localptr, sizeof(gpsdata->Reserved3));

    localptr = (char *)strtok(NULL, ",");
    if (localptr == NULL)
    {
        return;
    }
    strncpy(gpsdata->CN0max, localptr, sizeof(gpsdata->CN0max));

    localptr = (char *)strtok(NULL, ",");
    if (localptr == NULL)
    {
        return;
    }
    strncpy(gpsdata->HPA, localptr, sizeof(gpsdata->HPA));

    localptr = (char *)strtok(NULL, "\r\n");
    if (localptr == NULL)
    {
        return;
    }
    strncpy(gpsdata->VPA, localptr, sizeof(gpsdata->VPA));
}

static void GsmCheckRingAndHanupMessage(void)
{
    if (strstr(g_uart_buff, "RING") != NULL)
    {
        printf("ring.\n");
        if (GsmGetConnectSts() == false)
        {
            GsmSetRingSts(true);
        }
    }
    if (strstr(g_uart_buff, "NO CARRIER") != NULL)
    {
        printf("hang up.\n");

        GsmSetHungUpSts(true);

        if (GsmGetConnectSts() == true)
        {
            GsmSetConnectSts(false);
        }
    }
}

static uint32_t GsmSendCmd(char *cmd, int len)
{
    if (cmd == NULL || len <= 0)
    {
        return HI_ERR_FAILURE;
    }

    uint32_t ret = HI_ERR_FAILURE;
    static uint32_t count = 0;
    uint8_t *uart_buff_ptr = g_uart_buff;

    ret = hi_uart_write(DEMO_UART_NUM, (hi_u8 *)cmd, len);
    if (ret == HI_ERR_FAILURE)
    {
        return HI_ERR_FAILURE;
    }
    printf(" SendData:len:%d,cmd:%s.\n", len, cmd);

    while (g_uartController.isReadBusy)
    {
        count++;
        if (count > UART_WAIT_COUNT_MAX)
        {
            break;
        }
        
    }

    if (g_uartController.isReadBusy)
    {
        printf("GsmSendCmd hi_uart_read busy return");
        return HI_ERR_FAILURE;
    }
    if (!g_uartController.isReadBusy){
        usleep(100000); /* sleep 100ms */
    }
    
    g_uartController.isReadBusy = true;
    g_ReceivedDatalen = hi_uart_read(DEMO_UART_NUM, uart_buff_ptr, UART_BUFF_SIZE);

    if (g_ReceivedDatalen > 0)
    {
        printf(" rcvData len:%d,msg:%s.\n", g_ReceivedDatalen, g_uart_buff);
        if (strstr(g_uart_buff, "OK") != NULL)
        {
            GsmCheckRingAndHanupMessage();
            memset(g_uart_buff, 0, sizeof(g_uart_buff));
            g_ReceivedDatalen = 0;
            g_uartController.isReadBusy = false;
            return HI_ERR_SUCCESS;
        }
        else
        {
            printf(" received error cmd\r\n");
            GsmCheckRingAndHanupMessage();
            memset(g_uart_buff, 0, sizeof(g_uart_buff));
            g_ReceivedDatalen = 0;
            g_uartController.isReadBusy = false;
            return HI_ERR_FAILURE;
        }
    }
    else
    {
        g_uartController.isReadBusy = false;
        printf(" SendCmd no cmd return!\r\n");
        return HI_ERR_FAILURE;
    }

    return HI_ERR_SUCCESS;
}


uint32_t GpsGetLocation(GGPS_INFO *gpsInfo)
{
    uint32_t ret = HI_ERR_FAILURE;
    static uint32_t count = 0;
    uint8_t *uart_buff_ptr = g_uart_buff;

    ret = hi_uart_write(DEMO_UART_NUM, (hi_u8 *)"AT+CGNSINF\r\n", strlen("AT+CGNSINF\r\n"));
    if (ret == HI_ERR_FAILURE)
    {
        return NULL;
    }

    while (g_uartController.isReadBusy)
    {
        count++;
        if (count > UART_WAIT_COUNT_MAX)
        {
            break;
        }
        usleep(100000); /* sleep 100ms */
    }

    if (g_uartController.isReadBusy)
    {
        printf("GpsGetLocation hi_uart_read busy return");
        return HI_ERR_FAILURE;
    }else{
        usleep(100000); /* sleep 100ms */
    }

    g_uartController.isReadBusy = true;
    g_ReceivedDatalen = hi_uart_read(DEMO_UART_NUM, uart_buff_ptr, UART_BUFF_SIZE);

    if (g_ReceivedDatalen > 0)
    {
        printf(" rcvData len:%d,msg:%s.\n", g_ReceivedDatalen, g_uart_buff);

        uint8_t *strLocation = (uint8_t *)strstr(g_uart_buff, "+CGNSINF: 1,1");

        if (strLocation != NULL)
        {
            GGPS_DATA gpsData;
            GPS_CGNSINF_Analyze((char *)g_uart_buff, &gpsData);

            printf("latitude:%s.\n", gpsData.latitude);
            printf("logitude:%s.\n", gpsData.logitude);

            memcpy_s(gpsInfo->UTCdatetime, sizeof(gpsInfo->UTCdatetime), gpsData.UTCdatetime, sizeof(gpsData.UTCdatetime));
            memcpy_s(gpsInfo->logitude, sizeof(gpsInfo->logitude), gpsData.logitude, sizeof(gpsData.logitude));
            memcpy_s(gpsInfo->latitude, sizeof(gpsInfo->latitude), gpsData.latitude, sizeof(gpsData.latitude));
            memcpy_s(gpsInfo->satellitesinview, sizeof(gpsInfo->satellitesinview), gpsData.satellitesinview, sizeof(gpsData.satellitesinview));

            GsmCheckRingAndHanupMessage();

            memset(g_uart_buff, 0, sizeof(g_uart_buff));
            g_ReceivedDatalen = 0;
            g_uartController.isReadBusy = false;

            return HI_ERR_SUCCESS;
        } else {
            GsmCheckRingAndHanupMessage();
            memset(g_uart_buff, 0, sizeof(g_uart_buff));
            g_ReceivedDatalen = 0;
            g_uartController.isReadBusy = false;
            return HI_ERR_FAILURE;
        }
    } else {
        printf(" SendCmd no cmd return!\r\n");
        g_uartController.isReadBusy = false;

        return HI_ERR_FAILURE;
    }
}

uint32_t GsmCallCellPhone(char *cellPhoneNumeber)
{
    uint32_t ret = HI_ERR_FAILURE;
    char sendCmd[32] = "";
    uint8_t cPhoneNumLength = strlen(cellPhoneNumeber);

    if (cPhoneNumLength < PHONE_NUMB_LEN)
    {
        return HI_ERR_FAILURE;
    }

    /* Send AT+CSQ. */
    strncpy(sendCmd, "AT+CSQ\r\n", strlen("AT+CSQ\r\n"));
    printf(" sendCmd=%s\n", sendCmd);
    ret = GsmSendCmd(sendCmd, strlen(sendCmd));
    if (ret == HI_ERR_FAILURE)
    {
        return HI_ERR_FAILURE;
    }
    memset(sendCmd, 0, strlen(sendCmd));

    /* Call cellPhone Number:ATD+cellPhoneNumber. */
    snprintf(sendCmd, sizeof(sendCmd), "ATD%s;\r\n", cellPhoneNumeber);
    printf(" sendCmd=%s\n", sendCmd);
    ret = GsmSendCmd(sendCmd, strlen(sendCmd));
    if (ret == HI_ERR_FAILURE)
    {
        return HI_ERR_FAILURE;
    }

    return HI_ERR_SUCCESS;
}

uint32_t GsmHangUpPhone(void)
{
    uint32_t ret = HI_ERR_FAILURE;
    char sendCmd[16] = "";

    /* Hang up cellPhone :ATH. */
    strncpy(sendCmd, "ATH\r\n", strlen("ATH\r\n"));
    printf(" sendCmd=%s\n", sendCmd);
    ret = GsmSendCmd(sendCmd, strlen(sendCmd));
    if (ret == HI_ERR_FAILURE)
    {
        return HI_ERR_FAILURE;
    }

    return HI_ERR_SUCCESS;
}

uint32_t GsmGpsPowerOn(void)
{
    uint32_t ret = HI_ERR_FAILURE;
    char sendCmd[16] = "";

    /* Gps power on:AT+CGNSPWR=1 */
    strncpy(sendCmd, "AT+CGNSPWR=1\r\n", strlen("AT+CGNSPWR=1\r\n"));
    printf(" sendCmd=%s\n", sendCmd);
    ret = GsmSendCmd(sendCmd, strlen(sendCmd));
    if (ret == HI_ERR_FAILURE)
    {
        return HI_ERR_FAILURE;
    }

    return HI_ERR_SUCCESS;
}

static uint32_t GsmConnectPhone(void)
{
    uint32_t ret = HI_ERR_FAILURE;
    char sendCmd[16] = "";

    /* Answer cellPhone :ATA. */
    strncpy(sendCmd, "ATA\r\n", strlen("ATA\r\n"));
    printf(" sendCmd=%s\n", sendCmd);
    ret = GsmSendCmd(sendCmd, strlen(sendCmd));
    if (ret == HI_ERR_FAILURE)
    {
        return HI_ERR_FAILURE;
    }

    return HI_ERR_SUCCESS;
}

uint32_t GsmInit(void)
{
    uint32_t ret = HI_ERR_FAILURE;
    char *sendData = "AT\r\n";

    ret = GsmSendCmd(sendData, strlen(sendData));
    return ret;
}

uint32_t GetSimCardSts(void)
{
    return HI_ERR_SUCCESS;
}

static void UarthandleUartReceiveMessage(void)
{
    if (UartIsBufEmpty())
    {
        return;
    }
    if (g_uartController.isReadBusy)
    {
        return;
    }

    g_uartController.isReadBusy = true;
    g_ReceivedDatalen = hi_uart_read(DEMO_UART_NUM, g_uart_buff, UART_BUFF_SIZE);

    if (g_ReceivedDatalen > 0)
    {
        printf("handleUartReceiveMessage rcvData len:%d,msg:%s.\n", g_ReceivedDatalen, g_uart_buff);
        GsmCheckRingAndHanupMessage();

        memset(g_uart_buff, 0, sizeof(g_uart_buff));
        g_ReceivedDatalen = 0;
    }

    g_uartController.isReadBusy = false;
}

int UartIsBufEmpty(void)
{
    bool val = false;
    int ret;

    ret = hi_uart_is_buf_empty(HI_UART_IDX_1, &val);
    if (ret == HI_ERR_SUCCESS && val == HI_TRUE)
    {
        return 1;
    }

    return 0;
}

void GsmCallPhoneNumber(void)
{
    /* call cell phone number. */
    if (GsmGetDailSts() == false)
    {
        char *phoneNum = NULL;
        int totalPhoneNumber = Iot_getTotalPhoneNumber();

        printf("totalPhoneNumber = %d\n", totalPhoneNumber);

        if (totalPhoneNumber == 1)
        {
            char *phoneNumFather = (char *)IotProfile_GetFatherPhoneNumber();
            char *phoneNumMother = (char *)IotProfile_GetMohterPhoneNumber();

            if (strlen(phoneNumFather) == PHONE_NUMB_LEN)
            {
                GsmSetPhoneBookItem(E_PHONEBOOK_FATHER);
            }
            if (strlen(phoneNumMother) == PHONE_NUMB_LEN)
            {
                GsmSetPhoneBookItem(E_PHONEBOOK_MOTHER);
            }
        }

        if (GsmGetPhoneBookItem() == E_PHONEBOOK_FATHER){
            phoneNum = (char *)IotProfile_GetFatherPhoneNumber();
        } else {
            phoneNum = (char *)IotProfile_GetMohterPhoneNumber();
        }

        printf("phoneNum = %s,len=%d,g_phoneBookSelect=%d\n",
                phoneNum, strlen(phoneNum), GsmGetPhoneBookItem());

        if (strlen(phoneNum) == PHONE_NUMB_LEN)
        {
            GsmCallCellPhone(phoneNum);
            GsmSetDailSts(true);
        }
    }
    else /* hungUp phone. */
    {
        GsmSetDailSts(false);
        GsmHangUpPhone();
    }
}

void GsmControlGps(void)
{
    if (g_isGpsPowerOn == false){

        if (GsmGpsPowerOn() == HI_ERR_SUCCESS){
            g_isGpsPowerOn = true;
        }else{
            IotProfile_Report("", "", "", "");
        }
    } else {

        GGPS_INFO gpsInfo;
        if (GpsGetLocation(&gpsInfo) == HI_ERR_SUCCESS){
            printf("UTCdatetime = %s\n", gpsInfo.UTCdatetime);
            IotProfile_Report(gpsInfo.UTCdatetime, gpsInfo.logitude, gpsInfo.latitude, gpsInfo.satellitesinview);
        } else {
            IotProfile_Report("", "", "", "");
        }
    }
}
static void GsmHandleKeyPress(void)
{
    /* key presssed */
    if (keyLastSts.lastKeySts != button_GetKey2Sts())
    {
        keyLastSts.lastKeySts = button_GetKey2Sts();

        printf("uart_demo_task:g_isRing=%d,ConnectSts=%d,DailSts=%d\n", GsmGetRingSts(), GsmGetConnectSts(), GsmGetDailSts());
        printf("uart_demo_task:lastKeyLongPressSts=%d,g_isKey2LongPressed=%d\n",
                keyLastSts.lastKeyLongPressSts, button_GetKey2LongPressSts());

        if (button_GetKey2LongPressSts() == true){
            int totalPhoneNumber = Iot_getTotalPhoneNumber();

            printf("totalPhoneNumber = %d \n", totalPhoneNumber);

            if (totalPhoneNumber == 2)
            {
                g_phoneBookSelect = (g_phoneBookSelect + 1) % 2;
            }
            printf("after g_phoneBookSelect=%d\n", g_phoneBookSelect);
        } else {
            if (GsmGetRingSts()){  /* connect the phone */
                GsmSetConnectSts(true);
                GsmSetRingSts(false);
                GsmConnectPhone();
            } else if (GsmGetConnectSts() == true) { /* calling status, hungup phone. */
                GsmSetConnectSts(false);
                GsmSetRingSts(false);
                GsmHangUpPhone();
            } else if (GsmGetHungUpSts() == true){
                GsmSetRingSts(false);
                GsmSetConnectSts(false);
            } else if (GsmGetRingSts() == false && GsmGetConnectSts() == false){
                if (button_GetKey2LongPressSts() == false)
                {
                    GsmCallPhoneNumber();
                }
            } else {
            }
        }
    }

}

static void *uart_demo_task(void)
{
    static uint16_t countSendTimes = 0;
    static uint8_t countReceiveTimes = 0;

    g_uartController.isReadBusy = false;

    GsmContolInit();

    printf("Initialize uart demo successfully\n");

    while (1)
    {
        osDelay(50); /* add delay */

        UarthandleUartReceiveMessage();

        if (g_isInitSim800)
        {
            GsmHandleKeyPress();
            app_pageChange();
            countSendTimes++;
            if (countSendTimes > 20)
            {
                countSendTimes = 0;
                GsmControlGps();
            }
        }else{
            int ret = GsmInit();
            if (ret == HI_ERR_SUCCESS)
            {
                printf("-InitSim800_SUCCESS--------------------\n");
                g_isInitSim800 = 1;
            }
        }
    }

    return 0;
}

/*
 * This demo simply shows how to read datas from UART2 port and then echo back.
 */
void uart_task_create(void)
{
    /* Create a task to handle uart communication */
    osThreadAttr_t attr;

    attr.name = "uartTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = UART_DEMO_TASK_STAK_SIZE;
    attr.priority = CONFIG_TASK_UART_PRIOR;

    if (osThreadNew((osThreadFunc_t)uart_demo_task, NULL, &attr) == NULL)
    {
        printf("Falied to create uart_demo_task!\n");
    }
}



void app_uart_init(void)
{

    hi_uart_attribute uart_attr = {
        .baud_rate = UART_BAUD_RATE, /* baud_rate: 115200 */
        .data_bits = UART_DATA_BITS,      /* data_bits: 8bits */
        .stop_bits = UART_STOP_BITS,
        .parity = 0,
    };

    /* Initialize uart driver */
    hi_u32 ret = hi_uart_init(HI_UART_IDX_1, &uart_attr, HI_NULL);
    if (ret != HI_ERR_SUCCESS)
    {
        printf("Failed to init uart! Err code = %d\n", ret);
        return;
    }
}