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

#ifndef __WINDER_H__
#define __WINDER_H__


#include <cmsis_os2.h>

#define WINDER_TASK_STACK_SIZE      (1024*4)
#define WINDER_TASK_PRIO            (25)
#define WINDER_LOOP_DELAY           (500)
#define TIMER_HALF_HOUR             30
#define TIMER_ONE_HOUR              60
#define TIMER_MAX_HOURS             8
#define TIMER_60_SECOND             60

#define WINDER_MAX_SCOP             4


#define ASCII_0         ('0')
#define ASCII_1         ('1')

#define TICKS_NUMBER    (100)  // 1 ticks = 10ms
#define MULTIPLE        (10)
#define SPEED_PLUS      0
#define SPEED_MINUS     1

#define BUF_SIZE        64

#define POWER_XPOS      0
#define POWER_YPOS      0
#define NETSTA_XPOS     0
#define NETSTA_YPOS     1
#define MODE_XPOS       0
#define MODE_YPOS       2
#define SPEED_XPOS      0
#define SPEED_YPOS      3
#define SCOPE_XPOS      0
#define SCOPE_YPOS      4
#define TIMER_XPOS      0
#define TIMER_YPOS      5

#define MESSAGE_LEN     6
#define MSG_VAL_LEN     2
typedef enum {
    MESSAGE_POWER_OFF = 1,
    MESSAGE_SPEED_SET,
    MESSAGE_SPEED_MODE,
    MESSAGE_SCOP_SET,
    MESSAGET_TIMER_SET,

    MESSAGE_POWER_OFF_S = 11,
    MESSAGE_SPEED_SET_S,
    MESSAGE_SPEED_MODE_S,
    MESSAGE_SCOP_SET_S,
    MESSAGET_TIMER_SET_S,
}MESSAGE_TYPE;

typedef enum {
    WINDER_MODE_SLEEP = 1,
    WINDER_MODE_NATURAL,
    WINDER_MODE_AUTO,

    WINDER_MODE_NBR
}WINDER_MODE;

#endif  /* __WINDER_H__ */
