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

#include <hi_types_base.h>
#include <hi_early_debug.h>

#include <hi_stdlib.h>
#include <unistd.h>
#include <string.h>

#include "ohos_types.h"
#include "iot_errno.h"
#include "kal.h"
#include "sensor.h"
#include "defines.h"

static uint32 SensorWrite(uint8 triggerCmd, uint8 highByteCmd, uint8 lowByteCmd)
{
    uint8 _send_user_cmd[AHT_SNED_CMD_LEN] = {triggerCmd, highByteCmd, lowByteCmd};

    return IoTI2cWrite(WIFI_IOT_SENSOR_I2C_IDX_0, (AHT_DEVICE_ADDR<<1) | 0x00, _send_user_cmd, AHT_SNED_CMD_LEN);
}

static uint32 SensorRead(uint8 type)
{
    uint8 recvData[AHT_REG_ARRAY_LEN] = {0};
    float temper = 0;
    uint32 temper_t = 0;

    memset_s(&recvData, sizeof(recvData), 0x0, sizeof(recvData));

    IoTI2cRead(WIFI_IOT_SENSOR_I2C_IDX_0, (AHT_DEVICE_ADDR<<1) | 0x01, recvData, AHT_REG_ARRAY_LEN);
    if (type == AHT_TEMPERATURE) {
        temper = (float)((recvData[AHT_REG_TEMP_BITS_1] &0x0f)<<AHT_REG_TEMP_OFFSET
            | recvData[AHT_REG_TEMP_BITS_2]<<AHT_REG_TEMP_OFFSET_1 | recvData[AHT_REG_TEMP_BITS_3]); // 娓╁害鎷兼帴
        temper_t = (uint32)((temper / AHT_CALCULATION) * 200 - 50);  // T= (S_t/2^20)*200-50
    }

    return temper_t;
}

int SensorInit(void)
{
    uint32 status;
    uint8 recvDataInit[AHT_REG_ARRAY_INIT_LEN] = {0};
    uint8 initSendUserCmd[AHT_SNED_CMD_LEN] = {
        AHT_DEVICE_INIT_CMD,
        AHT_DEVICE_PARAM_INIT_HIGH,
        AHT_DEVICE_PARAM_LOW_BYTE };

    memset_s(&recvDataInit, sizeof(recvDataInit), 0x0, sizeof(recvDataInit));
    status = IoTI2cRead(WIFI_IOT_SENSOR_I2C_IDX_0, (AHT_DEVICE_ADDR<<1) | 0x01, recvDataInit, AHT_REG_ARRAY_INIT_LEN);
    if (((recvDataInit[0] != AHT_DEVICE_CALIBRATION_ERR)
        && (recvDataInit[0] != AHT_DEVICE_CALIBRATION_ERR_R))
        || (recvDataInit[0] == AHT_DEVICE_CALIBRATION)) {
        status = IoTI2cWrite(WIFI_IOT_SENSOR_I2C_IDX_0, (AHT_DEVICE_ADDR<<1) | 0x00, initSendUserCmd, AHT_SNED_CMD_LEN);
        usleep(AHT_SLEEP_1S);
    }

    return status;
}
// AHT20温度传感器获取温度
int SensorGetValue(void)
{
    uint32 temp;

    uint32 status = SensorWrite(AHT_DEVICE_TEST_CMD, AHT_DEVICE_PARAM_HIGH_BYTE, AHT_DEVICE_PARAM_LOW_BYTE);
    if (status != IOT_SUCCESS) {
        LOG_E("get temp data error!\r\n");
        return -1;
    }
    osDelay(AHT_DELAY_100MS); // 100ms Waiting for the capture finished.
    temp = SensorRead(AHT_TEMPERATURE);

    return temp;
}
