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

#include <ctype.h>
#include <securec.h>
#include <stdio.h>
#include <stdlib.h>

#include "ohos_errno.h"
#include "ohos_types.h"
#include "utils_config.h"
#include "utils_file.h"

#include "schedule_store.h"

#define SCHEDULE_KEY    "shcedule_info"
#define TEMP_KEY        "info.tmp"

static int SearchScheduleInfo(const char *id)
{
    int idx = -1;
    int total = 0;
    int file;

    if (id == NULL) {
        return -1;
    }

    file = UtilsFileOpen(SCHEDULE_KEY, O_RDWR_FS, 0);
    if (file < 0) {
        return -1;
    }

    UtilsFileSeek(file, 0, SEEK_SET_FS);
    if (UtilsFileRead(file, (char *)&total, sizeof(int)) < 0) {
        UtilsFileClose(file);
        return -1;
    }

    for (int i = 0; i < total; i++) {
        ScheduleInfo info;
        int off = sizeof(ScheduleInfo) * i + sizeof(int);
        if (UtilsFileSeek(file, off, SEEK_SET_FS) < 0) {
            UtilsFileClose(file);
            return -1;
        }
        if (UtilsFileRead(file, (char *)&info, sizeof(ScheduleInfo)) < 0) {
            UtilsFileClose(file);
            return -1;
        }
        if (strncmp((const char *)info.id, id, strlen(id)) == 0) {
            idx = i;
            break;
        }
    }

    UtilsFileClose(file);

    return idx;
}

static int DeletScheduleStore(ScheduleInfo *info)
{
    int fd;
    int tmp;
    int idx;
    int total;
    if (info == NULL) {
        return -1;
    }

    idx = SearchScheduleInfo((const char *)info->id);
    if (idx < 0) {
        return -1;
    }

    fd = UtilsFileOpen(SCHEDULE_KEY, O_RDWR_FS, 0);
    if (fd < 0) {
        return -1;
    }

    UtilsFileSeek(fd, 0, SEEK_SET_FS);
    if (UtilsFileRead(fd, (char *)&total, sizeof(int)) < 0) {
        UtilsFileClose(fd);
        return -1;
    }
    if (total <= 0) {
        UtilsFileClose(fd);
        return -1;
    } else if (total == 1) {
        UtilsFileClose(fd);
        UtilsFileDelete(SCHEDULE_KEY);
        return 0;
    }

    total -= 1;

    tmp = UtilsFileOpen(TEMP_KEY, O_CREAT_FS | O_RDWR_FS, 0);
    if (tmp < 0) {
        UtilsFileClose(fd);
        return -1;
    }

    if (UtilsFileWrite(tmp, (char *)&total, sizeof(int)) < 0) {
        UtilsFileClose(fd);
        UtilsFileClose(tmp);
        return -1;
    }

    for (int i = 0; i <= total; i++) {
        ScheduleInfo tmpInfo;
        if (UtilsFileRead(fd, (char *)&tmpInfo, sizeof(ScheduleInfo)) < 0) {
            UtilsFileClose(fd);
            UtilsFileClose(tmp);
            return -1;
        }
        if (i == idx) {
            continue;
        }
        if (UtilsFileWrite(tmp, (char *)&tmpInfo, sizeof(ScheduleInfo)) < 0) {
            UtilsFileClose(fd);
            UtilsFileClose(tmp);
            return -1;
        }
    }

    UtilsFileClose(fd);
    UtilsFileClose(tmp);

    UtilsFileDelete(SCHEDULE_KEY);
    UtilsFileCopy(TEMP_KEY, SCHEDULE_KEY);
    UtilsFileDelete(TEMP_KEY);

    return 0;
}

static int UpdateScheduleStore(ScheduleInfo *info)
{
    int fd, idx;

    if (info == NULL) {
        return -1;
    }

    idx = SearchScheduleInfo((const char *)info->id);
    if (idx < 0) {
        return -1;
    }

    fd = UtilsFileOpen(SCHEDULE_KEY, O_RDWR_FS, 0);
    if (fd < 0) {
        return -1;
    }

    if (UtilsFileSeek(fd, idx * sizeof(ScheduleInfo) + sizeof(int), SEEK_SET_FS) < 0) {
        UtilsFileClose(fd);
        return -1;
    }

    if (UtilsFileWrite(fd, (char *)info, sizeof(ScheduleInfo)) < 0) {
        UtilsFileClose(fd);
        return -1;
    }

    UtilsFileClose(fd);

    return 0;
}

static int AddScheduleStore(ScheduleInfo *info)
{
    int total = 0;
    int fd;

    if (info == NULL) {
        return -1;
    }

    fd = UtilsFileOpen(SCHEDULE_KEY, O_RDWR_FS | O_CREAT_FS | O_APPEND_FS, 0);
    if (fd < 0) {
        return -1;
    }
    UtilsFileSeek(fd, 0, SEEK_SET_FS);
    if (UtilsFileRead(fd, (char *)&total, sizeof(int)) < 0) {
        UtilsFileClose(fd);
        return -1;
    }
    total += 1;
    UtilsFileSeek(fd, 0, SEEK_SET_FS);
    if (UtilsFileWrite(fd, (char *)&total, sizeof(int)) < 0) {
        UtilsFileClose(fd);
        return -1;
    }

    UtilsFileSeek(fd, 0, SEEK_END_FS);
    if (UtilsFileWrite(fd, (char *)info, sizeof(ScheduleInfo)) < 0) {
        UtilsFileClose(fd);
        return -1;
    }

    UtilsFileClose(fd);

    return 0;
}

int ScheduleStoreUpdate(ScheduleInfo *info, SCHEDULE_OPTION option)
{
    int retval = -1;
    switch (option) {
        case SCHEDULE_OPTION_ADD:
            AddScheduleStore(info);
            break;
        case SCHEDULE_OPTION_UPDATE:
            UpdateScheduleStore(info);
            break;
        case SCHEDULE_OPTION_DELETE:
            DeletScheduleStore(info);
            break;
        default:
            break;
    }

}

int ScheduleStoreGetTotal(void)
{
    int total = 0;
    int fd = UtilsFileOpen(SCHEDULE_KEY, O_RDWR_FS, 0);
    if (fd < 0) {
        return -1;
    }

    if (UtilsFileRead(fd, (char *)&total, sizeof(int)) < 0) {
        total = -1;
    }
    UtilsFileClose(fd);

    return total;
}

int ScheduleStoreGetInfo(ScheduleInfo *info, int idx)
{
    int total = 0;
    int fd = UtilsFileOpen(SCHEDULE_KEY, O_RDWR_FS, 0);
    if (fd < 0) {
        return -1;
    }

    if (UtilsFileRead(fd, (char *)&total, sizeof(int)) < 0) {
        UtilsFileClose(fd);
        return -1;
    }

    if (idx >= total) {
        UtilsFileClose(fd);
        return -1;
    }

    if (UtilsFileSeek(fd, idx * sizeof(ScheduleInfo) + sizeof(int), SEEK_SET_FS) < 0) {
        UtilsFileClose(fd);
        return -1;
    }

    if (UtilsFileRead(fd, (char *)info, sizeof(ScheduleInfo)) < 0) {
        UtilsFileClose(fd);
        return -1;
    }

    UtilsFileClose(fd);

    return 0;
}

void ScheduleStoreDelete(void)
{
    UtilsFileDelete(SCHEDULE_KEY);
}
