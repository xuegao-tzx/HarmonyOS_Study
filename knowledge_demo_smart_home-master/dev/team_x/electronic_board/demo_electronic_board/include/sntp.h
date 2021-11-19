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

#ifndef __SNTP_H__
#define __SNTP_H__

#include <time.h>

#ifdef  __cplusplus
extern "C" {
#endif

/**
 * @brief Get the current time from network with sntp protocal
 * @param localTimeZone Indicates the local time zone
 * @param rtcTime Indicates the rtcTime to store the time info.
 *
 * @return Returns 0 success, when -1 failed
 */
int SntpGetRtcTime(int localTimeZone, struct tm *rtcTime);

#ifdef  __cplusplus
}
#endif
#endif  /* __SNTP_H__ */
