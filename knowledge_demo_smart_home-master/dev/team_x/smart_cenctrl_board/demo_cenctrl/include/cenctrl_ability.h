/*
* Copyright(c) 2021 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the license is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permisssions and
* limitations under the License.
*/

#ifndef _OHOS_CENCTRL_ABILITY_H_
#define _OHOS_CENCTRL_ABILITY_H_

#include <ability_loader.h>

namespace OHOS {
class CenctrlAbility : public Ability {
protected:
    void OnStart(const Want& want) override;
    void OnInactive() override;
    void OnActive(const Want& want) override;
    void OnBackground() override;
    void OnStop() override;
};
}   // namespace OHOS
#endif