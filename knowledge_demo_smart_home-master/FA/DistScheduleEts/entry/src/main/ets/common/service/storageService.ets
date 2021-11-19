/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
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

import {DigitalConstants} from '../constants'
import {DistScheduleService} from './distScheduleService'
import data_storage from '@ohos.data.storage';
/**
 * 数字管家轻量级存储类
 */
export class StorageService {
  static store = data_storage.getStorageSync('/data/data/' + DigitalConstants.BundleName + '/q' + DigitalConstants.BundleName);
  static deviceTypeCategoryMap = new Map();

  static getUserIdSync(): string {
    return StorageService.store.getSync("userId", "").toString();
  }

  static putUserIdSync(value: string) {
    StorageService.store.putSync("userId", value);
    StorageService.store.flushSync();
  }

  static getUserNameSync(): string {
    return StorageService.store.getSync("userName", "").toString();
  }

  static putUserNameSync(value: string) {
    StorageService.store.putSync("userName", value);
    StorageService.store.flushSync();
  }

  static getUserPhoneSync(): string {
    return StorageService.store.getSync("userPhone", "").toString();
  }

  static putUserPhoneSync(value: string) {
    StorageService.store.putSync("userPhone", value);
    StorageService.store.flushSync();
  }

  static async getDeviceCategory(type: number) {
    if (StorageService.deviceTypeCategoryMap.size == 0) {
      let deviceTypes = await DistScheduleService.getDeviceTypes()
      for (let deviceType of deviceTypes) {
        StorageService.deviceTypeCategoryMap.set(deviceType.id, deviceType.category)
      }
    }
    return StorageService.deviceTypeCategoryMap.get(type)
  }
}