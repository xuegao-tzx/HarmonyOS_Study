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

package com.example.distschedule;

import com.example.distschedule.slice.MainAbilitySlice;
import ohos.aafwk.ability.IAbilityConnection;
import ohos.aafwk.ability.fraction.FractionAbility;
import ohos.aafwk.content.Intent;
import ohos.aafwk.content.Operation;
import ohos.bundle.ElementName;
import ohos.rpc.IRemoteObject;

import java.util.ArrayList;
import java.util.List;

public class MainAbility extends FractionAbility {
    private final String[] permission = {"ohos.permission.LOCATION", "ohos.permission.CAMERA", "ohos.permission.READ_MEDIA"};

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setMainRoute(MainAbilitySlice.class.getName());
        askPermission();
        startService();
    }


    private void askPermission() {
        List<String> permissionList = new ArrayList<>();
        for (String s : permission) {
            if (verifySelfPermission(s) != 0 && canRequestPermission(s)) {
                permissionList.add(s);
            }
        }
        if (permissionList.size() > 0) {
            requestPermissionsFromUser(permissionList.toArray(new String[0]), 0);
        }
    }

    private void startService() {
        Intent intent = new Intent();
        Operation operation = new Intent.OperationBuilder()
                .withDeviceId("")
                .withBundleName(getBundleName())
                .withAbilityName(ServiceAbility.class.getName())
                .build();
        intent.setOperation(operation);
        connectAbility(intent, new IAbilityConnection() {
            @Override
            public void onAbilityConnectDone(ElementName elementName, IRemoteObject iRemoteObject, int i) {
                if (iRemoteObject instanceof ServiceAbility.CurrentRemoteObject) {
                    ((ServiceAbility.CurrentRemoteObject) iRemoteObject).test();
                }
            }

            @Override
            public void onAbilityDisconnectDone(ElementName elementName, int i) {

            }
        });
    }

    private void stopService() {
        Intent intent = new Intent();
        Operation operation = new Intent.OperationBuilder()
                .withDeviceId("")
                .withBundleName(getBundleName())
                .withAbilityName(ServiceAbility.class.getName())
                .build();
        intent.setOperation(operation);
        stopAbility(intent);
    }
}
