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
import {DigitalCommonUtil} from '../../common/digitalCommonUtil'
import {StorageService} from '../../common/service/storageService'
import {DigitalConstants} from '../../common/constants'
import featureAbility from '@ohos.ability.featureAbility';
import router from '@system.router'

/**
 * 个人详细界面
 */
@Preview
@Entry
@Component
struct PersonDetail {
  private userName: string = ''

  build() {
    Column() {
      Flex({ alignItems: ItemAlign.Center, justifyContent: FlexAlign.Start }) {
        Image($r("app.media.icon_back")).width(24).height(24).margin({ left: 18, right: 18 })
          .onClick(() => {
            router.back()
          })
        Text($r("app.string.personDetail")).fontSize(18)
      }.height(56).width('100%').backgroundColor(Color.White)

      Column() {
        Flex({ direction: FlexDirection.Column, justifyContent: FlexAlign.Start }) {

          Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center }) {
            Text($r("app.string.myPicture")).fontSize(14).flexGrow(1)
            Image($r("app.media.icon_touxiang_papa")).width(24).height(24).margin({ right: 10 })
            Image($r("app.media.icon_main_right")).width(16).height(16).objectFit(ImageFit.Contain)
          }.height(30)

          Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center }) {
            Text($r("app.string.myAccountName")).fontSize(14).flexGrow(1)
            Text(this.userName).fontSize(14).margin({ right: 10 })
            Image($r("app.media.icon_main_right")).width(16).height(16).objectFit(ImageFit.Contain)
          }.height(30).padding({top:5})

          Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center }) {
            Text($r("app.string.myQrCode")).fontSize(14).flexGrow(1)
            Image($r("app.media.icon_qr_code")).width(16).height(16).margin({ right: 10 })
            Image($r("app.media.icon_main_right")).width(16).height(16).objectFit(ImageFit.Contain)
          }.height(30).padding({top:5})
        }
        .width('100%')
        .height(120)
        .backgroundColor(Color.White)
        .borderRadius(10)
        .padding({top:20,left:16,right:16})
      }.padding({ top: 20, left: 16, right: 16 })

      Column() {
        Text($r("app.string.userLogout"))
          .fontSize(14)
          .fontColor(Color.Red)
          .textAlign(TextAlign.Center)
          .width('100%')
          .height(50)
          .backgroundColor(Color.White)
          .borderRadius(10)
      }.padding({ top: 15, left: 16, right: 16 }).onClick(() => {
        this.logout()
      })
    }
    .width('100%').height('100%')
    .backgroundColor($r("app.color.backgroud_grey"))
  }

/**
   * 注销账号
   */
  logout() {
    StorageService.putUserIdSync('')
    StorageService.putUserNameSync('')
    StorageService.putUserPhoneSync('')
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

  aboutToAppear() {
    this.userName = StorageService.getUserNameSync()
  }
}