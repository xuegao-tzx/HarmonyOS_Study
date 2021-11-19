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

#ifndef  __PERIPHERAL_HAL_H__
#define  __PERIPHERAL_HAL_H__

typedef enum {
    /** GPIO hardware pin 0 */
    HAL_WIFI_IOT_IO_NAME_GPIO_0,
    /** GPIO hardware pin 1 */
    HAL_WIFI_IOT_IO_NAME_GPIO_1,
    /** GPIO hardware pin 2 */
    HAL_WIFI_IOT_IO_NAME_GPIO_2,
    /** GPIO hardware pin 3 */
    HAL_WIFI_IOT_IO_NAME_GPIO_3,
    /** GPIO hardware pin 4 */
    HAL_WIFI_IOT_IO_NAME_GPIO_4,
    /** GPIO hardware pin 5 */
    HAL_WIFI_IOT_IO_NAME_GPIO_5,
    /** GPIO hardware pin 6 */
    HAL_WIFI_IOT_IO_NAME_GPIO_6,
    /** GPIO hardware pin 7 */
    HAL_WIFI_IOT_IO_NAME_GPIO_7,
    /** GPIO hardware pin 8 */
    HAL_WIFI_IOT_IO_NAME_GPIO_8,
    /** GPIO hardware pin 9 */
    HAL_WIFI_IOT_IO_NAME_GPIO_9,
    /** GPIO hardware pin 10 */
    HAL_WIFI_IOT_IO_NAME_GPIO_10,
    /** GPIO hardware pin 11 */
    HAL_WIFI_IOT_IO_NAME_GPIO_11,
    /** GPIO hardware pin 12 */
    HAL_WIFI_IOT_IO_NAME_GPIO_12,
    /** GPIO hardware pin 13 */
    HAL_WIFI_IOT_IO_NAME_GPIO_13,
    /** GPIO hardware pin 14 */
    HAL_WIFI_IOT_IO_NAME_GPIO_14,
    /** Maximum value */
    HAL_WIFI_IOT_IO_NAME_MAX,
} HalWifiIotIoName;

unsigned int HalIoSetFunc(HalWifiIotIoName id, unsigned char *val);

typedef enum {
    ADC_CHANNEL_0,
    ADC_CHANNEL_1,
    ADC_CHANNEL_2,
    ADC_CHANNEL_3,
    ADC_CHANNEL_4,
    ADC_CHANNEL_5,
    ADC_CHANNEL_6,
    ADC_CHANNEL_7,
    ADC_CHANNEL_BUTT,
} ADC_CHANNEL;

unsigned int HalAdcRead(ADC_CHANNEL channel, unsigned short *buffer);

#endif // __PERIPHERAL_HAL_H__