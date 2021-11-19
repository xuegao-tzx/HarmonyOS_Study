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
import {HomeTabs} from './homeTabs.ets'

/**
 * 首页 “主页”tab子页面
 */
@Component
export struct HomeComponent {
  @Prop userName: string
  @Prop userPhone: string
  @Prop userId: string

  /**
   * 首页头部
   */
  @Builder HomeHeader() {
    Flex({ alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceBetween }) {
      Text($r("app.string.userFamily", this.userName)).fontSize(18).margin({ left: 24 })
      Image($r("app.media.icon_add")).width(24).height(24).margin({ right: 18 })
    }.height(56).width('100%').backgroundColor(Color.White)
  }

  build() {
    Column() {
      //首页头部(用户名称 + 添加菜单)
      this.HomeHeader()

      //首页Tab页签("场景"，"设备")
      HomeTabs({ userName: this.userName, userPhone: this.userPhone, userId: this.userId })
    }
    .width('100%').height('100%').backgroundColor($r("app.color.backgroud_grey"))
  }
}