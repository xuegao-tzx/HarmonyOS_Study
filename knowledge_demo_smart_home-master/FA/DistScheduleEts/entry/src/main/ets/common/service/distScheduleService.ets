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
import http from '@ohos.net.http';
import prompt from '@system.prompt';

export enum RequestMethod {
  OPTIONS = "OPTIONS",
  GET = "GET",
  HEAD = "HEAD",
  POST = "POST",
  PUT = "PUT",
  DELETE = "DELETE",
  TRACE = "TRACE",
  CONNECT = "CONNECT"
}

/**
 * 数字管家服务端调用类
 */
export class DistScheduleService {
  static distScheduleServer: string = "http://81.71.17.188:8080/distschedule-api"
  static readTimeout: number = 60000
  static connectTimeout: number = 60000

  /**
   * 根据手机号，获取用户信息
   * @param phone 手机号
   */
  static async getUserInfo(phone: string) {
    let httpRequest = http.createHttp();

    let data = await httpRequest.request(
      `${DistScheduleService.distScheduleServer}/user/phone/${phone}`,
      {
        method: RequestMethod.GET,
        header: {
          'Content-Type': 'application/json'
        },
        readTimeout: DistScheduleService.readTimeout,
        connectTimeout: DistScheduleService.connectTimeout
      }
    );

    // @ts-ignore
    let result = JSON.parse(data.result)

    if (result.success) {
      return result.result
    }
    throw new Error(result.message)
  }

  /**
   * 根据用户id，获取设备列表
   * @param phone 手机号
   */
  static async getDeviceListByUserId(userId: string) {
    let httpRequest = http.createHttp();

    let data = await httpRequest.request(
      `${DistScheduleService.distScheduleServer}/device/userId/${userId}`,
      {
        method: RequestMethod.GET,
        header: {
          'Content-Type': 'application/json'
        },
        readTimeout: DistScheduleService.readTimeout,
        connectTimeout: DistScheduleService.connectTimeout
      }
    );

    // @ts-ignore
    let result = JSON.parse(data.result)

    if (result.success) {
      return result.result
    }
    throw new Error(result.message)
  }

  /**
   * 获取设备类型
   */
  static async getDeviceTypes() {
    let httpRequest = http.createHttp();

    let data = await httpRequest.request(
      `${DistScheduleService.distScheduleServer}/devicetype`,
      {
        method: RequestMethod.GET,
        header: {
          'Content-Type': 'application/json'
        },
        readTimeout: DistScheduleService.readTimeout,
        connectTimeout: DistScheduleService.connectTimeout
      }
    );

    // @ts-ignore
    let result = JSON.parse(data.result)

    if (result.success) {
      return result.result
    }
    throw new Error(result.message)
  }
}