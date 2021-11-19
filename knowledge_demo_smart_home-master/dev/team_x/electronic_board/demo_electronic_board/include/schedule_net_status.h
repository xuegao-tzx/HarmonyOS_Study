
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

#ifndef     __SCHEDULE_NET_STATUS_H__
#define     __SCHEDULE_NET_STATUS_H__
#ifdef  __cplusplus
extern "C" {
#endif

/**
 * @brief Start the net check process.
 *
 * @return Returns 0 success, when -1 failed
 */
int ScheduleStartNetStatus(void);

/**
 * @brief Stop the net check process.
 *
 */
void ScheduleStopNetStatus(void);

/**
 * @brief Get the network status.
 *
 * @return Returns 1 network connect, 0 network disconnect.
 */
int ScheduleGetStatus(void);

#ifdef  __cplusplus
}
#endif

#endif      __SCHEDULE_NET_STATUS_H__
