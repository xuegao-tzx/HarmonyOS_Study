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
export class DigitalConstants {
  /**
   * 应用包名
   */
  static BundleName: string = 'com.example.distschedule'

  /**
   * 在线状态
   */
  static OnlineState: string = 'ONLINE'

  /**
   * 设备类型映射，从typeID映射为serviceID,productID，以及选中和未选中的图片资源
   * 后续服务端应该提供接口自动获取
   */
  static DeviceTypeMap = new Map([
    // 灯
    [1, {
          serviceId: "SmartLamp",
          productId: "6128c7b60ad1ed0286680f19",
          onLineImage: $r("app.media.icon_fraction_lamp_online"),
          offLineImage: $r("app.media.icon_fraction_lamp_offline")
        }],
    // 扫地机器人
    [2, {
          serviceId: "CleanData",
          productId: "61371e952cce4f0286262229",
          onLineImage: $r("app.media.icon_fraction_robot_online"),
          offLineImage: $r("app.media.icon_fraction_robot_offline")
        }],
    // 护花使者
    [3, {
          serviceId: "SmartWatering",
          productId: "6135e8fc2cce4f028625ccc9",
          onLineImage: $r("app.media.icon_fraction_watering_online"),
          offLineImage: $r("app.media.icon_fraction_watering_offline")
        }],
    // 窗帘
    [4, {
          serviceId: "SmartCurtain",
          productId: "6136ceba0ad1ed02866fa3b2",
          onLineImage: $r("app.media.icon_fraction_curtain_online"),
          offLineImage: $r("app.media.icon_fraction_curtain_offline")
        }],
    // 手机
    [5, {
          serviceId: "smartPhone",
          productId: "00000",
          onLineImage: $r("app.media.icon_fraction_phone_online"),
          offLineImage: $r("app.media.icon_fraction_phone_online")
        }],
    // 手表
    [6, {
          serviceId: "smartWatch",
          productId: "00000",
          onLineImage: $r("app.media.icon_fraction_watch2_online"),
          offLineImage: $r("app.media.icon_fraction_watch2_online")
        }],
    // 平板
    [7, {
          serviceId: "smartTablet",
          productId: "00000",
          onLineImage: $r("app.media.icon_fraction_pad_online"),
          offLineImage: $r("app.media.icon_fraction_pad_online")
        }],
    // 烟感
    [8, {
          serviceId: "SmartSmokeSensation",
          productId: "6128bdd12cce4f02861e6d98",
          onLineImage: $r("app.media.icon_fraction_smoke_online"),
          offLineImage: $r("app.media.icon_fraction_smoke_offline")
        }],
    // 可燃气体
    [9, {
          serviceId: "GasDetection",
          productId: "61445c41f74139027d2233e5",
          onLineImage: $r("app.media.icon_fraction_gas_online"),
          offLineImage: $r("app.media.icon_fraction_gas_offline")
        }],
    // 甲醛检测
    [10, {
           serviceId: "TVOCDetection",
           productId: "61445cdadcbb87027db095d1",
           onLineImage: $r("app.media.icon_fraction_formaldehyde_on"),
           offLineImage: $r("app.media.icon_fraction_formaldehyde_offline")
         }],
    // 智能垃圾桶
    [11, {
           serviceId: "Trashcan",
           productId: "614456cff74139027d2230ec",
           onLineImage: $r("app.media.icon_fraction_trash_online"),
           offLineImage: $r("app.media.icon_fraction_trash_offline")
         }],
    // 儿童手表
    [12, {
           serviceId: "SmartWatch",
           productId: "613ad28b0109930287315230",
           onLineImage: $r("app.media.icon_fraction_watch_online"),
           offLineImage: $r("app.media.icon_fraction_watch_offline")
         }],
    // 电风扇
    [13, {
           serviceId: "SmartFan",
           productId: "6150601d88056b027dd2ca47",
           onLineImage: $r("app.media.icon_fraction_fan_online"),
           offLineImage: $r("app.media.icon_fraction_fan_offline")
         }]
  ])
}