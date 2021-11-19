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

#include <string.h>
#include <unistd.h>
#include "iot_errno.h"
#include "iot_i2c.h"
#include "log.h"
#include "oled.h"
#include "code_tab.h"


static uint8 OledInitCmd[CMD_LENGTH] = {
    DISPLAY_OFF,               // Display off
    SET_LOW_COLUMN_ADDRESS,    // ---set low column address
    SET_HIGH_COLUMN_ADDRESS,   // ---set high column address
    SET_START_LINE_ADDRESS,    // --set start line address
    SET_PAGE_ADDRESS,          // --set page address
    CONTRACT_CONTROL,          // contract control
    FULL_SCREEN,               // --128
    SET_SEGMENT_REMAP,         // set segment remap
    NORMAL,
    SET_MULTIPLEX,             // --set multiplex ratio(1 to 64)
    DUTY,                      // --1/32 duty
    SCAN_DIRECTION,            // Com scan direction
    DISPLAY_OFFSET,            // -set display offset
    DISPLAY_TYPE,
    OSC_DIVISION,              // set osc division
    DIVISION,
    COLOR_MODE_OFF,            // set area color mode off
    COLOR,
    PRE_CHARGE_PERIOD,         // set Pre-Charge Period
    PERIOD,
    PIN_CONFIGUARTION,         // set com pin configuartion
    CONFIGUARTION,
    SET_VCOMH,                 // set Vcomh
    VCOMH,
    SET_CHARGE_PUMP_ENABLE,    // set charge pump enable
    PUMP_ENABLE,
    TURN_ON_OLED_PANEL         // --turn on oled panel
};

/* I2C write Byte */
static uint32 I2cWriteByte(uint8 regAddr, uint8 cmd)
{
    uint32 status = 0;
    uint8 userData = cmd;
    uint8 sendUserCmd [2] = {0x00, userData};
    uint8 sendUserData [2] = {0x40, userData};

    // write cmd
    if (regAddr == 0x00) {
        status = IoTI2cWrite(WIFI_IOT_OLED_I2C_IDX_0, OLED_ADDRESS, sendUserCmd, sizeof(sendUserCmd));
        if (status != IOT_SUCCESS) {
            LOG_E("===== Error: SSD1306 OLED Screen I2C write status = 0x%x! =====\r\n", status);
            return status;
        }
    } else if (regAddr == 0x40) { // write data
        status = IoTI2cWrite(WIFI_IOT_OLED_I2C_IDX_0, OLED_ADDRESS, sendUserData, sizeof(sendUserData));
        if (status != IOT_SUCCESS) {
            LOG_E("===== Error: SSD1306 OLED Screen I2C write status = 0x%x! =====\r\n", status);
            return status;
        }
    }
    return IOT_SUCCESS;
}

static uint32 WriteCmd(uint8 cmd)
{
    // Oled write cmd
    if (IOT_SUCCESS != I2cWriteByte(OLED_ADDRESS_WRITE_CMD, cmd)) {
        LOG_E("===== Error!!! Write Cmd : write device address cmd failed %d =====\r\n", __LINE__);
        return IOT_FAILURE;
    }
    return IOT_SUCCESS;
}

static uint32 WriteData(uint8 I2C_Data)
{
    // Oled write data
    if (IOT_SUCCESS != I2cWriteByte(OLED_ADDRESS_WRITE_DATA, I2C_Data)) {
        LOG_E("===== Error!!! Write Data :  write device address failed %d =====\r\n", __LINE__);
        return IOT_FAILURE;
    }
    return IOT_SUCCESS;
}

void OledSetPosition(uint8 x, uint8 y)
{
    if (IOT_SUCCESS != WriteCmd(0xb0 + y)) {
        LOG_E("WriteCmd Error \n");
        return;
    }
    if (IOT_SUCCESS != WriteCmd(((x&0xf0)>>SET_POSITION_OFFSET) | 0x10)) {
        LOG_E("WriteCmd Error \n");
        return;
    }
    if (IOT_SUCCESS != WriteCmd(x & 0x0f)) {
        LOG_E("WriteCmd Error \n");
        return;
    }
}

void OledFillScreen(uint8 fii_data)
{
    for (uint8 m = 0; m < MAX_LINE_OF_LANTTICE; m++) {
        if (IOT_SUCCESS != WriteCmd(0xb0 + m)) {
            LOG_E("WriteCmd Error \n");
            return;
        }
        if (IOT_SUCCESS != WriteCmd(0x00)) {
            LOG_E("WriteCmd Error \n");
            return;
        }
        if (IOT_SUCCESS != WriteCmd(0x10)) {
            LOG_E("WriteCmd Error \n");
            return;
        }
        for (uint8 n = 0; n < Max_Column; n++) {
            if (IOT_SUCCESS != WriteData(fii_data)) {
                LOG_E("WriteData Error \n");
                return;
            }
        }
    }
}

void OledShowChar(uint8 x, uint8 y, uint8 chr, uint8 charSize)
{
    uint8 i;
    uint8 c = chr - ' '; // calc the offset

    if (charSize == CHAR_SIZE_OF_LANTTICE_8_16) {
        OledSetPosition(x, y);
        for (i = 0; i < HORI_PIXEL_OF_LANTTICE_8_16; i++) {
            WriteData(F8X16[c*CHAR_SIZE_OF_LANTTICE_8_16 + i ]);
        }
        OledSetPosition(x, y + 1);
        for (i = 0; i < HORI_PIXEL_OF_LANTTICE_8_16; i++) {
            WriteData(F8X16[c*CHAR_SIZE_OF_LANTTICE_8_16 + i + HORI_PIXEL_OF_LANTTICE_8_16]);
        }
    } else if (charSize == CHAR_SIZE_OF_LANTTICE_HZ_16_16) {
        c = chr - '0';
        OledSetPosition(x, y);
        for (i = 0; i < HORI_PIXEL_OF_LANTTICE_HZ_16_16; i++) {
            WriteData(HZ_F16X16[c][0][i]);
        }

        OledSetPosition(x, y + 1);
        for (i = 0; i < HORI_PIXEL_OF_LANTTICE_HZ_16_16; i++) {
            WriteData(HZ_F16X16[c][1][i]);
        }
    } else if (charSize == CHAR_SIZE_OF_LANTTICE_HZ_16_16_1) {
        c = chr - '0';
        OledSetPosition(x, y);
        for (i = 0; i < HORI_PIXEL_OF_LANTTICE_HZ_16_16_1; i++) {
            WriteData(HZ_F16X16_1[c][0][i]);
        }
        OledSetPosition(x, y + 1);
        for (i = 0; i < HORI_PIXEL_OF_LANTTICE_HZ_16_16_1; i++) {
            WriteData(HZ_F16X16_1[c][1][i]);
        }
    } else {
        OledSetPosition(x, y);
        for (i = 0; i < HORI_PIXEL_OF_LANTTICE_6_8; i++) {
            WriteData(F6X8[c][i]);
        }
    }
}

void OledShowStr(uint8 x, uint8 y, const char *chr, uint8 charSize)
{
    uint8 j = 0;
    if (chr == NULL) {
        return;
    }

    while (chr[j] != '\0') {
        if (x >= LIMIT_HORI_PIXEL) {
            LOG_E("please input the correct x parameter\n");
            return;
        }
        OledShowChar(x, y, chr[j], charSize);
        if (charSize == CHAR_SIZE_OF_LANTTICE_HZ_16_16 || charSize == CHAR_SIZE_OF_LANTTICE_HZ_16_16_1) {
            x += HORI_PIXEL_OF_LANTTICE_HZ_16_16;
        } else {
            x += HORI_PIXEL_OF_LANTTICE_8_16;
        }
        j++;
    }
}

uint32 OledInit(void)
{
    uint32 status;
    osDelay(DELAY_100_MS); // 100ms  Keep the power supply stable.
    for (int i = 0; i < CMD_LENGTH; i++) {
        status = WriteCmd(OledInitCmd[i]);
        if (status != IOT_SUCCESS) {
            LOG_E("Failed to write OledInitCmd: 0x%x \n", OledInitCmd[i]);
            return IOT_FAILURE;
        }
    }

    OledFillScreen(0x00); // The screen off
    osDelay(DELAY_10_MS); // 10ms
    OledShowStr(HORIZONTAL_COORDINATE_OF_SLOGANS, VERTICAL_COORDINATE_OF_SLOGANS, "OpenHarmony OS", 1);
    osDelay(DELAY_500_MS); // The time of keep display
    OledShowStr(HORIZONTAL_COORDINATE_OF_SLOGANS, VERTICAL_COORDINATE_OF_SLOGANS, "              ", 1);
    LOG_I("OledInit succuss\n");
    return IOT_SUCCESS;
}