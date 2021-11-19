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

#ifndef __LINK_LIST_H__
#define __LINK_LIST_H__
#ifdef  __cplusplus
extern "C" {
#endif

typedef void *ListHandle;

ListHandle LinkListCreate(void);
int LinkListGetSize(ListHandle mHandle);
int LinkListAppend(ListHandle mHandle, void *data, int size);
int LinkListAdd(ListHandle mHandle, void *data, int size);
void *LinkListGetWithIndex(ListHandle mHandle, int idx);
int LinkListDelete(ListHandle mHandle, void *data, int size);
int LinkListDeleteWithIdx(ListHandle mHandle, int idx);
void LinkListClear(ListHandle mHandle);
void LinkListDestroy(ListHandle mHandle);

#ifdef  __cplusplus
}
#endif
#endif  /* __LINK_LIST_H__ */
