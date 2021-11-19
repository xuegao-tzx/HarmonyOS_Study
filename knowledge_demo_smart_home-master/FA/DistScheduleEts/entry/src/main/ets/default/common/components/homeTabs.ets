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
import {DigitalCommonUtil} from '../../../common/digitalCommonUtil'
import {DistScheduleService} from '../../../common/service/distScheduleService'
import {StorageService} from '../../../common/service/storageService'
import {DigitalConstants} from '../../../common/constants'
/**
 * 设备类型
 */
enum DeviceType {
  CARRY = 0,
  HOME = 1,
  ENTERTAINMENT = 2,
  WATCH = 3,
}

/**
 * 首页Tab页签("场景"，"设备")
 */
@Component
export struct HomeTabs {
  private scroller: Scroller = new Scroller()
  private controller: TabsController = new TabsController()
  @State homeTabIndex: number = 0
  @State carryDevices: any[] = [] //随身设备
  @State homeDevices: any[] = [] //家庭设备
  @State entertainmentDevices: any[] = [] //娱乐设备
  @State watchDevices: any[] = [] //监控设备
  @Prop userName: string
  @Prop userPhone: string
  @Prop userId: string

/**
   * 根据状态，返回“在线”和“不在线”
   */
  private getDeviceSateText(state: string) {
    if (state == DigitalConstants.OnlineState) {
      return $r("app.string.online")
    } else {
      return $r("app.string.offline")
    }
  }

/*
 * 返回设备图片
 */
  private getDeviceImage(device: any) {
    if (!DigitalConstants.DeviceTypeMap.has(device.typeId)) {
      return $r("app.media.icon_fraction_phone_online") // 需要做一张默认的位置设备图片
    }

    if (device.state == DigitalConstants.OnlineState) {
      return DigitalConstants.DeviceTypeMap.get(device.typeId).onLineImage
    } else {
      return DigitalConstants.DeviceTypeMap.get(device.typeId).offLineImage
    }
  }

/**
   * 显示监控设备列表，家务设备列表，娱乐设备列表，随身设备列表
   */
  @Builder shouwDeviceList(deviceTypeName: Resource, deviceList: any[]) {
    if (deviceList.length > 0) {
      Text(deviceTypeName).width('100%').fontSize(14).fontColor('ff000000').margin({ bottom: 5 })
      ForEach(deviceList, (item: any) => {
        Stack({ alignContent: Alignment.TopStart }) {
          Row() {
            Blank().layoutWeight(1)
            if (item.state == DigitalConstants.OnlineState) {
              Text(this.getDeviceSateText(DigitalConstants.OnlineState))
                .fontColor(Color.Blue)
                .fontSize(10)
                .backgroundColor('#1a0a59f7')
                .borderRadius(20)
                .padding({ left: 5, right: 5 })
            } else {
              Text(this.getDeviceSateText(''))
                .fontColor("#919191")
                .fontSize(10)
                .backgroundColor("#F2F2F2")
                .borderRadius(20)
                .padding({ left: 5, right: 5 })
            }
          }.width('100%')

          Image(this.getDeviceImage(item)).width(56).height(56).margin({ top: 20, left: 5 })
          Text(item.name).fontSize(14).margin({ left: 60, top: 30 })
          Text($r("app.string.fromDevice", item.ownerName))
            .fontSize(12)
            .fontColor('#9A9A9A')
            .margin({ left: 60, top: 50 })
        }
        .height(88)
        .width('46%')
        .backgroundColor(Color.White)
        .borderRadius(10)
        .margin({ top: 10, right: 10, bottom: 10 })
      }, item => item.id)
    }
  }

  @Builder homeTabBarContent() {
    Tabs({ barPosition: BarPosition.End, index: 0, controller: this.controller }) {
      TabContent() { //场景页签
        Stack({ alignContent: Alignment.Start }) {
          Image($r("app.media.icon_main_tabSense")).width(192).height(182)
          Text($r("app.string.familyScence")).fontSize(16).margin({ top: 100, left: 30 })
          Image($r("app.media.icon_main_right")).width(6).height(15).margin({ top: 100, left: 150 })
        }
      }

      TabContent() { //设备页签
        Scroll(this.scroller) {
          Flex({ alignItems: ItemAlign.Start, justifyContent: FlexAlign.Start, wrap: FlexWrap.Wrap }) {
            // 移动设备列表
            this.shouwDeviceList($r("app.string.carryDevice"), this.carryDevices)
            // 家务设备列表
            this.shouwDeviceList($r("app.string.homeDevice"), this.homeDevices)
            // 娱乐设备列表
            this.shouwDeviceList($r("app.string.entertaimentDevice"), this.entertainmentDevices)
            // 监控设备列表
            this.shouwDeviceList($r("app.string.watchDevice"), this.watchDevices)
          }
          .width('100%')
        }.scrollBar(BarState.Off)
      }
    }
    .onChange((index: number) => {
      this.homeTabIndex = index
    })
    .vertical(false)
    .barHeight(0)
    .width('100%')
    .scrollable(false)
    .padding({ left: 12, right: 12, top: 11 })
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.End, justifyContent: FlexAlign.End }) {
      // 页签列表
      HomeTabBar({ controller: this.controller, homeTabIndex: $homeTabIndex })
      // 页签内容
      this.homeTabBarContent()
    }.flexGrow(1)
  }

  async aboutToAppear() {
    await this.initDevices()
  }

/**
   * 获取个人设备列表
   */
  async initDevices() {
    if (this.userId != '') {
      let devices = await DistScheduleService.getDeviceListByUserId(this.userId)
      for (let device of devices) {
        let categoryId = await StorageService.getDeviceCategory(device.typeId)
        switch (categoryId) {
          case DeviceType.CARRY:
          // 当前除儿童手表（12）外，其他的随身设备状态始终是online
            if (device.typeId != 12) {
              device.state = DigitalConstants.OnlineState
            }
            this.carryDevices.push(device)
            break;
          case DeviceType.ENTERTAINMENT:
            this.entertainmentDevices.push(device)
            break;
          case DeviceType.HOME:
            this.homeDevices.push(device)
            break;
          default:
            this.watchDevices.push(device)
            break;
        }
        //DigitalCommonUtil.log("Device " + device.name + "--" + categoryId)
      }
//      DigitalCommonUtil.log("carryDevices total  " + this.carryDevices.length)
//      DigitalCommonUtil.log("entertainmentDevices total  " + this.entertainmentDevices.length)
//      DigitalCommonUtil.log("homeDevices total  " + this.homeDevices.length)
//      DigitalCommonUtil.log("watchDevices total  " + this.watchDevices.length)
    }
  }
}

export class HomeTabBarModel {
  id: number
  name: Resource

  constructor(id: number, name: Resource) {
    this.id = id
    this.name = name
  }
}

/**
 * 首页tabBar  显示“场景”和“设备”
 */
@Component
export struct HomeTabBar {
  private tabBars: HomeTabBarModel[] = [
    { id: 0, name: $r("app.string.scence") },
    { id: 1, name: $r("app.string.device") }]
  private controller: TabsController = new TabsController()
  @Link homeTabIndex: number

/**
   * 获取tab当前颜色
   */
  getTabTextColor(tabIndex: number, index: HomeTabBarModel) {
    let color = Color.Grey
    if (tabIndex === index.id) {
      color = Color.Blue
    }
    return color
  }

/**
   * 是否显示tab下划线
   */
  getTabDividerVisibility(tabIndex: number, index: HomeTabBarModel) {
    let visibility = Visibility.Hidden
    if (tabIndex === index.id) {
      visibility = Visibility.Visible
    }
    return visibility
  }

  build() {
    Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      ForEach(this.tabBars, item => {
        Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center }) {
          Text(item.name)
            .fontSize(16)
            .fontColor(this.getTabTextColor(this.homeTabIndex, item))

          Divider()
            .color(Color.Blue)
            .strokeWidth(2)
            .width(50)
            .margin({ top: 5 })
            .visibility(this.getTabDividerVisibility(this.homeTabIndex, item))

        }
        .onClick(() => {
          this.homeTabIndex = item.id
          this.controller.changeIndex(this.homeTabIndex)
        }).width(100).margin({ top: 10 })
      }, item => item.id)
    }
    .width('100%').height(56).padding({ top: 10 })
  }
}