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

#ifndef _CENCTRL_LOG_H_
#define _CENCTRL_LOG_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C"{
#endif
enum LOG_LEVEL {
    CENCTRL_DEBUG = 0,
    CENCTRL_INFO,
    CENCTRL_WARN,
    CENCTRL_ERROR,

    LOGLEVEL_NBR
};

static const char* CENCTRL_LOG_TAG[LOGLEVEL_NBR] = {
    "CENCTRL_DEBUG",
    "CENCTRL_INFO",
    "CENCTRL_WARN",
    "CENCTRL_ERROR"
};

static int loglevel = CENCTRL_DEBUG;

#define SET_LOG_LEVEL(lv) ((lv) < LOGLEVEL_NBR ? loglevel = (lv) : loglevel = CENCTRL_DEBUG)

#define LOG(LOGEVEL, fmt, args...) ({                                                       \
    if (LOGEVEL <= loglevel) {                                                              \
        printf("[%s][%s][%d]" fmt, CENCTRL_LOG_TAG[LOGEVEL], __func__, __LINE__, ##args);   \
    }                                                                                       \
})
#ifdef __cplusplus
}
#endif

#endif /* __CENCTRL_LOG_H__ */