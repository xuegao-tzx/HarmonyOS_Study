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

#ifndef __COMMON_INIT_H__
#define __COMMON_INIT_H__

#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "securec.h"
#include "ohos_types.h"
#include "iot_gpio.h"
#include "iot_errno.h"
#include "iot_i2c.h"

/**
 * @brief Init the trashcan device. Include the button Oled.
 *
 * @since 1.0
 * @version 1.0
 */
void CommonDeviceInit(void);

/**
 * @brief Init the led of trashcan. which use for show the network status.
 * @return Returns 0 success while others failed
 * @since 1.0
 * @version 1.0
 */
int InitPwmLed(void);

/**
 * @brief Set Pwm led duty cycle
 * @param dutyCycle, the duty cycle to set, not max than 40000
 * @return Returns 0 success while others failed
 */
int SetPwmLedDutyCycle(int dutyCycle);

/**
 * @brief Set hispark led duty cycle
 * @param flashHz, indicates the flash frequency
 * @return Returns 0 success while others failed
 */
int HisparkLedFlashFrequencySet(float flashHz);

#endif  /* __COMMON_INIT_H__ */
