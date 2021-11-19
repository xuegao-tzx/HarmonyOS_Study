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
import {Core, ExpectExtend, InstrumentLog} from "deccjsunit/index"
import testsuite from "../../../test/List.test.ets"
import app from '@system.app'
import featureAbility from "@ohos.ability.featureAbility"

@Entry
@Component
struct MyComponent {
  aboutToAppear() {
    console.info("start run testcase!!!!")
    featureAbility.getWant()
      .then((Want) => {
        const core = Core.getInstance()
        const instrumentLog = new InstrumentLog({
          'id': 'report', 'unity': 'true'
        })
        const expectExtend = new ExpectExtend({
          'id': 'extend'
        })
        core.addService('expect', expectExtend)
        core.addService('report', instrumentLog)
        core.init()
        core.subscribeEvent('spec', instrumentLog)
        core.subscribeEvent('suite', instrumentLog)
        core.subscribeEvent('task', instrumentLog)
        const configService = core.getDefaultService('config')
        configService.setConfig(Want.parameters)
        testsuite()
        core.execute()
        console.info('Operation successful. Data: ' + JSON.stringify(Want));
      })
      .catch((error) => {
        console.error('Operation failed. Cause: ' + JSON.stringify(error));
      })
  }

  build() {
    Flex({
      direction: FlexDirection.Column,
      alignItems: ItemAlign.Center,
      justifyContent: FlexAlign.Center
    }) {
      Text('Hello World')
        .fontSize(50)
        .fontWeight(FontWeight.Bold)
    }
    .width('100%')
    .height('100%')
  }
}

