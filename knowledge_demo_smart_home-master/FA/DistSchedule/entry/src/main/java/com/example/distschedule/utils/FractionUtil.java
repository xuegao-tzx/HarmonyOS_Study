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

package com.example.distschedule.utils;

import ohos.aafwk.ability.Ability;
import ohos.aafwk.ability.fraction.Fraction;
import ohos.aafwk.ability.fraction.FractionAbility;
import ohos.aafwk.ability.fraction.FractionManager;

import java.util.Optional;

/**
 * FractionUtil 工具类
 *
 * @since 2021-08-28
 */
public class FractionUtil {

    private FractionUtil() {

    }

    public static FractionManager getFractionManager(Ability ability) {
        FractionAbility fractionAbility = (FractionAbility) ability;
        return fractionAbility.getFractionManager();
    }

    /**
     * 替换Fraction
     *
     * @param ability    ability
     * @param resourceId resourceId
     * @param fraction   fraction
     */
    public static void replaceFraction(Ability ability, int resourceId, Fraction fraction) {
        getFractionManager(ability).startFractionScheduler().replace(resourceId, fraction).submit();
    }

    public static void replaceFraction(FractionAbility fractionAbility, int resourceId, Fraction fraction) {
        fractionAbility.getFractionManager().startFractionScheduler().replace(resourceId, fraction).submit();
    }

    public static void pushFraction(Ability ability, int resourceId, Fraction fraction, String tag) {
        getFractionManager(ability).startFractionScheduler().add(resourceId,
                fraction, fraction.getClass().getSimpleName()).pushIntoStack(tag).submit();
    }

    public static void popFraction(Ability ability, String tag) {
        getFractionManager(ability).popFromStack(tag, 0);
    }

    public static Optional<Fraction> getFractionByTag(Ability ability, String tag) {
        return getFractionManager(ability).getFractionByTag(tag);
    }

    public static void popFraction(Ability ability) {
        getFractionManager(ability).popFromStack();
    }

}
