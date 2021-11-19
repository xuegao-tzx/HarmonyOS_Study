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

#ifndef __PWM_H__
#define __PWM_H__

#include "ohos_types.h"

#define WIFI_IOT_GPIO_IDX_2 2
#define IOT_IO_FUNC_GPIO    0
#define GPIO_PIN_HIGH       1
#define GPIO_PIN_LOW        0
#define PWM_DELAY_TIME      100

#define PWM_TASK_PRIO       30
#define PWM_TASK_STACK_SIZE 512

typedef enum {
    PWM_LEVEL_0 = 0,    // 0为关闭PWM，其他对应PWM等级
    PWM_LEVEL_1,
    PWM_LEVEL_2,
    PWM_LEVEL_3,
    PWM_LEVEL_4,
    PWM_LEVEL_5,

    PWM_LEVEL_NBR
} PWM_LEVEL;

int PwmInit(void);

void PwmSetLevel(PWM_LEVEL level);

#endif  /* __PWM_H__ */
