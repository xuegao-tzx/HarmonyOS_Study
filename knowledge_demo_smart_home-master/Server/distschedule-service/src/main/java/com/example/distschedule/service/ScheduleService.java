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
package com.example.distschedule.service;


import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONArray;
import com.alibaba.fastjson.JSONObject;
import com.example.distschedule.dao.mapper.ScheduleMapper;
import com.example.distschedule.dao.model.Schedule;
import com.example.distschedule.dao.model.UserSchedule;
import com.example.distschedule.dto.CreateScheduleDto;
import com.example.distschedule.dto.DeviceDto;
import com.example.distschedule.dto.DeviceTypeDto;
import com.example.distschedule.dto.ScheduleDto;
import com.example.distschedule.enums.ScheduleComandType;
import com.example.distschedule.error.ErrorCode;
import com.example.distschedule.exception.DistscheduleScheduleException;
import com.huaweicloud.sdk.core.exception.ConnectionException;
import com.huaweicloud.sdk.core.exception.RequestTimeoutException;
import com.huaweicloud.sdk.core.exception.ServiceResponseException;
import com.huaweicloud.sdk.iotda.v5.model.CreateCommandRequest;
import com.huaweicloud.sdk.iotda.v5.model.CreateCommandResponse;
import com.huaweicloud.sdk.iotda.v5.model.DeviceCommandRequest;
import org.apache.commons.lang3.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.text.SimpleDateFormat;
import java.util.*;

@Service
public class ScheduleService {
    private static final Logger LOGGER = LoggerFactory.getLogger(ScheduleService.class);
    @Autowired
    private IOTCloudService iotCloudService;
    @Autowired
    private DeviceService deviceService;
    @Autowired
    private DeviceTypeService deviceTypeService;
    @Autowired
    private ScheduleMapper scheduleMapper;

    public Optional<ScheduleDto> getScheduleById(String scheduleId) {
        Schedule schedule = scheduleMapper.selectScheduleById(scheduleId);

        if (schedule != null) {
            return Optional.of(new ScheduleDto(schedule));
        }
        return Optional.empty();
    }

    @Transactional(rollbackFor = DistscheduleScheduleException.class)
    public String saveSchedule(CreateScheduleDto createScheduleDto) throws DistscheduleScheduleException {
        Schedule schedule = new Schedule();
        schedule.setId(UUID.randomUUID().toString());
        schedule.setName(createScheduleDto.getName());
        schedule.setDeviceConfig(createScheduleDto.getDeviceConfig());
        schedule.setRemindDay(createScheduleDto.getRemindDay());
        schedule.setCreatorId(createScheduleDto.getCreatorId());
        schedule.setSceneId("default");
        schedule.setStateDetail(createScheduleDto.getStateDetail());
        schedule.setStartTime(createScheduleDto.getStartTime());
        schedule.setEndTime(createScheduleDto.getEndTime());
        schedule.setUserIds(createScheduleDto.getUserIds());

        int res = scheduleMapper.saveSchedule(schedule);
        if (res == 0) {
            return "";
        }

        //保存用户日程信息
        List<UserSchedule> userSchedules = new LinkedList<>();
        if (StringUtils.isNotBlank(createScheduleDto.getUserIds())) {
            String[] userIds = createScheduleDto.getUserIds().split(",");
            for (String userId : userIds) {
                userSchedules.add(new UserSchedule(userId, schedule.getId()));
            }
        }
        res = scheduleMapper.saveUserSchedule(userSchedules);
        if (res == 0) {
            throw new DistscheduleScheduleException(ErrorCode.SCHEDULE_USER_SAVE_FAIL);
        }

        // 根据客户端传上来的设备列表，向IOT云发送定时命令
        if (StringUtils.isNotBlank(createScheduleDto.getDeviceConfig())) {
            JSONArray deviceJsonArray = JSON.parseArray(createScheduleDto.getDeviceConfig());

            for (Iterator<Object> iterator = deviceJsonArray.iterator(); iterator.hasNext(); ) {
                JSONObject next = (JSONObject) iterator.next();
                try {
                    sendScheduleComand(next.getString("deviceId"), ScheduleComandType.A, schedule, next.getJSONObject("command"));
                } catch (DistscheduleScheduleException e) {
                    LOGGER.warn("Fail to send iot command for device " + next.getString("deviceId"), e);
                }
            }
        }

        // 给联系人的儿童手表通知日程
        sendChildWatchSchedule(ScheduleComandType.A, schedule);

        return schedule.getId();
    }

    /**
     * 给儿童手表发送日程
     *
     * @param deviceId           设备ID
     * @param scheduleComandType 命令类型
     * @param schedule           日程信息
     */
    private void sendChildWatchScheduleComand(String deviceId, ScheduleComandType scheduleComandType, Schedule schedule) throws DistscheduleScheduleException {
        CreateCommandRequest request = new CreateCommandRequest();
        DeviceCommandRequest body = new DeviceCommandRequest();
        body.setCommandName("SetSchedule");//目前写死定时命令
        Optional<DeviceDto> deviceDto = deviceService.getDeviceById(deviceId);
        if (!deviceDto.isPresent()) {
            throw new DistscheduleScheduleException(ErrorCode.DEVICE_NOT_FOUND);
        }
        Optional<DeviceTypeDto> deviceTypeDto = deviceTypeService.getDeviceTypeById(deviceDto.get().getTypeId());
        if (!deviceTypeDto.isPresent()) {
            throw new DistscheduleScheduleException(ErrorCode.DEVICE_TYPE_NOT_EXIST_FAIL);
        }
        body.setServiceId(deviceTypeDto.get().getServiceId());
        try {
            /*
          {
    "service_id":"SmartWatch",// 产品服务ID，固定
    "command_name":"SetSchedule",// 日程预约命令名字，固定
    "paras":{
        "ScheduleID":"0", // 日程ID号（int）,取值为0-65535， 用于服务端对设备日程的增、删、改
        "option":"A", // 日程操作类型（string）,取值为“A”（增加）、“D”（删除）、“U”（更新）
        "Day":"1,2,3,4,5,6,7",// 日程的天（星期几）类型（string）,已逗号区分的数字，“1,2,3,4,5,6,7”,或者下划线连接的数字，比方“1-7”标识周一到周日
        "StartHour":11, // 日程的起始时辰，类型（int）,取值为0-23
        "StartMinute":59, // 日程的起始分，类型（int), 取值为0-59
        "Command":{"MessageType":1}, // 提醒的消息类别，类型（int）,取值为0 -- 未定义的日程提醒； 1 -- 起床；2 -- 上学； 3 -- 做作业； 4 -- 喝                                         水时间； 5 -- 睡觉时间
        "DurationMinutes":5// 日程的预约提醒持续时间，类型（int）
    }int
}
             */
            JSONObject paras = new JSONObject();
            paras.put("ScheduleID", schedule.getId());
            paras.put("Option", scheduleComandType.name());
            if (schedule.getStartTime() != null && schedule.getEndTime() != null) {
                Calendar startCalendar = Calendar.getInstance();
                startCalendar.setTime(schedule.getStartTime());
                paras.put("StartHour", startCalendar.get(Calendar.HOUR_OF_DAY));
                paras.put("StartMinute", startCalendar.get(Calendar.MINUTE));
                paras.put("DurationMinutes", (schedule.getEndTime().getTime() - schedule.getStartTime().getTime()) / 1000 / 60);//持续时间，按分钟
            } else {
                paras.put("StartHour", 0);
                paras.put("StartMinute", 0);
            }
            if (StringUtils.isNotBlank(schedule.getRemindDay())) {
                paras.put("Day", schedule.getRemindDay());
            } else {
                paras.put("Day", "0");//设备默认一次性日程
            }
            JSONObject command = new JSONObject();
            command.put("name", schedule.getName());
            paras.put("Command", command);
            body.setParas(paras);
        } catch (Exception e) {
            LOGGER.error(e.getMessage());
            throw new DistscheduleScheduleException("Invalid command value", ErrorCode.SCHEDULE_SEND_COMMAND_FAIL);
        }

        request.setDeviceId(deviceId);
        request.setBody(body);

        try {
            // 创建异步命令
            CreateCommandResponse response = iotCloudService.getIoTDAClient().createCommand(request);
            LOGGER.info("createCommand response= " + response.toString());
        } catch (ConnectionException e) {
            throw new DistscheduleScheduleException(e.getMessage(), ErrorCode.SCHEDULE_SEND_COMMAND_FAIL);
        } catch (RequestTimeoutException e) {
            throw new DistscheduleScheduleException(e.getMessage(), ErrorCode.SCHEDULE_SEND_COMMAND_FAIL);
        } catch (ServiceResponseException e) {
            throw new DistscheduleScheduleException(e.getMessage(), ErrorCode.SCHEDULE_SEND_COMMAND_FAIL);
        }
    }

    /**
     * 发送定时命令
     *
     * @param deviceId    设备ID
     * @param commandType 命令类型
     * @param schedule    日程
     */
    private void sendScheduleComand(String deviceId, ScheduleComandType commandType, Schedule schedule, JSONObject command) throws DistscheduleScheduleException {

        CreateCommandRequest request = new CreateCommandRequest();
        DeviceCommandRequest body = new DeviceCommandRequest();
        body.setCommandName("SetSchedule");//目前写死定时命令
        Optional<DeviceDto> deviceDto = deviceService.getDeviceById(deviceId);
        if (!deviceDto.isPresent()) {
            throw new DistscheduleScheduleException(ErrorCode.DEVICE_NOT_FOUND);
        }
        Optional<DeviceTypeDto> deviceTypeDto = deviceTypeService.getDeviceTypeById(deviceDto.get().getTypeId());
        if (!deviceTypeDto.isPresent()) {
            throw new DistscheduleScheduleException(ErrorCode.DEVICE_TYPE_NOT_EXIST_FAIL);
        }
        body.setServiceId(deviceTypeDto.get().getServiceId());
        try {
            /*
            {
	"service_id": "SmartLamp",            // 产品服务ID，固定为SmartLamp
	"command_name": "SetLampShedule",     // 命令名称，固定为SetLampShedule
	"paras":
 	{
 		"ScheduleID":0			// 日程ID号(类型int)： 0 - 65535
 		"option":"A"			// 日程操作(类型string)：长度为1，暂指定3个操作：A(新增日程)，U(更新日程)，D(删除日程)
		"Day": "1,3,5",   		// 开始亮灯的日期，以星期为单位
								// 取值(类型string): "1,2,3,4,5,6,7", 1-7代表周一到周日，每天之间用逗号(,)隔开，可以只设1天，也可以设置多天
		"StartHour": 18,     	// 开始亮灯的小时时间(类型int) : 0 - 23
		"StartMinute": 12,   	// 开始亮灯的分钟时间(类型int) : 0-59
		"DurationMinutes": 1 	// 亮灯持续时间(类型int) : 0 - 1440
	}
}
             */
            JSONObject paras = new JSONObject();
            paras.put("ScheduleID", schedule.getId());
            paras.put("Option", commandType.name());
            if (schedule.getStartTime() != null && schedule.getEndTime() != null) {
                Calendar startCalendar = Calendar.getInstance();
                startCalendar.setTime(schedule.getStartTime());
                paras.put("StartHour", startCalendar.get(Calendar.HOUR_OF_DAY));
                paras.put("StartMinute", startCalendar.get(Calendar.MINUTE));
                paras.put("DurationMinutes", (schedule.getEndTime().getTime() - schedule.getStartTime().getTime()) / 1000 / 60);//持续时间，按分钟
            } else {
                paras.put("StartHour", 0);
                paras.put("StartMinute", 0);
            }
            if (StringUtils.isNotBlank(schedule.getRemindDay())) {
                paras.put("Day", schedule.getRemindDay());
            } else {
                paras.put("Day", "0");//设备默认一次性日程
            }
            paras.put("Command", command);
            body.setParas(paras);
        } catch (Exception e) {
            LOGGER.error(e.getMessage());
            throw new DistscheduleScheduleException("Invalid command value", ErrorCode.SCHEDULE_SEND_COMMAND_FAIL);
        }

        request.setDeviceId(deviceId);
        request.setBody(body);

        try {
            // 创建异步命令
            CreateCommandResponse response = iotCloudService.getIoTDAClient().createCommand(request);
            LOGGER.info("createCommand response= " + response.toString());
        } catch (ConnectionException e) {
            throw new DistscheduleScheduleException(e.getMessage(), ErrorCode.SCHEDULE_SEND_COMMAND_FAIL);
        } catch (RequestTimeoutException e) {
            throw new DistscheduleScheduleException(e.getMessage(), ErrorCode.SCHEDULE_SEND_COMMAND_FAIL);
        } catch (ServiceResponseException e) {
            throw new DistscheduleScheduleException(e.getMessage(), ErrorCode.SCHEDULE_SEND_COMMAND_FAIL);
        }
    }

    @Transactional(rollbackFor = DistscheduleScheduleException.class)
    public int deleteSchedule(String userId, String scheduleId) throws DistscheduleScheduleException {
        //1. 查设备
        Schedule schedule = scheduleMapper.selectScheduleById(scheduleId);
        if (schedule == null) {
            return 1;//找不到日程，删除成功，保证幂等性
        }
        //2. 验证权限
        if (!StringUtils.equals(schedule.getCreatorId(), userId)) {
            throw new DistscheduleScheduleException(ErrorCode.ILLEGAL_PERMISSION);
        }

        //3.删除日程
        scheduleMapper.deleteUserSchedule(scheduleId);
        int res = scheduleMapper.deleteSchedule(scheduleId);


        //4.删除IOT定时任务
        if (StringUtils.isNotBlank(schedule.getDeviceConfig())) {
            JSONArray deviceJsonArray = JSON.parseArray(schedule.getDeviceConfig());

            for (Iterator<Object> iterator = deviceJsonArray.iterator(); iterator.hasNext(); ) {
                JSONObject next = (JSONObject) iterator.next();
                try {
                    sendScheduleComand(next.getString("deviceId"), ScheduleComandType.D, schedule, new JSONObject());
                } catch (DistscheduleScheduleException e) {
                    LOGGER.error(e.getMessage(), e);
                    //即使发送不成功也允许删除设备
                }
            }
        }

        // 给联系人的儿童手表通知日程
        sendChildWatchSchedule(ScheduleComandType.D, schedule);
        return res;
    }

    public int updateSchedule(String userId, String scheduleId, CreateScheduleDto updateScheduleDto) throws DistscheduleScheduleException {
        Schedule schedule = scheduleMapper.selectScheduleById(scheduleId);
        if (schedule == null) {
            throw new DistscheduleScheduleException(ErrorCode.SCHEDULE_NOT_FOUND);
        }

        if (!StringUtils.equals(schedule.getCreatorId(), userId)) {
            throw new DistscheduleScheduleException(ErrorCode.ILLEGAL_PERMISSION);
        }

        schedule.setName(updateScheduleDto.getName());
        schedule.setDeviceConfig(updateScheduleDto.getDeviceConfig());
        schedule.setRemindDay(updateScheduleDto.getRemindDay());
        //schedule.setCreatorId(updateScheduleDto.getCreatorId());暂不修改该属性
        //schedule.setSceneId(updateScheduleDto.getSceneId());暂不修改该属性
        schedule.setStateDetail(updateScheduleDto.getStateDetail());
        schedule.setStartTime(updateScheduleDto.getStartTime());
        schedule.setEndTime(updateScheduleDto.getEndTime());

        // 更新用户联系人~~后续优化
        scheduleMapper.deleteUserSchedule(scheduleId);
        List<UserSchedule> userSchedules = new LinkedList<>();
        if (StringUtils.isNotBlank(updateScheduleDto.getUserIds())) {
            String[] userIds = updateScheduleDto.getUserIds().split(",");
            for (String scheduleUserId : userIds) {
                userSchedules.add(new UserSchedule(scheduleUserId, schedule.getId()));
            }
        }
        scheduleMapper.saveUserSchedule(userSchedules);
        int res = scheduleMapper.updateSchedule(schedule);

        // 通知IOT设备，发送失败后，暂不回滚。后续再改成异步，并处理异常。
        if (StringUtils.isNotBlank(updateScheduleDto.getDeviceConfig())) {
            JSONArray deviceJsonArray = JSON.parseArray(updateScheduleDto.getDeviceConfig());

            for (Iterator<Object> iterator = deviceJsonArray.iterator(); iterator.hasNext(); ) {
                JSONObject next = (JSONObject) iterator.next();
                try {
                    sendScheduleComand(next.getString("deviceId"), ScheduleComandType.U, schedule, next.getJSONObject("command"));
                } catch (DistscheduleScheduleException e) {
                    LOGGER.error(e.getMessage(), e);
                    //即使发送不成功也允许更新设备
                }
            }
        }

        // 给联系人的儿童手表通知日程
        sendChildWatchSchedule(ScheduleComandType.U, schedule);

        return res;
    }

    public List<ScheduleDto> queryScheduleByUser(String userId, Date startTime, Date endTime) {
        List<ScheduleDto> scheduleDtos = new LinkedList<>();
        // 1.按起始时间查询日程
        List<Schedule> defaultSchedules = scheduleMapper.selectDefaultSchedulesByDatesAndUser(userId, startTime, endTime);
        for (Schedule defaultSchedule : defaultSchedules) {
            ScheduleDto scheduleDto = new ScheduleDto(defaultSchedule);
            scheduleDtos.add(scheduleDto);
        }

        // 2.查询非普通日程
        List<Schedule> noDefaultSchedules = scheduleMapper.selectNotDefaultSchedulesByUser(userId);
        for (Schedule noDefaultSchedule : noDefaultSchedules) {
            ScheduleDto scheduleDto = new ScheduleDto(noDefaultSchedule);
            scheduleDtos.add(scheduleDto);
        }
        Collections.sort(scheduleDtos);
        return scheduleDtos;
    }

    public List<ScheduleDto> searchSchedulesByName(String userId, String name) {
        // 查询前后30天的日程
        Calendar calendarStartTime = Calendar.getInstance();
        calendarStartTime.setTime(new Date());
        calendarStartTime.add(Calendar.DATE, -30);

        Calendar calendarEndTime = Calendar.getInstance();
        calendarEndTime.setTime(new Date());
        calendarEndTime.add(Calendar.DATE, +30);

        List<ScheduleDto> scheduleDtos = new LinkedList<>();
        // 1.按起始时间查询日程
        List<Schedule> defaultSchedules = scheduleMapper.searchDefaultSchedulesByName(userId, calendarStartTime.getTime(), calendarEndTime.getTime(), name);
        for (Schedule defaultSchedule : defaultSchedules) {
            ScheduleDto scheduleDto = new ScheduleDto(defaultSchedule);
            scheduleDtos.add(scheduleDto);
        }

        // 2.查询非普通日程
        List<Schedule> noDefaultSchedules = scheduleMapper.searchNotDefaultSchedulesByName(userId, name);
        for (Schedule noDefaultSchedule : noDefaultSchedules) {
            ScheduleDto scheduleDto = new ScheduleDto(noDefaultSchedule);
            scheduleDtos.add(scheduleDto);
        }
        return scheduleDtos;
    }

    @Transactional(rollbackFor = Exception.class)
    public void deleteUserAllSchedules(String userId) {
        scheduleMapper.deleteSchedulesByUserId(userId);
        scheduleMapper.deleteUserScheduleByUserId(userId);
    }

    /**
     * 给儿童手表发送命令
     *
     * @param scheduleComandType 命令类型
     * @param schedule           日程
     */
    private void sendChildWatchSchedule(ScheduleComandType scheduleComandType, Schedule schedule) {
        if (StringUtils.isNotBlank(schedule.getUserIds())) {
            String[] userIds = schedule.getUserIds().split(",");
            for (String userId : userIds) {
                // 查到手表,给手表发送日程信息
                List<DeviceDto> deviceInfos = deviceService.getDevicesByUserId(userId);
                for (DeviceDto deviceInfo : deviceInfos) {
                    if (deviceInfo.getTypeId() == 12) {//儿童手表的类型号
                        try {
                            sendChildWatchScheduleComand(deviceInfo.getId(), scheduleComandType, schedule);
                        } catch (DistscheduleScheduleException e) {
                            LOGGER.warn("Fail to send iot command for device " + deviceInfo.getId(), e);
                        }
                    }
                }
            }
        }
    }
}
