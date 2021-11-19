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
 * @brief Initialize the board main led
 * @return Returns 0 success while others failed
 */
int E53SC1_InitMainLed(void);

/**
 * @brief Set main led status
 * @param status, defines as CN_BOARD_SWITCH_ON/OFF in iot_demo_def.h
 * @param dutyCycle, should be from 1-99
 * @return Returns 0 success while others failed
 */
int E53SC1_SetMainLed(int status, int dutyCycle);


/**
 * @brief Initialize the board light intensity sensor bH1750
 * @return Returns 0 success while others failed
 */
int E53SC1_InitLightIntensity(void);

/**
 * @brief Get the light intensity from BH1750
 * @param lightIntensity,point to a int buffer to storage the value
 * @return Returns 0 success while others failed
 */
int E53SC1_GetLightIntensity(int *lightIntensity);

#endif /* E53_SC1_H_ */
