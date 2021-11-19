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
import dataProvider from './data.js';
import device from '@system.device';
let isFirstPhoneChange = false
let isFirstNameChange = false

export default {
    data: dataProvider,
    onShow() {
        // 查询当前设备类型
        device.getInfo({
            success: (data) => {
                if (data.deviceType && (data.deviceType === 'tv' || data.deviceType === 'wearable'
                || data.deviceType === 'tablet' )) {
                    this.isPhoneDevice = false;
                }
            },
            fail: function (data, code) {
                console.info('Failed to obtain device information. Error code:' + code + '; Error information: ' + data);
                this.isPhoneDevice = false;
            }
        });
    },
    onReady() {

    },
    clickItem(idx) {
        this.detailObj = this.contactList[idx];
        this.name = this.detailObj.name;
        this.phone = this.detailObj.phone;
        if (this.isShowDialog) {
            this.showDialog();
        }
    },
    showDialog() {
        isFirstPhoneChange = true;
        isFirstNameChange = true;
        this.$element('detailDialog').show();
    },
    onInit() {
        this.detailObj = this.contactList[0];
    },
    listFocus(idx) {
        this.detailObj = this.contactList[idx];
    },
    confirm() {
        this.detailObj.name = this.name;
        this.detailObj.phone = this.phone;
        this.$element('detailDialog').close();
        this.resetInput();
    },
    resetInput(){
        this.name = "";
        this.phone = "";
    },
    cancel() {
        this.$element('detailDialog').close();
        this.resetInput();
    },
    // 更新input Name值
    changeName(e) {
        let changeValue = e.text
        if (isFirstNameChange && changeValue === "") {
            isFirstNameChange = false;
        }else{
            this.name = changeValue;
            isFirstNameChange = false;
        }
    },
    // 更新input Phone值
    changePhone(e) {
        let changeValue = e.text;
        if (isFirstPhoneChange && changeValue === "") {
            isFirstPhoneChange = false;
        }else{
            this.phone = changeValue;
            isFirstPhoneChange = false;
        }
    },
    dialogCancel(){
        this.resetInput();
    }
};