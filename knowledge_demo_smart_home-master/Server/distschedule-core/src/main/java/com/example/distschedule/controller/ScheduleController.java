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
 
package com.example.distschedule.controller;

import com.example.distschedule.dto.*;
import com.example.distschedule.error.ErrorCode;
import com.example.distschedule.exception.DistscheduleScheduleException;
import com.example.distschedule.service.ScheduleService;
import io.swagger.annotations.Api;
import io.swagger.annotations.ApiImplicitParam;
import io.swagger.annotations.ApiImplicitParams;
import io.swagger.annotations.ApiOperation;
import org.apache.commons.lang3.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.validation.annotation.Validated;
import org.springframework.web.bind.annotation.*;

import javax.validation.Valid;
import javax.validation.constraints.Size;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.*;

@Api(tags = "日程接口")
@RestController
@Validated
@RequestMapping(value = "/schedule")
public class ScheduleController {
    private static final Logger LOGGER = LoggerFactory.getLogger(ScheduleController.class);
    @Autowired
    private ScheduleService scheduleService;

    @ApiOperation(value = "创建日程")
    @PostMapping
    public ResponseResult<ScheduleDto> createSchedule(@Valid @RequestBody CreateScheduleDto createScheduleDto) {
        LOGGER.info("createSchedule:createScheduleDto=" + createScheduleDto);

        String scheduleId = null;
        try {
            scheduleId = scheduleService.saveSchedule(createScheduleDto);
        } catch (DistscheduleScheduleException e) {
            if (StringUtils.isBlank(e.getMessage())) {
                return ResponseResult.fail(e.getErrorCode());
            }
            return ResponseResult.fail(e.getMessage(), e.getErrorCode());
        }


        Optional<ScheduleDto> newScheduleOptional = scheduleService.getScheduleById(scheduleId);
        if (newScheduleOptional.isPresent()) {
            return ResponseResult.success(newScheduleOptional.get());
        }

        return ResponseResult.fail(ErrorCode.ILLEGAL_STATE);
    }

    @ApiOperation(value = "删除日程")
    @ApiImplicitParams({
            @ApiImplicitParam(name = "userId", value = "schedule creator id", required = true, paramType = "header", dataTypeClass = String.class),
    })
    @DeleteMapping("/{scheduleId}")
    public ResponseResult<String> deleteScheduleById(
            @Size(min = 36, max = 36, message = "illegal schedule id format") @PathVariable("scheduleId")
                    String scheduleId,
            @Size(min = 36, max = 36, message = "illegal user id format") @RequestHeader(name = "userId") String userId) {
        LOGGER.info("deleteScheduleById:userId=" + userId + ",scheduleId=" + scheduleId);
        int res = 0;
        try {
            res = scheduleService.deleteSchedule(userId, scheduleId);
        } catch (DistscheduleScheduleException e) {
            LOGGER.error(e.getMessage(), e);
            return ResponseResult.fail(e.getErrorCode());
        }

        if (res == 0) {
            return ResponseResult.fail(ErrorCode.SCHEDULE_DELETE_FAIL);
        }
        return ResponseResult.success("");
    }


    @ApiOperation(value = "更新日程")
    @ApiImplicitParams({
            @ApiImplicitParam(name = "userId", value = "creator owner id", required = true, paramType = "header", dataTypeClass = String.class),
    })
    @PutMapping("/{scheduleId}")
    public ResponseResult<String> updateScheduleById(
            @Size(min = 36, max = 36, message = "illegal schedule id format") @PathVariable("scheduleId") String scheduleId,
            @Size(min = 36, max = 36, message = "illegal user id format") @RequestHeader(name = "userId") String userId,
            @Valid @RequestBody CreateScheduleDto updateScheduleDto) {
        LOGGER.info("updateScheduleById:userId=" + userId + ",scheduleId=" + scheduleId + ",updateScheduleDto=" + updateScheduleDto);
        int res = 0;
        try {
            res = scheduleService.updateSchedule(userId, scheduleId, updateScheduleDto);
        } catch (DistscheduleScheduleException e) {
            LOGGER.error(e.getMessage(), e);
            return ResponseResult.fail(e.getErrorCode());
        }

        if (res == 0) {
            return ResponseResult.fail(ErrorCode.SCHEDULE_UPDATE_FAIL);
        } else {
            return ResponseResult.success("success");
        }
    }

    @ApiOperation(value = "根据id查询日程")
    @ApiImplicitParams({
            @ApiImplicitParam(name = "userId", value = "creator owner id", required = true, paramType = "header", dataTypeClass = String.class),
    })
    @GetMapping("/{scheduleId}")
    public ResponseResult<ScheduleDto> getScheduleById(@Size(min = 36, max = 36, message = "illegal schedule id format") @PathVariable("scheduleId") String scheduleId) {
        LOGGER.info("getScheduleById:scheduleId=" + scheduleId);
        Optional<ScheduleDto> scheduleDtoOptional = scheduleService.getScheduleById(scheduleId);
        if (scheduleDtoOptional.isPresent()) {
            return ResponseResult.success(scheduleDtoOptional.get());
        }
        return ResponseResult.fail(ErrorCode.SCHEDULE_NOT_FOUND);
    }

    @ApiOperation(value = "时间范围查询日程")
    @ApiImplicitParams({
            @ApiImplicitParam(name = "userId", value = "creator owner id", required = true, paramType = "header", dataTypeClass = String.class),
    })
    @GetMapping("/query")
    public ResponseResult<List<ScheduleDto>> querySchedules(@RequestParam("startTime") String startTimeStr, @RequestParam("endTime") String endTimeStr,
                                                            @Size(min = 36, max = 36, message = "illegal user id format") @RequestHeader(name = "userId") String userId) {
        LOGGER.info("querySchedules:userId=" + userId + ",startTimeStr=" + startTimeStr + ", endTimeStr = " + endTimeStr);
        SimpleDateFormat simpleDateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        Date startTime = null;
        Date endTime = null;
        try {
            startTime = simpleDateFormat.parse(startTimeStr);
            endTime = simpleDateFormat.parse(endTimeStr);
        } catch (ParseException e) {
            LOGGER.error(e.getMessage(), e);
            return ResponseResult.fail(ErrorCode.ILLEGAL_PARAM);
        }
        return ResponseResult.success(scheduleService.queryScheduleByUser(userId, startTime, endTime));
    }

    @ApiOperation(value = "按名称搜索日程")
    @ApiImplicitParams({
            @ApiImplicitParam(name = "userId", value = "creator user id", required = true, paramType = "header", dataTypeClass = String.class),
    })
    @GetMapping("/search")
    public ResponseResult<List<ScheduleDto>> searchSchedulesByName(@RequestParam("name") String name,
                                                                   @Size(min = 36, max = 36, message = "illegal user id format") @RequestHeader(name = "userId") String userId) {
        LOGGER.info("searchSchedulesByName:userId=" + userId + ",name=" + name);
        return ResponseResult.success(scheduleService.searchSchedulesByName(userId, name));
    }

    @ApiOperation(value = "删除当前用户所有日程(临时联调时接口)")
    @ApiImplicitParams({
            @ApiImplicitParam(name = "userId", value = "schedule creator id", required = true, paramType = "header", dataTypeClass = String.class),
    })
    @DeleteMapping("/deleteUserSchedule")
    public ResponseResult<String> deleteUserSchedule(
            @Size(min = 36, max = 36, message = "illegal user id format") @RequestHeader(name = "userId") String userId) {
        LOGGER.info("deleteUserSchedule:userId=" + userId);

        scheduleService.deleteUserAllSchedules(userId);

        return ResponseResult.success("");
    }
}
