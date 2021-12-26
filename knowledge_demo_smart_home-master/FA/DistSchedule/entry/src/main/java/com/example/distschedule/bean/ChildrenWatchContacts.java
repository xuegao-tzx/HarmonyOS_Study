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

package com.example.distschedule.bean;

/**
 * 用于记录儿童手表联系人数据类
 *
 * @since 2021-09-19


public class ChildrenWatchContacts {

    private int contactsImgId;
    private String contactsName;
    private String contactsNumber;
    private boolean isSelect;

    public ChildrenWatchContacts(int contactsImgId, String contactsName, String contactsNumber, boolean isSelect) {
        this.contactsImgId = contactsImgId;
        this.contactsName = contactsName;
        this.contactsNumber = contactsNumber;
        this.isSelect = isSelect;
    }

    public int getContactsImgId() {
        return contactsImgId;
    }

    public void setContactsImgId(int contactsImgId) {
        this.contactsImgId = contactsImgId;
    }

    public String getContactsName() {
        return contactsName;
    }

    public void setContactsName(String contactsName) {
        this.contactsName = contactsName;
    }

    public String getContactsNumber() {
        return contactsNumber;
    }

    public void setContactsNumber(String contactsNumber) {
        this.contactsNumber = contactsNumber;
    }

    public boolean getIsSelect() {
        return isSelect;
    }

    public void setIsSelect(boolean isSelect) {
        this.isSelect = isSelect;
    }

}
*/