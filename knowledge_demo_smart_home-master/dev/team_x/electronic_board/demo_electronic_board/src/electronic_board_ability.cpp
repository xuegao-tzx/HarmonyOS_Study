/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
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

#include "electronic_board_ability.h"

namespace OHOS {
REGISTER_AA(ElectronicBoardAbility)

void ElectronicBoardAbility::OnStart(const Want &want)
{
    SetMainRoute("ElectrBoardAbilitySlice");
    Ability::OnStart(want);
}

void ElectronicBoardAbility::OnInactive()
{
    Ability::OnInactive();
}

void ElectronicBoardAbility::OnActive(const Want &want)
{
    Ability::OnActive(want);
}

void ElectronicBoardAbility::OnBackground()
{
    Ability::OnBackground();
}

void ElectronicBoardAbility::OnStop()
{
    Ability::OnStop();
}
}

