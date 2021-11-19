/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hal_hota_board.h"
#include "hi_upg_api.h"
#include "stdio.h"

int HotaHalInit(void)
{
    hi_u32 result = hi_upg_init();
    if (result != HI_ERR_SUCCESS && result != HI_ERR_UPG_INITILIZATION_ALREADY) {
        printf("hi_upg_init Failed.Result = %x.\r\n", result);
        return OHOS_FAILURE;
    }
    return OHOS_SUCCESS;
}

int HotaHalGetUpdateIndex(unsigned int *index)
{
    if (hi_upg_get_file_index((hi_u8 *)index) != HI_ERR_SUCCESS) {
        printf("get upgrade index error\r\n");
        return OHOS_FAILURE;
    }
    return OHOS_SUCCESS;
}

int HotaHalDeInit(void)
{
    hi_u32 result = hi_upg_stop();
    if (result != HI_ERR_SUCCESS) {
        printf("hi_upg_stop Failed.Result = %x.\r\n", result);
        return OHOS_FAILURE;
    }
    return OHOS_SUCCESS;
}

int HotaHalRead(int partition, unsigned int offset, unsigned int bufLen, unsigned char *buffer)
{
    if ((buffer == NULL) || (bufLen == 0)) {
        return OHOS_FAILURE;
    }

    if (hi_upg_get_content(offset, buffer, bufLen) != HI_ERR_SUCCESS) {
        printf("get ota content error! Partition: %d\r\n", partition);
        return OHOS_FAILURE;
    }

    return OHOS_SUCCESS;
}

int HotaHalWrite(int partition, unsigned char *buffer, unsigned int offset, unsigned int buffLen)
{
    if (partition == PARTITION_INFO_COMP) {
        printf("partition == PARTITION_INFO_COMP, skip it.");
        return OHOS_SUCCESS;
    }
    hi_u32 result = hi_upg_transmit(offset, buffer, buffLen);
    if (result != HI_ERR_SUCCESS) {
        printf("hi_upg_transmit failed. retCode = %x.\r\n", result);
        return OHOS_FAILURE;
    }
    return OHOS_SUCCESS;
}

int HotaHalRestart(void)
{
    hi_upg_finish_with_cache();
    return OHOS_SUCCESS;
}

int HotaHalSetBootSettings(void)
{
    hi_u32 result = hi_upg_transmit_finish_save_cache();
    if (result != HI_ERR_SUCCESS) {
        printf("hi_upg_transmit_finish failed. retCode = %x.\r\n", result);
        return OHOS_FAILURE;
    }
    return OHOS_SUCCESS;
}

int HotaHalRollback(void)
{
    return OHOS_SUCCESS;
}
