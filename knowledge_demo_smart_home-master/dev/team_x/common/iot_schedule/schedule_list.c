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

#include "schedule_list.h"

typedef struct __ScheduleNode {
    ScheduleInfo *data;
    struct __ScheduleNode *prev;
    struct __ScheduleNode *next;
} ScheduleNode;

typedef struct {
    ScheduleNode *head, *tail;
    int total;
} ScheduleList;

static ScheduleNode *ScheduleNodeCreate(ScheduleInfo *info)
{
    ScheduleNode *pNode;

    if (info == NULL) {
        SCH_ERR("NULL POINT! \n");
        return NULL;
    }

    pNode = (ScheduleNode *)malloc(sizeof(ScheduleNode));
    if (pNode == NULL) {
        SCH_ERR("OOM!\n");
        return NULL;
    }

    pNode->data = (char *)malloc(sizeof(ScheduleInfo));
    if (pNode->data == NULL) {
        SCH_ERR("OOM!\n");
        free(pNode);
        return NULL;
    }

    memcpy(pNode->data, info, sizeof(ScheduleInfo));
    pNode->prev = NULL;
    pNode->next = NULL;

    return pNode;
}

static void ScheduleNodeDestroy(ScheduleNode *pNode)
{
    if (pNode == NULL) {
        return;
    }
    if (pNode->data) {
        free(pNode->data);
        pNode->data = NULL;
    }
    free(pNode);
    pNode = NULL;
}

static ScheduleNode *ScheduleGetNodeWithTime(ScheduleNode *head, int time)
{
    ScheduleNode *pNode = head;
    while (pNode) {
        if (pNode->data && (time <= pNode->data->starttime)) {
            break;
        }
        pNode = pNode->next;
    }

    return pNode;
}

static ScheduleNode *ScheduleGetNodeWithId(ScheduleNode *head, const char *id)
{
    ScheduleNode *pNode = head;
    if (id == NULL) {
        return NULL;
    }
    while (pNode) {
        if (pNode->data && (strcmp(pNode->data->id, id) == 0)) {
            break;
        }
        pNode = pNode->next;
    }

    return pNode;
}

static ScheduleNode *ScheduleGetNodeWithIdx(ScheduleNode *head, int idx)
{
    ScheduleNode *pNode = head;
    for (int i = 0; i < idx; i++) {
        if (pNode) {
            pNode = pNode->next;
        }
    }

    return pNode;
}

SCHEDULE_LIST ScheduleListCreate(ScheduleInfo *info)
{
    ScheduleNode *pNode;
    ScheduleList *list = (ScheduleList *)malloc(sizeof(ScheduleList));
    if (list == NULL) {
        SCH_ERR("OOM!\n");
        return NULL;
    }

    pNode = ScheduleNodeCreate(info);
    if (pNode != NULL) {
        list->total = 1;
    } else {
        list->total = 0;
    }
    list->head = list->tail = pNode;

    return (SCHEDULE_LIST)list;
}

int ScheduleListAppend(SCHEDULE_LIST mHandle, ScheduleInfo *info)
{
    ScheduleNode *pNode;
    ScheduleList *list = (ScheduleList *)mHandle;
    if (list == NULL || info == NULL) {
        SCH_ERR("NULL POINT! \n");
        return -1;
    }

    pNode = ScheduleNodeCreate(info);
    if (pNode == NULL) {
        SCH_ERR("ScheduleNodeCreate failed! \n");
        return -1;
    }

    if (list->head == NULL) {
        list->head = list->tail = pNode;
    } else {
        ScheduleNode *temp = ScheduleGetNodeWithTime(list->head, info->starttime);
        if (temp == NULL) {
            pNode->prev = list->tail;
            list->tail->next = pNode;
            list->tail = pNode;
        } else {
            pNode->prev = temp->prev;
            pNode->next = temp;
            if (temp->prev != NULL) {
                temp->prev->next = pNode;
            } else {
                list->head = pNode;
            }
            temp->prev = pNode;
        }
    }

    list->total++;

    return 0;
}

int ScheduleListUpdate(SCHEDULE_LIST mHandle, ScheduleInfo *info)
{
    int retval = 0;
    ScheduleNode *pNode;
    ScheduleList *list = (ScheduleList *)mHandle;
    if (list == NULL || info == NULL) {
        SCH_ERR("NULL POINT! \n");
        return -1;
    }

    pNode = ScheduleGetNodeWithId(list->head, info->id);
    if (pNode != NULL) {
        memcpy(pNode->data, info, sizeof(ScheduleInfo));
    } else {
        retval = ScheduleListAppend(mHandle, info);
    }

    return retval;
}

int ScheduleListDelete(SCHEDULE_LIST mHandle, ScheduleInfo *info)
{
    ScheduleNode *pNode, *prev, *next;
    ScheduleList *list = (ScheduleList *)mHandle;
    if (list == NULL || info == NULL) {
        SCH_ERR("NULL POINT! \n");
        return -1;
    }

    pNode = ScheduleGetNodeWithId(list->head, info->id);
    if (pNode == NULL) {
        return -1;
    }
    prev = pNode->prev;
    next = pNode->next;

    if (prev == NULL && next == NULL) {
        list->head = list->tail = NULL;
    } else if (prev == NULL && next != NULL) {
        next->prev = NULL;
        list->head = next;
    } else if (prev != NULL && next == NULL) {
        prev->next = NULL;
        list->tail = prev;
    } else {
        prev->next = next;
        next->prev = prev;
    }
    pNode->next = NULL;
    pNode->prev = NULL;
    list->total--;
    ScheduleNodeDestroy(pNode);

    return 0;
}

int ScheduleListGetSize(SCHEDULE_LIST mHandle)
{
    if (mHandle == NULL) {
        return -1;
    }

    return ((ScheduleList *)mHandle)->total;
}

int ScheduleListGet(SCHEDULE_LIST mHandle, int idx, ScheduleInfo *info)
{
    ScheduleNode *pNode;
    ScheduleList *list = (ScheduleList *)mHandle;
    if (list == NULL || info == NULL) {
        SCH_ERR("NULL POINT! \n");
        return -1;
    }

    pNode = ScheduleGetNodeWithIdx(list->head, idx);
    if (pNode == NULL || pNode->data == NULL) {
        SCH_ERR("no such node(%d) \n", idx);
        return -1;
    }

    memcpy(info, pNode->data, sizeof(ScheduleInfo));

    return 0;
}

void ScheduleListDestroy(SCHEDULE_LIST mHandle)
{
    ScheduleList *list = (ScheduleList *)mHandle;
    if (list == NULL) {
        return;
    }
    while (list->head) {
        ScheduleNode *pNode = list->head;
        list->head = pNode->next;
        pNode->next = NULL;
        list->head->prev = NULL;

        ScheduleNodeDestroy(pNode);
    }
    list->total = 0;

    free(list);
    list = NULL;
}
