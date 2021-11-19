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

#ifndef __COMMON_H__
#define __COMMON_H__

#include <time.h>

#ifdef  __cplusplus
extern "C" {
#endif

/**
 *  get system time
 */
#define LOCAL_TIME_ZONE    8

/**
 * @brief Get the time from network/local.
 * @param rtcTime Indicates the tm data to store the time info.
 *
 * @return Returns 0 success, when -1 failed
 */
int SystemGetTime(struct tm *rtcTime);

/**
 * @brief Get the network status.
 *
 * @return Returns 0 network disconnect, 1 network connectted
 */
int SystemGetNetStatus(void);

/*
*   weather
*/
#define WEATHER_BUF_SIZE    32
typedef struct {
    char city[WEATHER_BUF_SIZE];
    char date[WEATHER_BUF_SIZE];
    char week[WEATHER_BUF_SIZE];
    char time[WEATHER_BUF_SIZE];
    char weather[WEATHER_BUF_SIZE];
    char temperature[WEATHER_BUF_SIZE];
    char humidity[WEATHER_BUF_SIZE];
    char airSpeed[WEATHER_BUF_SIZE];
    char airQuality[WEATHER_BUF_SIZE];
    char airPM25[WEATHER_BUF_SIZE];
} WeatherInfo;

/**
 * @brief Get the weather info
 * @param wInfo Indicates the WeatherInfo data to store the Weather info.
 *
 * @return Returns 0 success, when -1 failed
 */
int SystemGetWeather(WeatherInfo *wInfo);

#ifdef  __cplusplus
}
#endif
#endif  /* __COMMON_H__ */
