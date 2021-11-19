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

/**
 * NET_CONFIG JS API
 */
const CONSTANT = {
    BUNDLE_NAME: "com.huawei.ailifeability",
    ABILITY_NAME: "com.huawei.ailifeability.NetConfigAbility",
    IS_SUPPORT_WIFI_AWARE: 1000,
    DISCOVERY_BY_NAN_CODE: 1001,
    DISCOVERY_BY_SOFT_AP_CODE: 1002,
    CONNECT_DEVICE_CODE: 1003,
    GET_WIFI_LIST_CODE: 1004,
    SEND_MESSAGE_CODE: 1005,
    CONFIG_DEVICE_NET_CODE: 1006,
    DISCONNECT_DEVICE_CODE: 1007,
    SUBSCRIBE_EVENT_CODE: 1008,
    UNSUBSCRIBE_EVENT_CODE: 1009,
    REGISTER_MSG_RECEIVE: 1010,
    REGISTER_DISCONNECT_CALLBACK: 1011,
    SET_DEVICE_BIND_INFO: 1012
};
var EventObserver = {
    isInited: false,
    callbackMap: new Map(),
    register: function (item) {
        EventObserver.callbackMap.set(item.messageCode, item.callback);
    },
    notify: function (result) {
        if (result.messageCode && EventObserver.callbackMap.get(result.messageCode) != undefined) {
            EventObserver.callbackMap.get(result.messageCode)(result);
        } else {
            console.error("NetConfig# notify failed, current messageCode not exist, messageCode=" + result.messageCode);
        }
    }
};

function getSubscribeParam() {
    let action = {};
    action.bundleName = CONSTANT.BUNDLE_NAME;
    action.abilityName = CONSTANT.ABILITY_NAME;
    action.messageCode = CONSTANT.SUBSCRIBE_EVENT_CODE;
    action.abilityType = 1;
    action.syncOption = 0;

    return action;
}

function unsubscribe() {
    let action = {};
    action.bundleName = CONSTANT.BUNDLE_NAME;
    action.abilityName = CONSTANT.ABILITY_NAME;
    action.messageCode = CONSTANT.UNSUBSCRIBE_EVENT_CODE;
    action.abilityType = 1;

    FeatureAbility.unsubscribeAbilityEvent(action).then(function (result) {
        let ret = JSON.parse(result);
        if (ret.code !== 0) {
            console.error("NetConfig# unsubscribe abilityEvent failed, error message: " + ret.data);
        }
    });
}

function callAbilityFunc(callCode, argsObj, callbackFunc) {
    console.info("NetConfig# callAbilityFunc arguments" + JSON.stringify(arguments))
    let action = {};
    action.bundleName = CONSTANT.BUNDLE_NAME;
    action.abilityName = CONSTANT.ABILITY_NAME;
    action.messageCode = callCode;
    action.abilityType = 1;
    action.data = argsObj;

    if (callbackFunc != undefined) {
        EventObserver.register({
            messageCode: callCode,
            callback: callbackFunc
        });
    }

    console.info("NetConfig# callAbilityFunc start.");
    return FeatureAbility.callAbility(action);
}

function callAbility(callCode, argsObj, callbackFunc) {
    console.info("NetConfig# callAbility arguments" + JSON.stringify(arguments))
    if (EventObserver.isInited) {
        return callAbilityFunc(callCode, argsObj, callbackFunc);
    } else {
        console.info("NetConfig# subscribe start.");
        return FeatureAbility.subscribeAbilityEvent(getSubscribeParam(), (result) => {
            let ret = JSON.parse(result);
            if (ret.data) {
                EventObserver.notify(ret.data);
            } else {
                console.error("NetConfig# subscribe callback JSON parse failed");
            }
        }).then((result) => {
            console.info("NetConfigTime#FA subscribe end.");
            let ret = JSON.parse(result);
            if (ret.code !== 0) {
                console.error("NetConfig# subscribe abilityEvent failed, error message: " + ret.data);
            }

            EventObserver.isInited = true;
            return callAbilityFunc(callCode, argsObj, callbackFunc);
        });
    }
}

var FANetConfig = {
    isSupportWifiAware: function () {
        return callAbility(CONSTANT.IS_SUPPORT_WIFI_AWARE, {});
    },
    discoveryByNAN: function (scanInfo, callbackFunc) {
        if (!(typeof scanInfo.duration === 'number' && scanInfo.duration % 1 === 0)) {
            console.error("NetConfig# call discoveryByNAN failed, parameter 'duration' type should be integer.");
            return;
        }
        if (!(typeof scanInfo.lockTime === 'number' && scanInfo.lockTime % 1 === 0)) {
            console.error("NetConfig# call discoveryByNAN failed, parameter 'lockTime' type should be integer.");
            return;
        }

        if (typeof callbackFunc !== 'function') {
            console.error("NetConfig# call discoveryByNAN failed, parameter 'callbackFunc' type should be function.");
            return;
        }
        return callAbility(CONSTANT.DISCOVERY_BY_NAN_CODE, {
            scanNanInfo: scanInfo
        }, callbackFunc);
    },
    discoveryBySoftAp: function (callbackFunc) {
        if (typeof callbackFunc !== 'function') {
            console.error("NetConfig# call discoveryBySoftAp failed, parameter 'callbackFunc' type should be function.");
            return;
        }
        return callAbility(CONSTANT.DISCOVERY_BY_SOFT_AP_CODE, {}, callbackFunc);
    },
    connectDevice: function (connectInfo, callbackFunc) {
        let isConnectInfoNull = connectInfo == null || connectInfo == undefined;
        if (isConnectInfoNull) {
            console.error("NetConfig# call connectDevice failed, parameter 'connectInfo' should not be undefined.");
            return;
        }

        if (typeof connectInfo.targetDeviceId !== 'string') {
            console.error("NetConfig# call connectDevice failed, parameter 'connectInfo.targetDeviceId' type should be string.");
            return;
        }

        if (!(typeof connectInfo.type === 'number' && connectInfo.type % 1 === 0)) {
            console.error("NetConfig# call connectDevice failed, parameter 'connectInfo.type' type should be integer.");
            return;
        }

        if (typeof connectInfo.pin !== 'string') {
            console.error("NetConfig# call connectDevice failed, parameter 'connectInfo.pin' type should be string.");
            return;
        }

        if (typeof connectInfo.password !== 'string') {
            console.error("NetConfig# call connectDevice failed, parameter 'connectInfo.password' type should be string.");
            return;
        }

        if (typeof connectInfo.sessionId !== 'string') {
            console.error("NetConfig# call connectDevice failed, parameter 'connectInfo.sessionId' type should be string.");
            return;
        }

        if (typeof callbackFunc !== 'function') {
            console.error("NetConfig# call connectDevice failed, parameter 'callbackFunc' type should be function.");
            return;
        }

        return callAbility(CONSTANT.CONNECT_DEVICE_CODE, {
            connectInfo: connectInfo
        }, callbackFunc);
    },
    getWifiList: function (callbackFunc) {
        if (typeof callbackFunc !== 'function') {
            console.error("NetConfig# call getWifiList failed, parameter 'callbackFunc' type should be function.");
            return;
        }
        return callAbility(CONSTANT.GET_WIFI_LIST_CODE, {}, callbackFunc);
    },
    sendMessage: function (commonInfo, message, callbackFunc) {
        if (commonInfo == null || commonInfo == undefined) {
            console.error("NetConfig# call sendMessage failed, parameter 'commonInfo' type should be string.");
            return;
        }
        if (typeof commonInfo.sessionId !== 'string') {
            console.error("NetConfig# call sendMessage failed, parameter 'sessionId' type should be string.");
            return;
        }

        if (typeof message !== 'string') {
            console.error("NetConfig# call sendMessage failed, parameter 'message' type should be string.");
            return;
        }

        if (typeof callbackFunc !== 'function') {
            console.error("NetConfig# call sendMessage failed, parameter 'callbackFunc' type should be function.");
            return;
        }
        return callAbility(CONSTANT.SEND_MESSAGE_CODE, {
            commonInfo: commonInfo,
            message: message
        }, callbackFunc);
    },
    configDeviceNet: function (deviceInfo, accountInfo, netConfigInfo, callbackFunc) {
        if (typeof deviceInfo !== 'string') {
            console.error("NetConfig# call configDeviceNet failed, parameter 'deviceInfo' type should be string.");
            return;
        }

        if (typeof accountInfo !== 'string') {
            console.error("NetConfig# call configDeviceNet failed, parameter 'accountInfo' type should be string.");
            return;
        }

        let isNetConfigInfoNull = netConfigInfo == null || netConfigInfo == undefined;
        if (isNetConfigInfoNull) {
            console.error("NetConfig# call configDeviceNet failed, parameter 'netConfigInfo' should not be undefined.");
            return;
        }

        if (typeof netConfigInfo.ssid !== 'string') {
            console.error("NetConfig# call configDeviceNet failed, parameter 'netConfigInfo.ssid' type should be string.");
            return;
        }

        if (typeof netConfigInfo.isDefaultPassword !== 'boolean') {
            console.error("NetConfig# call configDeviceNet failed, parameter 'netConfigInfo.isDefaultPassword' type should be boolean.");
            return;
        }

        if (!(typeof netConfigInfo.channel === 'number' && netConfigInfo.channel % 1 === 0)) {
            console.error("NetConfig# call configDeviceNet failed, parameter 'netConfigInfo.channel' type should be integer.");
            return;
        }

        if (typeof netConfigInfo.sessionId !== 'string') {
            console.error("NetConfig# call configDeviceNet failed, parameter 'netConfigInfo.sessionId' type should be string.");
        }

        if (!(typeof netConfigInfo.type === 'number' && netConfigInfo.type % 1 === 0)) {
            console.error("NetConfig# call configDeviceNet failed, parameter 'netConfigInfo.type' type should be integer.");
            return;
        }

        if (typeof callbackFunc !== 'function') {
            console.error("NetConfig# call configDeviceNet failed, parameter 'callbackFunc' type should be function.");
            return;
        }
        return callAbility(CONSTANT.CONFIG_DEVICE_NET_CODE, {
            deviceInfo: deviceInfo,
            accountInfo: accountInfo,
            netConfigInfo: netConfigInfo
        }, callbackFunc);
    },
    disconnectDevice: function (commonInfo, callbackFunc) {
        if (commonInfo == null || commonInfo == undefined) {
            console.error("NetConfig# call disconnectDevice failed, parameter 'commonInfo' type should be string.");
            return;
        }
        if (typeof commonInfo.sessionId !== 'string') {
            console.error("NetConfig# call disconnectDevice failed, parameter 'sessionId' type should be string.");
            return;
        }

        if (typeof callbackFunc !== 'function') {
            console.error("NetConfig# call disconnectDevice failed, parameter 'callbackFunc' type should be function.");
            return;
        }
        return callAbility(CONSTANT.DISCONNECT_DEVICE_CODE, {
            commonInfo: commonInfo
        }, callbackFunc);
    },
    registerMsgReceive: function (commonInfo, callbackFunc) {
        if (commonInfo == null || commonInfo == undefined) {
            console.error("NetConfig# call registerMsgReceive failed, parameter 'commonInfo' type should be string.");
            return;
        }
        if (typeof commonInfo.sessionId !== 'string') {
            console.error("NetConfig# call registerMsgReceive failed, parameter 'sessionId' type should be string.");
            return;
        }

        if (typeof callbackFunc !== 'function') {
            console.error("NetConfig# call registerMsgReceive failed, parameter 'callbackFunc' type should be function.");
            return;
        }
        return callAbility(CONSTANT.REGISTER_MSG_RECEIVE, {
            commonInfo: commonInfo
        }, callbackFunc);
    },
    registerDisconnectCallback: function (commonInfo, callbackFunc) {
        if (commonInfo == null || commonInfo == undefined) {
            console.error("NetConfig# call registerDisconnectCallback failed, parameter 'commonInfo' type should be string.");
            return;
        }
        if (typeof commonInfo.sessionId !== 'string') {
            console.error("NetConfig# call registerDisconnectCallback failed, parameter 'sessionId' type should be string.");
            return;
        }

        if (typeof callbackFunc !== 'function') {
            console.error("NetConfig# call registerDisconnectCallback failed, parameter 'callbackFunc' type should be function.");
            return;
        }
        return callAbility(CONSTANT.REGISTER_DISCONNECT_CALLBACK, {
            commonInfo: commonInfo
        }, callbackFunc);
    },
    setDeviceBindInfo: function (bindInfo, callbackFunc) {
        if (bindInfo == null || bindInfo == undefined) {
            console.error("NetConfig# call setDeviceBinfInfo failed, parameter 'bindInfo' is null.");
            return;
        }
        if (typeof callbackFunc !== 'function') {
            console.error("NetConfig# call setDeviceBinfInfo failed, parameter 'callbackFunc' type should be function.");
            return;
        }
        return callAbility(CONSTANT.SET_DEVICE_BIND_INFO, {
            bindInfo: bindInfo
        }, callbackFunc);
    }
};

export default FANetConfig;