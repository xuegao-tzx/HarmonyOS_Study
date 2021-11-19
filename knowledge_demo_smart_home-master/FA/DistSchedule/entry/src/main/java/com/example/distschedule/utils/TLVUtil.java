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

import java.util.Base64;

public class TLVUtil {
    private static final int TAG_LENGTH = 1;
    private static final int LEN_LENGTH = 2;
    private static final int TAG_PRODUCT_ID = 1;
    private static final int TAG_NODE_ID = 2;
    private static final int TAG_DEVICE_PWD = 3;
    private static final int TAG_NET_TAG = 4;
    private static final int TAG_AP_SSID = 5;
    private static final int TAG_AP_TOKEN = 6;
    private static final int TAG_BLE_MAC = 7;

    private static String base64Decoder(String data91) {
        Base64.Decoder decoder = Base64.getDecoder();
        byte[] bytes = decoder.decode(data91);
        StringBuilder stringBuilder = new StringBuilder();
        for (byte byteData : bytes) {
            stringBuilder.append((char) byteData);
        }
        return stringBuilder.toString();
    }

    private static TLV getTLV(String string, int position) {
        int tag = Integer.parseInt(StringUtils.substring(string, position, position + TAG_LENGTH));
        int length = Integer.parseInt(StringUtils.substring(string, position + TAG_LENGTH, position + TAG_LENGTH + LEN_LENGTH));
        String value = StringUtils.substring(string, position + TAG_LENGTH + LEN_LENGTH, position + TAG_LENGTH + LEN_LENGTH + length);
        return new TLV(tag, length, value, TAG_LENGTH + LEN_LENGTH + length);
    }

    /**
     * 返回NFC数据
     *
     * @param base64String nfc自定义数据字符串
     * @return 返回NFC数据
     */
    public static NFCData getNFCData(String base64String) {
        NFCData nfcData = new NFCData();
        int position = 0;
        String string = base64Decoder(base64String);
        while (position + TAG_LENGTH + LEN_LENGTH < string.length()) {
            TLV tlv = getTLV(string, position);
            setNFCData(nfcData, tlv);
            position += tlv.getTLVLength();
        }
        return nfcData;
    }

    private static void setNFCData(NFCData nfcData, TLV tlv) {
        switch (tlv.getTag()) {
            case TAG_PRODUCT_ID: {
                nfcData.setProductId(tlv);
                break;
            }
            case TAG_NODE_ID: {
                nfcData.setNodeId(tlv);
                break;
            }
            case TAG_DEVICE_PWD: {
                nfcData.setDevicePwd(tlv);
                break;
            }
            case TAG_NET_TAG: {
                nfcData.setNetTag(tlv);
                break;
            }
            case TAG_AP_SSID: {
                nfcData.setApSSID(tlv);
                break;
            }
            case TAG_AP_TOKEN: {
                nfcData.setApToken(tlv);
                break;
            }
            case TAG_BLE_MAC: {
                nfcData.setBLEMAC(tlv);
                break;
            }
        }
    }
}
