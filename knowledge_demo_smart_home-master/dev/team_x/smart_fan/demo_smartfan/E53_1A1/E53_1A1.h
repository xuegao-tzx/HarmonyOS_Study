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

#ifndef E53_1A1_H_
#define E53_1A1_H_

#include "iot_demo_def.h"

/**
 * @brief Initialize the fan device
 * @return Returns 0 success while others failed
 */
void E531A1_FanInit(void);

/**
 * @brief Set main fan status
 * @param status, defines as CN_BOARD_SWITCH_ON/OFF in iot_demo_def.h
 * @param speedLevel, should be from 1-4
 * @return Returns 0 success while others failed
 */
int E531A1_SetFanStatus(int status, int speedLevel);


#endif /* E53_SC1_H_ */
