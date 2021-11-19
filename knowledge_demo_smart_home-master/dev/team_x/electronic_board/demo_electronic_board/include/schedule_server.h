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

#ifndef __SCHEDULE_SERVER_H__
#define __SCHEDULE_SERVER_H__

#include "link_list.h"

#ifdef  __cplusplus
extern "C" {
#endif

#define BUFF_SHORT   64
#define BUFF_LONG    512

typedef struct {
    char id[BUFF_SHORT];
    char name[BUFF_SHORT];
    char details[BUFF_LONG];
    char startTime[BUFF_SHORT];
    char endTime[BUFF_SHORT];

    int weekday;
    int startHour;
    int startMinute;
    int endHour;
    int endMinute;
} ScheduleInfoT;

/**
 * @brief callback functions used to deal the shcedule list update
 * @param data Indicates the data for the caller to use
 * @param list Indicates the list to update.
 *
 * @return Returns 0 success, when -1 failed
 */
typedef int (*ScheduleUpdate)(void *data, ListHandle list);

/**
 * @brief start schedule server
 * @param updateFunc Indicates the callback function
 * @param data Indicates the data used by the callback.
 *
 * @return Returns 0 success, when -1 failed
 */
int ScheduleServerStart(ScheduleUpdate updateFunc, void *data);

/**
 * @brief stop schedule server
 * 
 */
void ScheduleServerStop(void);

#ifdef  __cplusplus
}
#endif
#endif  /* __SCHEDULE_SERVER_H__ */
