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
import featureAbility from '@ohos.ability.featureAbility';
import router from '@system.router'
import {MainTabBarModel} from '../models/mainTabBarModel'
import {MyComponent} from './myComponent'
import {HomeComponent} from './homeComponent'
import {ScheduleComponent} from './scheduleComponent'
import {DigitalConstants} from '../../../common/constants'
import {StorageService} from '../../../common/service/storageService'
import {DigitalCommonUtil} from '../../../common/digitalCommonUtil'

function getTabSrc(tabIndex: number, index: MainTabBarModel) {
  let imgSrc = index.unSelectedResource
  if (tabIndex === index.id) {
    imgSrc = index.selectedResource
  }
  return imgSrc
}

function getTabTextColor(tabIndex: number, index: MainTabBarModel) {
  let color = Color.Grey
  if (tabIndex === index.id) {
    color = Color.Blue
  }
  return color
}

/**
 * 数字家庭首页 Tab组件（“主页”，“日程”，“我的”）
 * @param error
 * @param data
 */
@Component
export struct MainTabs {
  @State userName: string = ''
  @State userPhone: string = ''
  @State userId: string = ''
  private controller: TabsController = new TabsController()
  // 定义三个页签
  private tabBars: MainTabBarModel[] = [{
                                            id: 0,
                                            name: $r("app.string.home"),
                                            selectedResource: $r("app.media.icon_home_selected"),
                                            unSelectedResource: $r("app.media.icon_home_unselected")
                                          },
                                          {
                                            id: 1,
                                            name: $r("app.string.schedule"),
                                            selectedResource: $r("app.media.icon_calender_selected"),
                                            unSelectedResource: $r("app.media.icon_calender_unselected")
                                          },
                                          {
                                            id: 2,
                                            name: $r("app.string.my"),
                                            selectedResource: $r("app.media.icon_mine_selected"),
                                            unSelectedResource: $r("app.media.icon_mine_unselected")
                                          }]
  @State mainTabIndex: number = 0

  @Builder MainTabContent(){
    Tabs({ barPosition: BarPosition.End, index: 0, controller: this.controller }) {
      TabContent() {
        // 主页
        HomeComponent({ userName: this.userName, userPhone: this.userPhone, userId: this.userId })
      }

      TabContent() {
        // 日程
        ScheduleComponent()
      }

      TabContent() {
        // 我的
        MyComponent({ userName: this.userName, userPhone: this.userPhone, userId: this.userId })
      }
    }
    .onChange((index: number) => {
      this.mainTabIndex = index
    })
    .vertical(false)
    .barHeight(0)
    .width('100%')
    .scrollable(false)
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.End, justifyContent: FlexAlign.End }) {
      // Tab页签内容
      this.MainTabContent()

      // Tab页签导航条：主页，日程，我的
      MainTabBar({ controller: this.controller, mainTabIndex: $mainTabIndex, tabBars: this.tabBars })
    }
  }

  async checkHasLogin(){
    this.userId = StorageService.getUserIdSync()
    this.userName = StorageService.getUserNameSync()
    this.userPhone = StorageService.getUserPhoneSync()
    if (this.userId == '' && router.getParams() != null && router.getParams().userId != null) {
      DigitalCommonUtil.log("load userId from router params:" + this.userId)
      this.userId = router.getParams().userId
      StorageService.putUserIdSync(this.userId)
    }
    if (this.userName == '' && router.getParams() != null && router.getParams().userName != null) {
      this.userName = router.getParams().userName
      StorageService.putUserNameSync(this.userName)
    }
    if (this.userPhone == '' && router.getParams() != null && router.getParams().userPhone != null) {
      this.userPhone = router.getParams().userPhone
      StorageService.putUserPhoneSync(this.userPhone)
    }

    DigitalCommonUtil.log("userId = " + this.userId)

    // 未登录，跳转到登录页面
    if (this.userId == '') {
      var str = {
        "want": {
          "deviceId": "",
          "bundleName": DigitalConstants.BundleName,
          "abilityName": "com.example.distschedule.LoginAbility",
          "uri": "",
          "type": "",
          "action": "",
          "parameters": {},
          "entities": [
            "entity.com.example.mytest.ENTITIES"
          ]
        },
        "abilityStartSetting": {}
      };
      featureAbility.startAbilityForResult(str, (error, data) => {
        if (error) {
          DigitalCommonUtil.error('Operation failed. Cause: ' + error);
          return;
        }
        DigitalCommonUtil.log('Operation succeeded: ' + data);
      });
    }
  }

  async aboutToAppear() {
    this.checkHasLogin()
  }
}


@Component
export struct MainTabBar {
  private tabBars: MainTabBarModel[]
  private controller: TabsController = new TabsController()
  @Link mainTabIndex: number

  build() {
    Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceAround }) {
      ForEach(this.tabBars, item => {
        Column() {
          Image(getTabSrc(this.mainTabIndex, item))
            .objectFit(ImageFit.Contain)
            .width(24).height(24)
          Text(item.name)
            .fontSize(12)
            .fontColor(getTabTextColor(this.mainTabIndex, item))
        }
        .onClick(() => {
          this.mainTabIndex = item.id
          this.controller.changeIndex(this.mainTabIndex)
        }).layoutWeight(1)
      }, item => item.id)
    }
    .width('100%').height(56)
    .backgroundColor($r("app.color.backgroud_grey"))
  }
}