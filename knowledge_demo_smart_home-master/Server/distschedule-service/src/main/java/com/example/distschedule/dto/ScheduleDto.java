/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.example.distschedule.dto;

import com.example.distschedule.dao.model.Schedule;

import java.util.Date;

public class ScheduleDto implements Comparable<ScheduleDto>{
    private String id;

    private String name;

    private String userIds;

    private String deviceConfig;

    private String remindDay;

    private String creatorId;

    private String sceneId;

    private String stateDetail;

    private Date startTime;

    private Date endTime;

    private Date createTime;

    private Date updateTime;

    public ScheduleDto() {
    }

    public ScheduleDto(Schedule schedule) {
        this.id = schedule.getId();
        this.name = schedule.getName();
        this.deviceConfig = schedule.getDeviceConfig();
        this.remindDay = schedule.getRemindDay();
        this.creatorId = schedule.getCreatorId();
        this.sceneId = schedule.getSceneId();
        this.stateDetail = schedule.getStateDetail();
        this.startTime = schedule.getStartTime();
        this.endTime = schedule.getEndTime();
        this.createTime = schedule.getCreateTime();
        this.updateTime = schedule.getUpdateTime();
        this.userIds = schedule.getUserIds();
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

    public Date getStartTime() {
        return startTime;
    }

    public void setStartTime(Date startTime) {
        this.startTime = startTime;
    }

    public Date getEndTime() {
        return endTime;
    }

    public void setEndTime(Date endTime) {
        this.endTime = endTime;
    }

    public Date getCreateTime() {
        return createTime;
    }

    public void setCreateTime(Date createTime) {
        this.createTime = createTime;
    }

    public Date getUpdateTime() {
        return updateTime;
    }

    public void setUpdateTime(Date updateTime) {
        this.updateTime = updateTime;
    }

    @Override
    public int compareTo(ScheduleDto scheduleDto) {
        return this.startTime.compareTo(scheduleDto.getStartTime());
    }
}
