// @ts-nocheck
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
import {DigitalConstants} from '../../common/constants'
import {DistScheduleService} from '../../common/service/distScheduleService'
import {StorageService} from '../../common/service/storageService'
import {DigitalCommonUtil} from '../../common/digitalCommonUtil'

/**
 * 数字管家手机登录页面
 */
@Entry
@Component
struct Index {
  private userId: string = ""
  private userPhone: string = ''

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      // logo,名称，版本
      Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center }) {
        Text('CPlogo')
          .margin({ top: 100 })
          .padding({ top: 30, bottom: 30, left: 10, right: 10 })
          .fontSize(25)
          .fontColor(Color.Grey)
          .borderStyle(BorderStyle.Solid)
          .borderWidth(1)
          .borderColor(Color.Grey)
          .borderRadius(20)
        Text($r("app.string.entry_MainAbility")).fontSize(20).fontWeight(600).margin({ top: 40 })
        Text($r("app.string.application_version")).fontSize(20).margin({ top: 10 })
      }
      .width('100%').height('100%').layoutWeight(3)

      // 手机输入框，登录
      Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center }) {

        TextInput({ placeholder: '', text: this.userPhone })
          .type(InputType.Normal)
          .placeholderColor(Color.Red)
          .placeholderFont({ size: 50, weight: 2 })
          .enterKeyType(EnterKeyType.Search)
          .caretColor(Color.Blue)
          .height(40)
          .borderRadius(20)
          .width(250)
          .backgroundColor($r("app.color.backgroud_grey"))
          .onChange((value: string) => {
            this.userPhone = value
          })

        Button("登录").width(250).margin({ top: 10 }).onClick(() => {
          console.info("hahha:" + this.userPhone)
          this.login()
        })
        Flex({ direction: FlexDirection.Row, justifyContent: FlexAlign.Center }) {
          Image($r("app.media.login_selected")).objectFit(ImageFit.None).width(9).height(9)
          Text($r("app.string.login_prove")).margin({ left: 5 })
          Text($r("app.string.user_privacy")).fontColor(Color.Blue)
        }.margin({ top: 10 })
      }
      .width('100%').height('100%').layoutWeight(2)
    }
    .width('100%')
    .height('100%')
    .backgroundColor($r("app.color.backgroud_grey"))
  }

/**
   * 用户登录，保存用户信息
   */
  async login() {
    let reg = new RegExp(/^\d{11}$/g);
    if (reg.test(this.userPhone)) {
      // 获取用户id
      let userInfo = await DistScheduleService.getUserInfo(this.userPhone)
        .catch(error => {
          AlertDialog.show(
            {
              title: $r("app.string.warning"),
              message: error,
              confirm: {
                value: $r("app.string.comfirm"),
                action: () => {
                  //console.info('Button-clicking callback')
                }
              }
            })
          return
        })
      // 保存用户id,名称,手机号
      DigitalCommonUtil.log(userInfo.id)
      StorageService.putUserIdSync(userInfo.id)
      StorageService.putUserNameSync(userInfo.name)
      StorageService.putUserPhoneSync(userInfo.phone)

      // 当前机器storage有时跨ability无法保存，暂用参数传递
      let params = {}
      params["userId"] = userInfo.id
      params["userName"] = userInfo.name
      params["userPhone"] = userInfo.phone

      var str = {
        "want": {
          "deviceId": "",
          "bundleName": DigitalConstants.BundleName,
          "abilityName": "com.example.distschedule.MainAbility",
          "uri": "",
          "type": "",
          "action": "",
          "parameters": params,
          "entities": [
            "entity.com.example.mytest.ENTITIES"
          ]
        },
        "abilityStartSetting": {}
      };
      featureAbility.startAbilityForResult(str, (error, data) => {
        if (error) {
          console.error('Operation failed. Cause: ' + error);
          return;
        }
        console.info('Operation succeeded: ' + data);
      });
    } else {
      AlertDialog.show(
        {
          title: $r("app.string.warning"),
          message: $r("app.string.pleaseInputPhone"),
          confirm: {
            value: $r("app.string.comfirm"),
            action: () => {
              //console.info('Button-clicking callback')
            }
          }
        }
      )
    }
  }

  aboutToAppear() {
    DigitalCommonUtil.log("userId=" + StorageService.getUserIdSync())
  }
}