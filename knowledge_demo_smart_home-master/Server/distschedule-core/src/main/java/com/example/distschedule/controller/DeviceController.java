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
import com.example.distschedule.exception.DistscheduleDeviceException;
import com.example.distschedule.service.DeviceService;
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
import java.util.List;
import java.util.Optional;

@Api(tags = "设备接口")
@RestController
@Validated
@RequestMapping(value = "/device")
public class DeviceController {
    private static final Logger LOGGER = LoggerFactory.getLogger(DeviceController.class);
    @Autowired
    private DeviceService deviceService;

    @ApiOperation(value = "根据用户id，查询用户的设备列表")
    @ApiImplicitParams({
    })
    @GetMapping("/userId/{userId}")
    public ResponseResult<List<DeviceDto>> getUserDevices(@PathVariable("userId") String userId) {
        LOGGER.info("getUserDevices:userId=" + userId);
        return ResponseResult.success(deviceService.getDevicesByUserId(userId));
    }

    @ApiOperation(value = "根据设备id，查询设备")
    @ApiImplicitParams({
            @ApiImplicitParam(name = "userId", value = "user id", required = true, paramType = "header", dataTypeClass = String.class),
    })
    @GetMapping("/{deviceId}")
    public ResponseResult<DeviceDto> getDevicesById(@PathVariable("deviceId") String deviceId,
                                                    @Size(min = 36, max = 36, message = "illegal user id format") @RequestHeader(name = "userId") String userId) {
        LOGGER.info("getDevicesById:userId=" + userId + ",deviceId=" + deviceId);
        Optional<DeviceDto> deviceDtoOptional = deviceService.getDeviceById(deviceId);
        if (deviceDtoOptional.isPresent()) {
            return ResponseResult.success(deviceDtoOptional.get());
        }
        return ResponseResult.fail(ErrorCode.DEVICE_NOT_FOUND);
    }

    @ApiOperation(value = "创建设备")
    @PostMapping
    public ResponseResult<DeviceDto> createDevice(@Valid @RequestBody CreateDeviceDto createDeviceDto) {
        LOGGER.info("createDevice:createDeviceDto=" + createDeviceDto);
        String deviceId = null;
        try {
            deviceId = deviceService.saveDevice(createDeviceDto);
        } catch (DistscheduleDeviceException e) {
            LOGGER.error(e.getMessage(), e);
            if (StringUtils.isBlank(e.getMessage())) {
                return ResponseResult.fail(e.getErrorCode());
            }
            return ResponseResult.fail(e.getMessage(), e.getErrorCode());
        }

        Optional<DeviceDto> newDeviceOptional = deviceService.getDeviceById(deviceId);
        if (newDeviceOptional.isPresent()) {
            return ResponseResult.success(newDeviceOptional.get());
        }

        return ResponseResult.fail(ErrorCode.ILLEGAL_STATE);
    }

    @ApiOperation(value = "删除设备")
    @ApiImplicitParams({
            @ApiImplicitParam(name = "userId", value = "device owner id", required = true, paramType = "header", dataTypeClass = String.class),
    })
    @DeleteMapping("/{deviceId}")
    public ResponseResult<String> deleteDeviceById(@PathVariable("deviceId") String deviceId,
                                                   @Size(min = 36, max = 36, message = "illegal user id format") @RequestHeader(name = "userId") String userId) {
        LOGGER.info("deleteDeviceById:userId=" + userId + ",deviceId=" + deviceId);
        int res = 0;
        try {
            res = deviceService.deleteDevice(userId, deviceId);
        } catch (DistscheduleDeviceException e) {
            if (StringUtils.isBlank(e.getMessage())) {
                return ResponseResult.fail(e.getErrorCode());
            }
            return ResponseResult.fail(e.getMessage(), e.getErrorCode());
        }

        if (res == 0) {
            return ResponseResult.fail(ErrorCode.DEVICE_DELETE_FAIL);
        }
        return ResponseResult.success("");
    }


    @ApiOperation(value = "更新设备")
    @ApiImplicitParams({
            @ApiImplicitParam(name = "userId", value = "device owner id", required = true, paramType = "header", dataTypeClass = String.class),
    })
    @PutMapping("/{deviceId}")
    public ResponseResult<String> updateDeviceById(
            @PathVariable("deviceId") String deviceId,
            @Valid @RequestBody UpdateDeviceDto updateDeviceDto,
            @Size(min = 36, max = 36, message = "illegal user id format") @RequestHeader(name = "userId") String userId) {
        LOGGER.info("updateDeviceById:userId=" + userId + ",deviceId=" + deviceId + ",updateDeviceDto=" + updateDeviceDto);
        int res = 0;
        try {
            res = deviceService.updateDeviceById(userId, deviceId, updateDeviceDto);
        } catch (DistscheduleDeviceException e) {
            LOGGER.error(e.getMessage(), e);
            return ResponseResult.fail(e.getErrorCode());
        }
        if (res == 0) {
            return ResponseResult.fail(ErrorCode.DEVICE_UPDATE_FAIL);
        } else {
            return ResponseResult.success("success");
        }
    }

    @ApiOperation(value = "共享设备")
    @ApiImplicitParams({
            @ApiImplicitParam(name = "userId", value = "user owner id", required = true, paramType = "header", dataTypeClass = String.class),
    })
    @PostMapping("/share")
    public ResponseResult<String> shareDevice(
            @Size(min = 36, max = 36, message = "illegal user id format") @RequestHeader String userId,
            @Size(min = 36, max = 36, message = "illegal shareUserId id format") @RequestParam("shareUserId") String shareUserId,
            @RequestParam("deviceId") String deviceId) {
        LOGGER.info("shareDevice:userId=" + userId + ",deviceId=" + deviceId + ",shareUserId=" + shareUserId);
        int res = 0;
        try {
            res = deviceService.shareDevice(userId, shareUserId, deviceId);
        } catch (DistscheduleDeviceException e) {
            LOGGER.error(e.getMessage(), e);
            return ResponseResult.fail(e.getErrorCode());
        }
        if (res == 0) {
            ResponseResult.fail(ErrorCode.DEVICE_SHARE_FAIL);
        }
        return ResponseResult.success("");
    }

    @ApiOperation(value = "取消共享设备")
    @ApiImplicitParams({
            @ApiImplicitParam(name = "userId", value = "user owner id", required = true, paramType = "header", dataTypeClass = String.class),
    })
    @PostMapping("/unshare")
    public ResponseResult<String> unshareDevice(
            @Size(min = 36, max = 36, message = "illegal user id format") @RequestHeader String userId,
            @Size(min = 36, max = 36, message = "illegal shareUserId id format") @RequestParam("shareUserId") String shareUserId,
            @RequestParam("deviceId") String deviceId) {
        LOGGER.info("unshareDevice:userId=" + userId + ",deviceId=" + deviceId + ",shareUserId=" + shareUserId);
        int res = 0;
        try {
            res = deviceService.unshareDevice(userId, shareUserId, deviceId);
        } catch (DistscheduleDeviceException e) {
            LOGGER.error(e.getMessage(), e);
            return ResponseResult.fail(e.getErrorCode());
        }
        if (res == 0) {
            ResponseResult.fail(ErrorCode.DEVICE_UNSHARE_FAIL);
        }
        return ResponseResult.success("");
    }

    @ApiOperation(value = "给设备发送命令")
    @PostMapping("/{deviceId}/sendCommand")
    public ResponseResult<String> sendDeviceCommand(
            @PathVariable("deviceId") String deviceId,
            @Valid @RequestBody DeviceCommandDto deviceCommandDto) {
        LOGGER.info("sendDeviceCommand:deviceId=" + deviceId + ",deviceCommandDto=" + deviceCommandDto);
        //1.记录发送日志

        //2.向IOT云发送命令
        try {
            deviceService.sendCommand(deviceId, deviceCommandDto);
        } catch (DistscheduleDeviceException e) {
            if (StringUtils.isBlank(e.getMessage())) {
                return ResponseResult.fail(e.getErrorCode());
            }
            return ResponseResult.fail(e.getMessage(), e.getErrorCode());
        }
        return ResponseResult.success("");
    }

    @ApiOperation(value = "根据设备id，查询已分享的用户")
    @ApiImplicitParams({
    })
    @GetMapping("/deviceId/{deviceId}/userShares")
    public ResponseResult<List<UserDeviceDto>> getShareUsersByDeviceId(@PathVariable("deviceId") String deviceId) {
        LOGGER.info("getShareUsersByDeviceId:deviceId=" + deviceId);

        List<UserDeviceDto> userDeviceDtos = deviceService.getShareUsersByDeviceId(deviceId);
        return ResponseResult.success(userDeviceDtos);
    }
}
