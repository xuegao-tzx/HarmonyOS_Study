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

#include "peripheral_hal.h"
#include "iot_errno.h"
#include "hi_io.h"
#include "hi_adc.h"

unsigned int HalIoSetFunc(HalWifiIotIoName id, unsigned char *val)
{
    if (id == HAL_WIFI_IOT_IO_NAME_MAX) {
        return IOT_FAILURE;
    }
    return hi_io_set_func((hi_io_name)id, val);
}

unsigned int HalAdcRead(ADC_CHANNEL channel, unsigned short *buffer)
{
    if (channel > ADC_CHANNEL_BUTT || channel < ADC_CHANNEL_0 || buffer == NULL) {
        return IOT_FAILURE;
    }

    return hi_adc_read(channel, buffer, HI_ADC_EQU_MODEL_4, HI_ADC_CUR_BAIS_DEFAULT, 0);
}
