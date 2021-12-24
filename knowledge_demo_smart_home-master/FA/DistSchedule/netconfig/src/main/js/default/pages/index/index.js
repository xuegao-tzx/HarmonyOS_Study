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

import app from '@system.app';
import {getApp} from '../../common/common.js';

export default {
    data: {
        state: "等待中",
        wifiApInfo: {},
        isFail: false,
        isNAN: true,
        sessionId: '',
        targetDeviceId: '',
        deviceId: ''
    },
    onInit() {
        if (this.sessionId == 'NAN_DEVICE_NOT_FOUND') {
            getApp(this).ConfigParams.sessionId = ''
        } else {
            getApp(this).ConfigParams.sessionId = this.sessionId
        }
        console.info("sessionId:" + this.sessionId)
        this.state = this.sessionId
        this.isNAN = true
        setTimeout(() => {
            this.getWifiInfo();
        }, 100)
    },
    onHide() {
        this.disconnectDevice()
    },
    getWifiInfo() {
        this.state = '开始获取wifi列表'
        getApp(this).NetConfig.getWifiList((result) => {
            if (result.code == 0 && result.data && result.data.length > 0) {
                this.state = '获取wifi列表成功'
                this.wifiApInfo = result.data[0]
                for (let i = 0;i < result.data.length; i++) {
                    if (result.data[i].hasDefaultPassword) {
                        this.wifiApInfo = result.data[i];
                        break;
                    }
                }
                if (Object.keys(this.wifiApInfo).length == 0) {
                    this.state = "没有已连上的wifi"
                    return;
                }
                if (this.isNAN) {
                    this.discoverDeviceByNAN()
                } else {
                    this.startSoftAp()
                }
            } else {
                this.state = "获取wifi列表失败"
                this.isFail = true
            }
        });
    },
    retry() {
        if (Object.keys(this.wifiApInfo).length > 0) {
            this.isNAN = true
            this.discoverDeviceByNAN()
            //            if (this.isNAN) {
            //                this.discoverDeviceByNAN()
            //            } else {
            //                this.startSoftAp()
            //            }
        } else {
            this.getWifiInfo()
        }
    },
    discoverDeviceByNAN() {
        this.state = "开始发现设备"
        let scanInfo = {
            duration: 5,
            lockTime: 60,
            sessionId: getApp(this).ConfigParams.sessionId
        };
        // Step1 discover the device through the NaN broadcast service.
        getApp(this).NetConfig.discoveryByNAN(scanInfo, (result) => {
            if (result.code == 0) {
                this.state = "NAN发现设备成功"
                getApp(this).ConfigParams.deviceInfo = result.data;
                this.registerDisconnectCallback(getApp(this).ConfigParams.deviceInfo.sessionId);
                let connectInfo = {
                    targetDeviceId: getApp(this).ConfigParams.deviceInfo.productId,
                    type: 0,
                    pin: '11111111',
                    password: getApp(this).ConfigParams.deviceInfo.sn,
                    sessionId: getApp(this).ConfigParams.deviceInfo.sessionId
                };
                console.info("netconfig connectInfo" + JSON.stringify(connectInfo))
                this.connectDevice(connectInfo);
            } else {
                this.state = "NAN发现设备失败"
                this.startSoftAp()
            }
        });
    },
    connectDevice(connectInfo) {
        if (this.isNAN) {
            this.state = "连接设备中(NAN)"
        } else {
            this.state = "连接设备中(SoftAp)"
        }
        console.info("Netconfig connectDevice argument" + JSON.stringify(connectInfo))
        // Step2 connect the device.
        getApp(this).NetConfig.connectDevice(connectInfo, (result) => {
            if (result.code === 0) {
                this.state = "连接设备成功"
                this.configDevice();
            } else {
                console.error("netconfig connectDevice fail" + JSON.stringify(result))
                if (this.isNAN) {
                    this.state = "连接设备失败(NAN)"
                    this.startSoftAp()
                } else {
                    this.state = "连接设备失败(softAp)"
                    this.isFail = true
                    this.disconnectDevice();
                }
            }
        });
    },
    async configDevice() {
        this.state = "开始配网"
        let netConfigInfo = {
            ssid: this.wifiApInfo.ssid,
            ssidPassword: '',
            isDefaultPassword: true,
            channel: this.wifiApInfo.channel,
            sessionId: getApp(this).ConfigParams.deviceInfo.sessionId,
            type: this.isNAN ? 0 : 1,
            wifiApId: this.wifiApInfo.wifiApId,
            vendorData: '',
            timeout: 30,
            paramValid: true
        };
        console.info("netconfig configDevice" + JSON.stringify(netConfigInfo))
        // Step4 config the device net.
        getApp(this).NetConfig.configDeviceNet('deviceInfo', 'accountInfo', netConfigInfo, (result) => {
            if (result.code == 0) {
                this.state = "配网成功"
                // Step5 config the device net success, go to the control.
                this.goToControl();
            } else if (this.isNAN) {
                this.startSoftAp()
            } else {
                this.state = "配网失败"
                this.isFail = true
                this.disconnectDevice();
            }
        });
    },
    registerDisconnectCallback(sessionId) {
        let commonInfo = {
            sessionId: sessionId
        }
        getApp(this).NetConfig.registerDisconnectCallback(commonInfo, (result) => {
            if (result.code == 0) {
            }
        });
    },
    startSoftAp() {
        this.isNAN = false
        this.state = "softAP配网"
        this.disconnectDevice()
        getApp(this).ConfigParams.deviceInfo.sessionId = ''
        this.discoverDeviceBySoftAp()
    },
    discoverDeviceBySoftAp() {
        if (!this.targetDeviceId) {
            this.state = "apName为空: " + this.targetDeviceId
            return;
        }
        getApp(this).NetConfig.discoveryBySoftAp((result) => {
            console.info("NetConfig# discoveryBySoftAp" + JSON.stringify(result))
            if (result.code == 0) {
                this.state = "softAP发现成功"
                getApp(this).ConfigParams.deviceInfo = result.data;
                getApp(this).ConfigParams.deviceInfo.sessionId = ''
                let connectInfo = {
                    targetDeviceId: this.targetDeviceId,
                    //                    targetDeviceId: "teamX-Lamp01",
                    type: 1,
                    pin: '11111111',
                    password: '',
                    sessionId: ''
                };
                this.connectDevice(connectInfo);
            } else {
                this.isFail = true
            }
        })
    },
    disconnectDevice() {
        let commonInfo = {
            sessionId: getApp(this).ConfigParams.deviceInfo.sessionId
        }
        getApp(this).NetConfig.disconnectDevice(commonInfo, (result) => {
            if (result.code == 0) {
                return;
            }
        });
    },
    goToControl() {
        this.disconnectDevice();
        let target = {
            bundleName: 'com.xclgj.distschedule',
            abilityName: 'com.example.distschedule.AddDeviceAbility',
            deviceType: 1,
            data: {
                deviceId: this.deviceId
            }
        }
        FeatureAbility.startAbility(target);
        app.terminate()
    },
    cancel() {
        app.terminate();
    }
}
