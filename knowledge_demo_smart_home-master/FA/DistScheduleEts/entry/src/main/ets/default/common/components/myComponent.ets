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
import router from '@system.router';

/**
 * 首页 “我的”tab子页面
 */
@Component
export struct MyComponent {
  private familyName: string = "xTeam"
  @Prop userName: string
  @Prop userPhone: string
  @Prop userId: string
/**
   * 列表项
   */
  @Builder MyComponentItem(itemImage:Resource, itemName:Resource) {
    Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center }) {
      Image(itemImage).width(24).height(24).margin({ right: 10 })
      Text(itemName).fontSize(12).flexGrow(1)
      Image($r("app.media.icon_main_right")).width(16).height(16).objectFit(ImageFit.Contain)
    }.height(60)
  }

  build() {
    Flex({ direction: FlexDirection.Column, justifyContent: FlexAlign.Start }) {
      // 头部header，跳转注销页面
      Flex({ direction: FlexDirection.Row }) {
        Row() {
          Image($r("app.media.icon_touxiang_papa")).width(48).height(48)
          Column() {
            Text(this.userName).fontSize(14)
            Text($r("app.string.totalDevice", "1")).fontSize(12).fontColor('#606162')
          }.margin({ left: 10 })
        }.flexGrow(1)

        Row() {
          Image($r("app.media.icon_qr_code")).width(16).height(16).margin({ right: 5 }).objectFit(ImageFit.Contain)
          Image($r("app.media.icon_main_right")).width(16).height(16).objectFit(ImageFit.Contain)
        }.onClick(() => {
          router.push({
            uri: 'pages/personDetail',
          })
        })
      }.width('100%').height(60)

      // 我的家庭
      Flex({ direction: FlexDirection.Column }) {
        Text($r("app.string.myFamily", "1")).fontSize(14).height(30)
        Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center }) {
          Text(this.familyName).fontSize(12).flexGrow(1)
          Image($r("app.media.icon_main_right")).width(16).height(16).objectFit(ImageFit.Contain)
        }
        .width('100%')
        .height(90)
        .backgroundColor(Color.White)
        .padding({ left: 8, right: 8 })
        .margin({ top: 10 })
        .borderRadius(10)
      }.margin({ top: 20 }).height(120)

      // 生活服务
      Flex({ direction: FlexDirection.Column }) {
        Text($r("app.string.lifeService")).fontSize(14).height(30)
        Flex({ direction: FlexDirection.Column }) {
          this.MyComponentItem($r("app.media.icon_myzhangben"), $r("app.string.myAccountBook"))
        }
        .width('100%')
        .height(60)
        .backgroundColor(Color.White)
        .padding({ left: 8, right: 8 })
        .margin({ top: 10 })
        .borderRadius(10)
      }.margin({ top: 20 }).height(110)

      // 监控出行
      Flex({ direction: FlexDirection.Column }) {
        Text($r("app.string.healthTrip")).fontSize(14).height(30)
        Flex({ direction: FlexDirection.Column }) {
          this.MyComponentItem($r("app.media.icon_mybus"), $r("app.string.myBusCard"))
          this.MyComponentItem($r("app.media.icon_my_health_code"), $r("app.string.myQrCode"))
          this.MyComponentItem($r("app.media.icon_mycar"), $r("app.string.myCar"))
        }
        .width('100%')
        .height(180)
        .backgroundColor(Color.White)
        .padding({ left: 8, right: 8 })
        .margin({ top: 10 })
        .borderRadius(10)
      }.margin({ top: 20 }).height(210)
    }
    .width('100%').height('100%')
    .padding({ left: 16, right: 16, top: 30 }).backgroundColor($r("app.color.backgroud_grey"))
  }
}