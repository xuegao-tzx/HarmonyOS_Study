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

import org.apache.commons.lang3.StringUtils;

public class ScheduleResult {
    private String id;
    private String name;
    private String userIds;
    private String deviceIds;
    private String deviceConfig;
    private String remindDay;
    private String creatorId;
    private String sceneId;
    private String stateDetail;
    private String startTime;
    private String endTime;
    private String updateTime;
    private String createTime;
    // 用来显示日期 不是服务器返回的 如2021年9月24日 星期五
    private String showDate;

    public String getShowDate() {
        return showDate;
    }

    public void setShowDate(String showDate) {
        this.showDate = showDate;
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getUserIds() {
        return userIds;
    }

    public void setUserIds(String userIds) {
        this.userIds = userIds;
    }

    public String getDeviceIds() {
        return deviceIds;
    }

    public void setDeviceIds(String deviceIds) {
        this.deviceIds = deviceIds;
    }

    public String getDeviceConfig() {
        return deviceConfig;
    }

    public void setDeviceConfig(String deviceConfig) {
        this.deviceConfig = deviceConfig;
    }

    public String getRemindDay() {
        return remindDay;
    }

    public void setRemindDay(String remindDay) {
        this.remindDay = remindDay;
    }

    public String getCreatorId() {
        return creatorId;
    }

    public void setCreatorId(String creatorId) {
        this.creatorId = creatorId;
    }

    public String getSceneId() {
        return sceneId;
    }

    public void setSceneId(String sceneId) {
        this.sceneId = sceneId;
    }

    public String getStateDetail() {
        return stateDetail;
    }

    public void setStateDetail(String stateDetail) {
        this.stateDetail = stateDetail;
    }

    public String getStartTime() {
        return startTime;
    }

    public void setStartTime(String startTime) {
        this.startTime = startTime;
    }

    public String getEndTime() {
        return endTime;
    }

    public void setEndTime(String endTime) {
        this.endTime = endTime;
    }

    public String getUpdateTime() {
        return updateTime;
    }

    public void setUpdateTime(String updateTime) {
        this.updateTime = updateTime;
    }

    public String getCreateTime() {
        return createTime;
    }

    public void setCreateTime(String createTime) {
        this.createTime = createTime;
    }

    public String getDurationString() {
        if (StringUtils.isEmpty(startTime) || StringUtils.isEmpty(endTime)) {
            return "";
        } else {
            return startTime.split(" ")[1] + "-" + endTime.split(" ")[1];
        }
    }

    public boolean isError() {
        return !StringUtils.isBlank(stateDetail);
    }
}
