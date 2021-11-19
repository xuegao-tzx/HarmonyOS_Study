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

#ifndef E53_SC1_H_
#define E53_SC1_H_

#include <iot_demo_def.h>

/**
 *  @brief initialize the E53SF1 extended board, like the MQ2 AND BEEP
 * 		   after this operation, the sensor could work as espected
 */
int E53SF1_Init(void);

/**
 * @brief Get the MQ2 value
 * @param mq2 used to storage the value and could not be NULL
 * @return Returns 0 success while others failed
 */
int E53SF1_GetMq2(float *mq2);

/**
 * @brief Set the Beep status
 * @param status as defines CN_BOARD_SWICTH_ON/OFF
 * @return Returns 0 success while others failed
 */
int E53SF1_SetBeep(int status);

#endif /* E53_SC1_H_ */
