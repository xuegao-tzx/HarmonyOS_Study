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

#ifndef __SENSOR_H__
#define __SENSOR_H__

#define WIFI_IOT_SENSOR_I2C_IDX_0   (0)
#define AHT_SLEEP_20MS              (20000) // 20ms
#define AHT_SLEEP_50MS              (50000) // 50ms
#define AHT_SLEEP_1S                (1000000) // 1s

#define AHT_DELAY_10MS              (1) // 10ms
#define AHT_DELAY_40MS              (4) // 40ms
#define AHT_DELAY_100MS             (10) // 100ms
#define AHT_DEVICE_ADDR             (0x38) // device addr
#define AHT_DEVICE_READ_STATUS      (0x71) // befor read tem&humi data need to send cmd to comfir the
#define AHT_DEVICE_INIT_CMD         (0xBE) // aht init cmd
#define AHT_DEVICE_TEST_CMD         (0xAC) // test cmd
#define AHT_DEVICE_PARAM_HIGH_BYTE  (0x33)
#define AHT_DEVICE_PARAM_LOW_BYTE   (0x00)
#define AHT_DEVICE_PARAM_INIT_HIGH  (0x08)
#define AHT_DEVICE_CALIBRATION      (0x80)
#define AHT_DEVICE_CALIBRATION_ERR  (0x1C)
#define AHT_DEVICE_CALIBRATION_ERR_R (0x18)
#define AHT_TASK_SLEEP_TIME         (20000) // thread sleep 20ms

#define  AHT_REG_ARRAY_LEN          (6)
#define  AHT_REG_TEMP_BITS_1        (3)
#define  AHT_REG_TEMP_BITS_2        (4)
#define  AHT_REG_TEMP_BITS_3        (5)
#define  AHT_REG_TEMP_OFFSET        (16)
#define  AHT_REG_TEMP_OFFSET_1      (8)
#define  AHT_OC_ARRAY_LEN           (6)
#define  AHT_SNED_CMD_LEN           (3)
#define  SENSOR_TASK_STAK_SIZE      (1024*4)
#define  SENSOR_TASK_PRIORITY       (14)
#define  AHT_REG_ARRAY_INIT_LEN     (1)
#define  AHT_CALCULATION            (1048576)
#define  DECIMAL                    10
#define  NUMBER_OF_DECIMAL_POINTS   3

typedef enum {
    AHT_TEMPERATURE = 1, // TEMP
    AHT_HUMIDITY = 2, // HUMI
} aht_serson_type;


/**
 * @brief The sensor init operation
 *
 * @return If success return WIFI_IOT_SUCCESS. else return WIFI_IOT_FAILURE.
 * @since 1.0
 * @version 1.0
 */
int SensorInit(void);


/**
 * @brief Get the temp value of sensor.
 *
 * @return Return the temp value.
 * @since 1.0
 * @version 1.0
 */
int SensorGetValue(void);

#endif  /* __SENSOR_H__ */
