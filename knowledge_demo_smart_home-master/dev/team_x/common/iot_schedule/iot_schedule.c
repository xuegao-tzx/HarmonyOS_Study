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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "schedule_list.h"
#include "schedule_store.h"
#include "iot_schedule.h"

#define MAX_SCHEDULE_LIST       7

static SCHEDULE_LIST gScheduleList[MAX_SCHEDULE_LIST];
static int g_isUpdate = 0;
static int g_startSeconds = -1;
static int g_durationSeconds = -1;
static int g_curWeekDay = -1;
static ScheduleCommand g_cmd = {0};
static int g_oneTime = 0;

static int GetWeekDay(unsigned char weekday, int Days[], int size)
{
    int n = 0;
    if (size != MAX_SCHEDULE_LIST) {
        SCH_ERR("weekday array is OOR! \n");
        return -1;
    }

    for (int i = 0; i < MAX_SCHEDULE_LIST; i++) {
        if ((weekday >> i) & 0x01) {
            Days[n++] = i;
        }
    }

    return n;
}

static void ScheduleGetStore(void)
{
    int total = ScheduleStoreGetTotal();
    if (total <= 0) {
        SCH_ERR("no schedule stored! \n");
        return;
    }

    for (int i = 0; i < total; i++) {
        ScheduleInfo info = {0};
        int weeknum = 0;
        int week[MAX_SCHEDULE_LIST] = {0};

        if (ScheduleStoreGetInfo(&info, i) < 0) {
            SCH_ERR("read schedulestore %d failed! \n", i);
            continue;
        }

        weeknum = GetWeekDay(info.week, week, MAX_SCHEDULE_LIST);
        if (weeknum <= 0) {
            SCH_ERR("the schedule has no weekday info! \n");
            continue;
        }

        for (int j = 0; j < weeknum; j++) {
            int weekday = week[j];
            if (weekday >= MAX_SCHEDULE_LIST) {
                SCH_ERR("weekday is OOR!\n");
                break;
            }
            if (ScheduleListAppend(gScheduleList[weekday], &info) < 0) {
                SCH_ERR("ScheduleListAppend failed!\n");
                break;
            }
            SCH_DBG("add info : id--%s, starttime--%d, duration--%d, week(%d)\n",
                info.id, info.starttime, info.duration, weekday);
        }
    }
}

int IOT_ScheduleInit(void)
{
    int retval = 0;

    for (int i = 0; i < MAX_SCHEDULE_LIST; i++) {
        gScheduleList[i] = ScheduleListCreate(NULL);
        if (gScheduleList[i] == NULL) {
            SCH_ERR("ScheduleListCreate (%d) failed! \n", i);
            retval = -1;
            break;
        }
    }

    if (retval < 0) {
        for (int i = 0; i < MAX_SCHEDULE_LIST; i++) {
            if (gScheduleList[i] != NULL) {
                ScheduleListDestroy(gScheduleList[i]);
                gScheduleList[i] = NULL;
            }
        }
    } else {
        ScheduleGetStore();
        g_isUpdate = 1;
    }

    return retval;
}

static int ScheduleUpdate(const char *id, int *day, int size, int startTime, int durTime,
            SCHEDULE_OPTION option, int cmd, int value)
{
typedef int (*OptionFunc)(SCHEDULE_LIST mHandle, ScheduleInfo *info);
    OptionFunc opFunc[] = {ScheduleListAppend, ScheduleListUpdate, ScheduleListDelete};
    ScheduleInfo info = {0};
    info.starttime = startTime;
    info.duration = durTime;
    info.scheduleCmd.cmd = cmd;
    info.scheduleCmd.value = value;

    SCH_DBG("cmd = %d, value = %d \n", cmd, value);

    if (option > SCHEDULE_OPTION_DELETE) {
        SCH_ERR("OPTION OOR!\n");
        return -1;
    }
    if (size == 1 && day[0] == 0) { // one time schedule
        g_oneTime = 1;
        g_isUpdate = 1;
        g_startSeconds = startTime;
        g_durationSeconds = durTime;
        g_cmd.value = value;
        g_cmd.cmd = cmd;
        SCH_DBG("this schedule is onetime! \n");
        return 0;
    }

    strncpy(info.id, id, strlen(id));

    for (int i = 0; i < size; i++) {
        int idx = day[i] - 1;
        if (idx >= MAX_SCHEDULE_LIST) {
            SCH_ERR("weekday is OOR! \n");
            continue;
        }
        info.week += (1 << idx);

        if (opFunc[option](gScheduleList[idx], &info) < 0) {
            SCH_ERR("ScheduleListAppend failed! \n");
            return -1;
        }
        SCH_DBG("add info : id--%s, starttime--%d, duration--%d, week(%d)--0x%x, cmd--%d, value -- %d\n",
            info.id, info.starttime, info.duration, idx, info.week, info.scheduleCmd.cmd, info.scheduleCmd.value);
    }

    ScheduleStoreUpdate(&info, option);

    if ((option == SCHEDULE_OPTION_DELETE) && (g_startSeconds == startTime)) {
        g_startSeconds = 0;
        g_durationSeconds = 0;
    }

    g_isUpdate = 1;
    return 0;
}

int IOT_ScheduleAdd(const char *id, int *day, int size, int startTime, int durTime, int cmd, int value)
{
    return ScheduleUpdate(id, day, size, startTime, durTime, SCHEDULE_OPTION_ADD, cmd, value);
}

int IOT_ScheduleUpdate(const char *id, int *day, int size, int startTime, int durTime, int cmd, int value)
{
    return ScheduleUpdate(id, day, size, startTime, durTime, SCHEDULE_OPTION_UPDATE, cmd, value);
}

int IOT_ScheduleDelete(const char *id, int *day, int size, int startTime, int durTime, int cmd, int value)
{
    return ScheduleUpdate(id, day, size, startTime, durTime, SCHEDULE_OPTION_DELETE, cmd, value);
}

int IOT_ScheduleIsUpdate(unsigned int weekday, int curtime)
{
    int total = 0;
    int retval = 0;

    if (g_isUpdate == 0) {
        return 0;
    }

    g_isUpdate = 0;

    if (g_oneTime) {
        g_oneTime = 0;
        if (g_startSeconds > curtime) {
            SCH_DBG("get one time schedule! \n");
            return 1;
        }
    }


    if (weekday > MAX_SCHEDULE_LIST || curtime < 0) {
        return 0;
    }

    total = ScheduleListGetSize(gScheduleList[weekday]);
    if (total <= 0) {
        return 0;
    }

    for (int i = 0; i < total; i++) {
        ScheduleInfo info;
        if (ScheduleListGet(gScheduleList[weekday], i, (ScheduleInfo *)&info) < 0) {
            SCH_ERR("ScheduleListGet failed! \n");
            return 0;
        }

        if (curtime < (info.starttime + info.duration)) {
            if (g_curWeekDay == weekday) {
                if (g_startSeconds != info.starttime ||
                g_durationSeconds != info.duration) {
                    g_startSeconds = info.starttime;
                    g_durationSeconds = info.duration;
                    g_cmd.cmd = info.scheduleCmd.cmd;
                    g_cmd.value = info.scheduleCmd.value;
                    retval = 1;
                }
            } else {
                g_curWeekDay = weekday;
                g_startSeconds = info.starttime;
                g_durationSeconds = info.duration;
                g_cmd.cmd = info.scheduleCmd.cmd;
                g_cmd.value = info.scheduleCmd.value;
                retval = 1;
            }
            break;
        }
    }

    if (retval == 1) {
        SCH_DBG("cmd = %d, value = %d \n", g_cmd.cmd, g_cmd.value);
    }

    return retval;
}

void IOT_ScheduleSetUpdate(int update)
{
    g_isUpdate = update;
}

int IOT_ScheduleGetStartTime(void)
{
    return g_startSeconds;
}

int IOT_ScheduleGetDurationTime(void)
{
    return g_durationSeconds;
}

int IOT_ScheduleGetCommand(int *cmd, int *value)
{
    if (cmd != NULL) {
        *cmd = g_cmd.cmd;
    }

    if (value != NULL) {
        *value = g_cmd.value;
    }

    return 0;
}

void IOT_ScheduleDeinit(void)
{
    for (int i = 0; i < MAX_SCHEDULE_LIST; i++) {
        if (gScheduleList[i] != NULL) {
            ScheduleListDestroy(gScheduleList[i]);
            gScheduleList[i] = NULL;
        }
    }
}
