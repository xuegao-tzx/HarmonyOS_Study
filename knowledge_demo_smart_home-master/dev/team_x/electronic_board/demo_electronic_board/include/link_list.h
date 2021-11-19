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

/**
 * @brief defined the list handle
 * 
 */
typedef void *ListHandle;

/**
 * @brief Create the list
 *
 * @return Returns the list handle success, when NULL failed
 */
ListHandle LinkListCreate(void);

/**
 * @brief Get the list number
 * @param mHandle Indicates the list handle
 *
 * @return Returns the list numbers success, when -1 failed
 */
int LinkListGetSize(ListHandle mHandle);

/**
 * @brief Add the item to the list with the tail position.
 * @param mHandle Indicates the list handle
 * @param data Indicates the data to append the list
 * @param size Indicates the size of the data
 *
 * @return Returns 0 success, when -1 failed
 */
int LinkListAppend(ListHandle mHandle, void *data, int size);

/**
 * @brief Add the item to the list with sort.
 * @param mHandle Indicates the list handle
 * @param data Indicates the data to append the list
 * @param size Indicates the size of the data
 *
 * @return Returns 0 success, when -1 failed
 */
int LinkListAdd(ListHandle mHandle, void *data, int size);

/**
 * @brief Get the data from the list with position
 * @param mHandle Indicates the list handle
 * @param idx Indicates the position in the list
 *
 * @return Returns Data Point success, when NULL failed
 */
void *LinkListGetWithIndex(ListHandle mHandle, int idx);

/**
 * @brief Delete the data in the list
 * @param mHandle Indicates the list handle
 * @param data Indicates the data to delete from the list
 * @param size Indicates the size of the data
 *
 * @return Returns 0 success, when -1 failed
 */
int LinkListDelete(ListHandle mHandle, void *data, int size);

/**
 * @brief Delete the data in the list with position
 * @param mHandle Indicates the list handle
 * @param data Indicates the data to append the list
 * @param idx Indicates the position in the list
 *
 * @return Returns 0 success, when -1 failed
 */
int LinkListDeleteWithIdx(ListHandle mHandle, int idx);

/**
 * @brief Delete all data in the list
 * @param mHandle Indicates the list handle
 *
 */
void LinkListClear(ListHandle mHandle);

/**
 * @brief Delete the list
 * @param mHandle Indicates the list handle
 *
 */
void LinkListDestroy(ListHandle mHandle);

#ifdef  __cplusplus
}
#endif
#endif  /* __LINK_LIST_H__ */
