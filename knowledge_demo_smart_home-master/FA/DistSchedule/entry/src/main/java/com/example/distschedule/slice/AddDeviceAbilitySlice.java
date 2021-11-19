/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

package com.example.distschedule.slice;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import com.example.distschedule.ResourceTable;
import com.example.distschedule.bean.DeviceResult;
import com.example.distschedule.bean.ProductResult;
import com.example.distschedule.bean.RespondBase;
import com.example.distschedule.rx2.OKHttpUtilsRx2;
import com.example.distschedule.toast.XToastUtils;
import com.example.distschedule.utils.CommonUtil;
import com.example.distschedule.utils.NFCData;
import com.example.distschedule.utils.PreferencesUtil;
import com.example.distschedule.utils.TLVUtil;
import com.socks.library.KLog;
import io.reactivex.disposables.Disposable;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.aafwk.content.Operation;
import ohos.utils.zson.ZSONObject;
import okhttp3.MediaType;
import okhttp3.RequestBody;
import org.apache.commons.lang3.StringUtils;

import java.util.Objects;

/**
 * 添加设备页面
 *
 * @since 2021-08-28
 */
public class AddDeviceAbilitySlice extends AbilitySlice {
    public int typeId;
    public NFCData nfcData;

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_add_device);
        String paramsFormJS = (String) intent.getParams().getParam("__startParams");
        if (!StringUtils.isEmpty(paramsFormJS)) {
            JSONObject jsonObject = JSONObject.parseObject(paramsFormJS);
            String deviceId = (String) jsonObject.get("deviceId");
            if (!StringUtils.isEmpty(deviceId)) {
                CommonUtil.judgeDevice(getContext(), deviceId, new Intent());
                terminateAbility();
            }
        } else {
            getNfcInfo();
            initComponents();
        }
    }

    private void initComponents() {
        findComponentById(ResourceTable.Id_add_device_img_back).setClickedListener(component -> goBackHome());
        findComponentById(ResourceTable.Id_add_text_tips).setClickedListener(component -> getNfcInfo());
    }

    @Override
    public void onActive() {
        super.onActive();

    }

    @Override
    public void onForeground(Intent intent) {
        super.onForeground(intent);
    }

    /**
     * 获取nfc数据
     */
    private void getNfcInfo() {
        Object businessInfo = Objects.requireNonNull(getAbility().getIntent().getParams()).getParam("businessInfo");
        if (businessInfo != null) {
            ZSONObject businessInfoZSON = ZSONObject.classToZSON(businessInfo);
            // data91 is your device sn in nfc tag.
            String data91 = businessInfoZSON.getZSONObject("params").getString("91");
            nfcData = TLVUtil.getNFCData(data91);
            // 从NFC贴纸读取
            KLog.i("deviceAPSsid：" + nfcData.getApSSID());
            getDeviceInfo();
        }
    }

    private void getDeviceInfo() {
        String deviceId = nfcData.getDeviceId();
        String userId = PreferencesUtil.getUserId(getContext());
        OKHttpUtilsRx2.INSTANCE.getApi().getDeviceInfo(deviceId, userId)
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase<DeviceResult>>() {
                    @Override
                    public void onNext(RespondBase<DeviceResult> deviceInfo) {
                        if (deviceInfo.isSuccess() && deviceInfo.getResult().isOnline()) {
                            typeId = deviceInfo.getResult().getTypeId();
                            CommonUtil.judgeDevice(getContext(), nfcData.getDeviceId(), new Intent());
                        } else if (deviceInfo.isSuccess()) {
                            toNetConfig();
                        } else {
                            getDefaultName(nfcData.getProductId());
                        }
                    }

                    @Override
                    public void onError(Throwable throwable) {
                        super.onError(throwable);
                    }

                    @Override
                    public void onComplete() {
                        super.onComplete();
                    }

                    @Override
                    protected Object clone() throws CloneNotSupportedException {
                        return super.clone();
                    }

                    @Override
                    public void onSubscribe(Disposable disposable) {
                        super.onSubscribe(disposable);
                        getContext().getUITaskDispatcher().asyncDispatch(() -> {
                            XToastUtils.info("查询中...");
                        });
                    }
                });
    }

    /**
     * 获取品类名字作为默认名字
     *
     * @param productId 品类ID
     */
    private void getDefaultName(String productId) {
        OKHttpUtilsRx2.INSTANCE.getApi().getNameByProId(productId)
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase<ProductResult>>() {
                    @Override
                    public void onNext(RespondBase<ProductResult> respond) {
                        super.onNext(respond);
                        KLog.i(respond);
                        if (respond.isSuccess()) {
                            typeId = respond.getResult().getId();
                            addDevice(typeId, respond.getResult().getName());
                        }
                    }
                });
    }

    private void addDevice(int typeId, String name) {
        JSONObject body = new JSONObject();
        body.put("id", nfcData.getDeviceId());
        body.put("name", name);
        body.put("ownerId", PreferencesUtil.getUserId(getContext()));
        body.put("secret", nfcData.getDevicePwd());
        body.put("typeId", typeId);
        KLog.i("OKHttpUtilsRx2", body);
        OKHttpUtilsRx2.INSTANCE.getApi().addDevice(RequestBody.create(MediaType.parse("application/json;charset=UTF-8"), JSON.toJSONString(body)))
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase>() {
                    @Override
                    public void onNext(RespondBase respond) {
                        super.onNext(respond);
                        KLog.i(respond);
                        if (respond.isSuccess()) {
                            getContext().getUITaskDispatcher().asyncDispatch(() -> {
                                XToastUtils.info("添加设备成功");
                            });
                            toNetConfig();
                        }
                    }
                });
    }

    private void toNetConfig() {
        String sessionId = getAbility().getIntent().getStringParam("nanSessionId");
        XToastUtils.info(sessionId);
        Intent intent = new Intent();
        Operation operation =
                new Intent.OperationBuilder()
                        .withDeviceId("")
                        .withBundleName(getBundleName())
                        .withAbilityName("com.example.netconfig.MainAbility")
                        .build();
        intent.setOperation(operation);
        intent.setParam("targetDeviceId", nfcData.getApSSID());
        intent.setParam("sessionId", sessionId);
        intent.setParam("deviceId", nfcData.getDeviceId());
        startAbility(intent);
        terminateAbility();
    }

    private void goBackHome() {
        onBackPressed();
    }
}
