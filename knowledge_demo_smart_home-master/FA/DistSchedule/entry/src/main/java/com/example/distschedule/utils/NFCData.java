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

import org.apache.commons.lang3.StringUtils;

public class NFCData {
    private TLV productId;
    private TLV nodeId;
    private TLV devicePwd;
    private TLV netTag;
    private TLV apSSID;
    private TLV apToken;
    private TLV BLEMAC;

    public String getDeviceId() {
        if (StringUtils.isEmpty(getProductId()) || StringUtils.isEmpty(getNodeId())) {
            return "";
        } else {
            return getProductId() + "_" + getNodeId();
        }
    }

    public String getProductId() {
        return productId.getValue();
    }

    public void setProductId(TLV productId) {
        this.productId = productId;
    }

    public String getNodeId() {
        return nodeId.getValue();
    }

    public void setNodeId(TLV nodeId) {
        this.nodeId = nodeId;
    }

    public String getDevicePwd() {
        return devicePwd.getValue();
    }

    public void setDevicePwd(TLV devicePwd) {
        this.devicePwd = devicePwd;
    }

    public String getNetTag() {
        return netTag.getValue();
    }

    public void setNetTag(TLV netTag) {
        this.netTag = netTag;
    }

    public String getApSSID() {
        return apSSID.getValue();
    }

    public void setApSSID(TLV apSSID) {
        this.apSSID = apSSID;
    }

    public String getApToken() {
        return apToken.getValue();
    }

    public void setApToken(TLV apToken) {
        this.apToken = apToken;
    }

    public String getBLEMAC() {
        return BLEMAC.getValue();
    }

    public void setBLEMAC(TLV BLEMAC) {
        this.BLEMAC = BLEMAC;
    }
}
