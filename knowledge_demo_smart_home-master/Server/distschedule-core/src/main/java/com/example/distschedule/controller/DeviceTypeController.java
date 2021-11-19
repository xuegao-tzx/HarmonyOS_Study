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


import com.example.distschedule.dto.DeviceTypeDto;
import com.example.distschedule.dto.ResponseResult;
import com.example.distschedule.error.ErrorCode;
import com.example.distschedule.service.DeviceTypeService;
import io.swagger.annotations.Api;
import io.swagger.annotations.ApiImplicitParams;
import io.swagger.annotations.ApiOperation;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.validation.annotation.Validated;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;
import java.util.Optional;

@Api(tags = "设备类型接口")
@RestController
@Validated
@RequestMapping(value = "/devicetype")
public class DeviceTypeController {
    private static final Logger LOGGER = LoggerFactory.getLogger(DeviceTypeController.class);
    @Autowired
    private DeviceTypeService deviceTypeService;

    @ApiOperation(value = "查询设备类型")
    @ApiImplicitParams({
    })
    @GetMapping
    public ResponseResult<List<DeviceTypeDto>> getDeviceTypes() {
        LOGGER.info("getDeviceTypes:");
        return ResponseResult.success(deviceTypeService.getDeviceTypes());
    }

    @ApiOperation(value = "根据ProductID查询设备类型")
    @ApiImplicitParams({
    })
    @GetMapping("/productId/{productId}")
    public ResponseResult<DeviceTypeDto> getDeviceTypesByProductId(@PathVariable(name = "productId") String productId) {
        LOGGER.info("getDeviceTypesByProductId:productId=" + productId);

        Optional<DeviceTypeDto> deviceTypeDto =deviceTypeService.getDeviceTypesByProductId(productId);
        if (deviceTypeDto.isPresent()){
            return ResponseResult.success(deviceTypeDto.get());
        }
        return ResponseResult.fail(ErrorCode.DEVICE_TYPE_NOT_EXIST_FAIL);
    }
}
