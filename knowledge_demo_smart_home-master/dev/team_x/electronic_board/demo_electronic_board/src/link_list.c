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

#include "link_list.h"

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
} ScheduleInfo;

#define LST_DBG(fmt, args...)   printf("[LIST_DEBUG][%s|%d]" fmt, __func__, __LINE__, ##args)
#define LST_ERR(fmt, args...)   printf("[LIST_ERROR][%s|%d]" fmt, __func__, __LINE__, ##args)

typedef struct ListNodeDec {
    void *data;
    int size;

    struct ListNodeDec *prev;
    struct ListNodeDec *next;
} ListNode;

typedef struct {
    ListNode *head, *tail;
    int total;
} LinkList;

static ListNode *ListNodeCreate(void *data, int size)
{
    ListNode *pNode = NULL;
    if (data == NULL || size <= 0) {
        LST_ERR("null point! \n");
        return NULL;
    }

    pNode = (ListNode *)malloc(sizeof(ListNode));
    if (pNode == NULL) {
        LST_ERR("OOM! \n");
        return NULL;
    }

    pNode->data = malloc(size);
    (void)memcpy_s(pNode->data, size, data, size);
    pNode->size = size;
    pNode->prev = NULL;
    pNode->next = NULL;

    return pNode;
}

static void ListNodeDestroy(ListNode *pNode)
{
    if (pNode) {
        if (pNode->data) {
            free(pNode->data);
            pNode->data = NULL;
        }
        free(pNode);
        pNode = NULL;
    }
}

static ListNode *ListNodeSearchIndex(ListNode *pHead, int idx)
{
    ListNode *pNode = pHead;
    for (int i = 0; i < idx; i++) {
        if (pNode == NULL) {
            break;
        }
        pNode = pNode->next;
    }

    return pNode;
}

static ListNode *ListNodeSearchData(ListNode *pHead, void *data, int size)
{
    ListNode *pNode = pHead;
    while (pNode) {
        if (pNode->data) {
            if (memcmp(pNode->data, data, size) == 0) {
                LST_DBG("get the same item! \n");
                break;
            }
        }
    }

    return pNode;
}

static ListNode *ListNodeSearchLeast(ListNode *pHead, void *data)
{
    ListNode *pNode = pHead;
    ScheduleInfo *info = (ScheduleInfo *)data;
    while (pNode != NULL) {
        ScheduleInfo *tmp = (ScheduleInfo *)pNode->data;
        if (tmp == NULL) {
            LST_ERR("NULL POINT! \n");
            break;
        }
        if (info->startTime == NULL || tmp->startTime == NULL) {
            LST_ERR("NULL POINT! \n");
            break;
        }
        if (strcmp(info->startTime, tmp->startTime) < 0) {
            return pNode;
        }
        pNode = pNode->next;
    }
    return pNode;
}

ListHandle LinkListCreate(void)
{
    LinkList *list = (LinkList *)malloc(sizeof(LinkList));
    if (list == NULL) {
        LST_ERR("OOM! \n");
        return NULL;
    }

    list->head = list->tail = NULL;
    list->total = 0;

    return (ListHandle)list;
}

int LinkListGetSize(ListHandle mHandle)
{
    LinkList *list = (LinkList *)mHandle;
    if (list == NULL) {
        LST_ERR("null point! \n");
        return 0;
    }

    return list->total;
}

int LinkListAppend(ListHandle mHandle, void *data, int size)
{
    ListNode *pNode = NULL;
    LinkList *list = (LinkList *)mHandle;
    if (list == NULL) {
        LST_ERR("null point! \n");
        return -1;
    }

    pNode = ListNodeCreate(data, size);
    if (pNode == NULL) {
        LST_ERR("ListNodeCreate failed! \n");
        return -1;
    }

    if (list->tail == NULL) {
        list->head = list->tail = pNode;
    } else {
        list->tail->next = pNode;
        pNode->prev = list->tail;
        list->tail = pNode;
    }

    list->total++;

    return 0;
}

int LinkListAdd(ListHandle mHandle, void *data, int size)
{
    ListNode *pNode = NULL;
    LinkList *list = (LinkList *)mHandle;
    if (list == NULL) {
        LST_ERR("null point! \n");
        return -1;
    }

    pNode = ListNodeCreate(data, size);
    if (pNode == NULL) {
        LST_ERR("ListNodeCreate failed! \n");
        return -1;
    }
    LST_ERR("\n");
    if (list->head == NULL) {
        list->head = list->tail = pNode;
    } else {
        ListNode *ptmp = ListNodeSearchLeast(list->head, data);
        if (ptmp == NULL) {
            pNode->prev = list->tail;
            list->tail->next = pNode;
            list->tail = pNode;
        } else {
            pNode->prev = ptmp->prev;
            pNode->next = ptmp;
            if (ptmp->prev != NULL) {
                ptmp->prev->next = pNode;
            } else {
                list->head = pNode;
            }
            ptmp->prev = pNode;
        }
    }

    list->total++;
    LST_ERR("\n");
    return 0;
}

void *LinkListGetWithIndex(ListHandle mHandle, int idx)
{
    ListNode *pNode = NULL;
    LinkList *list = (LinkList *)mHandle;
    if (list == NULL) {
        LST_ERR("null point! \n");
        return NULL;
    }
    if (idx >= list->total) {
        LST_ERR("(%d) is OOR! total is [%d] \n", idx, list->total);
        return NULL;
    }

    pNode = ListNodeSearchIndex(list->head, idx);
    if (pNode == NULL) {
        LST_ERR("search list (%d) item failed! \n", idx);
        return NULL;
    }

    return pNode->data;
}

int LinkListDeleteData(ListHandle mHandle, void *data, int size)
{
    ListNode *pNode = NULL;
    LinkList *list = (LinkList *)mHandle;
    if (list == NULL) {
        LST_ERR("null point! \n");
        return -1;
    }

    if (list->total == 0) {
        LST_ERR("list is empty! \n");
        return -1;
    }

    pNode = ListNodeSearchData(list->head, data, size);
    if (pNode == NULL) {
        LST_ERR("search list data failed! \n");
        return -1;
    }

    if (pNode->prev != NULL && pNode->next != NULL) {
        pNode->prev->next = pNode->next;
        pNode->next->prev = pNode->prev;
    } else {
        if (pNode->prev == NULL) {
            list->head = pNode->next;
        } else if (pNode->next == NULL) {
            list->tail = pNode->prev;
        }
    }
    pNode->prev = NULL;
    pNode->next = NULL;
    ListNodeDestroy(pNode);

    if (list->total > 0) {
        list->total--;
    }

    return 0;
}

int LinkListDeleteWithIdx(ListHandle mHandle, int idx)
{
    ListNode *pNode = NULL;
    LinkList *list = (LinkList *)mHandle;
    if (list == NULL) {
        LST_ERR("null point! \n");
        return -1;
    }

    if (idx >= list->total) {
        LST_ERR("(%d) is OOR! total is [%d] \n", idx, list->total);
        return -1;
    }

    pNode = ListNodeSearchIndex(list->head, idx);
    if (pNode == NULL) {
        LST_ERR("search list data failed! \n");
        return -1;
    }

    if (pNode->prev != NULL && pNode->next != NULL) {
        pNode->prev->next = pNode->next;
        pNode->next->prev = pNode->prev;
    } else {
        if (pNode->prev == NULL) {
            list->head = pNode->next;
        } else if (pNode->next == NULL) {
            list->tail = pNode->prev;
        }
    }
    pNode->prev = NULL;
    pNode->next = NULL;
    ListNodeDestroy(pNode);

    if (list->total > 0) {
        list->total--;
    }

    return 0;
}

void LinkListClear(ListHandle mHandle)
{
    ListNode *pNode = NULL;
    LinkList *list = (LinkList *)mHandle;
    if (list == NULL) {
        LST_ERR("null point! \n");
        return;
    }
    if (list->total == 0) {
        LST_ERR("list is empty! \n");
        return;
    }
    pNode = list->head;
    while (pNode) {
        ListNode *tmp = pNode;
        pNode = pNode->next;
        if (pNode) {
            pNode->prev = NULL;
        }
        tmp->next = NULL;

        ListNodeDestroy(tmp);
    }

    list->head = list->tail = NULL;
    list->total = 0;
}

void LinkListDestroy(ListHandle mHandle)
{
    LinkList *list = (LinkList *)mHandle;
    if (list == NULL) {
        LST_ERR("null point! \n");
        return;
    }
    LinkListClear(mHandle);

    free(list);
    list = NULL;
}
