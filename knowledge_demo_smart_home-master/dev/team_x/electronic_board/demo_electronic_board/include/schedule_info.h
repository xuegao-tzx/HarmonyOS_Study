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

#ifndef __SCHEDULE_INFO_H__
#define __SCHEDULE_INFO_H__

#include <securec.h>

namespace OHOS {
class ScheduleInfo {
public:
    ScheduleInfo() = default;
    ~ScheduleInfo(){};

    void setId(const char *id)
    {
        (void)strncpy_s(id_, sizeof(id_), id, strlen(id));
    }

    void setName(const char *name)
    {
        if (name != nullptr) {
            (void)strncpy_s(name_, sizeof(name_), name, strlen(name));
        }
    }

    void setDetails(const char *details)
    {
        if (details != nullptr) {
            (void)strncpy_s(details_, sizeof(details_), details, strlen(details));
        }
    }

    void setWeek(int weekday)
    {
        weekday_ = weekday;
    }

    void setTime(const char *startTime, const char *endTime)
    {
        char *ptr = NULL;
        if (startTime == nullptr || endTime == nullptr) {
            return;
        }
        ptr = (char *)strchr(startTime, ' ');
        if (ptr == nullptr) {
            (void)strncpy_s(startTime_, sizeof(startTime_), startTime, strlen(startTime));
        } else {
            ptr++;
            (void)strncpy_s(startTime_, sizeof(startTime_), ptr, strlen(ptr));
        }
        ptr = strrchr(startTime_, ':');
        if (ptr) {
            *ptr = 0x00;
        }

        ptr = (char *)strchr(endTime, ' ');
        if (ptr == nullptr) {
            (void)strncpy_s(endTime_, sizeof(endTime_), endTime, strlen(endTime));
        } else {
            ptr++;
            (void)strncpy_s(endTime_, sizeof(endTime_), ptr, strlen(ptr));
        }
        ptr = strrchr(endTime_, ':');
        if (ptr) {
            *ptr = 0x00;
        }
        sscanf_s(startTime_, "%d:%d", &startHours_, &startMinutes_);
        sscanf_s(endTime_, "%d:%d", &endHours_, &endMinutes_);
    }

    void setTime(int startHour, int startMinute, int endHour, int endMinute)
    {
        startHours_ = startHour;
        startMinutes_ = startMinute;
        endHours_ = endHour;
        endMinutes_ = endMinute;
    }

    int getId(char *id, int size)
    {
        if (id == nullptr || size <= 0) {
            return -1;
        }
        if (strlen(id_) == 0) {
            return -1;
        }

        (void)strncpy_s(id, size, id_, strlen(id_));

        return 0;
    }

    int getName(char *name, int size)
    {
        if (name == nullptr || size <= 0) {
            return -1;
        }
        if (strlen(name_) == 0) {
            return -1;
        }

        (void)strncpy_s(name, size, name_, strlen(name_));

        return 0;
    }

    int getStartTime(char *startTime, int size)
    {
        if (startTime == nullptr || size <= 0) {
            return -1;
        }
        if (strlen(startTime_) == 0) {
            return -1;
        }

        (void)strncpy_s(startTime, size, startTime_, strlen(startTime_));

        return 0;
    }

    int getEndTime(char *endTime, int size)
    {
        if (endTime == nullptr || size <= 0) {
            return -1;
        }
        if (strlen(endTime_) == 0) {
            return -1;
        }

        (void)strncpy_s(endTime, size, endTime_, strlen(endTime_));

        return 0;
    }

    int getDetails(char *details, int size)
    {
        if (details == nullptr || size <= 0) {
            return -1;
        }
        if (strlen(details_) == 0) {
            return -1;
        }

        (void)strncpy_s(details, size, details_, strlen(details_));

        return 0;
    }

    int getWeekDay()
    {
        return weekday_;
    }

    int getStartTimeHours()
    {
        return startHours_;
    }

    int getStartTimeMinutes()
    {
        return startMinutes_;
    }

    int getEndTimeHours()
    {
        return endHours_;
    }

    int getEndTimeMinutes()
    {
        return endMinutes_;
    }
private:
    static constexpr int SCH_BUF_SIZE = 256;
    static constexpr int SCH_LONG_BUF = 512;

    char id_[SCH_BUF_SIZE] = {0};
    char name_[SCH_BUF_SIZE] = {0};
    char details_[SCH_LONG_BUF] = {0};
    char startTime_[SCH_BUF_SIZE] = {0};
    char endTime_[SCH_BUF_SIZE] = {0};

    int weekday_ = 0;
    int startHours_ = 0;
    int startMinutes_ = 0;
    int endHours_ = 0;
    int endMinutes_ = 0;
};
}

#endif  /* __SCHEDULE_INFO_H__ */
