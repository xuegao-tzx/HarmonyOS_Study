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

import com.example.distschedule.dao.mapper.DeviceTypeMapper;
import com.example.distschedule.dao.model.DeviceType;
import com.example.distschedule.dto.DeviceTypeDto;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;

@Service
public class DeviceTypeService {
    private static final Logger LOGGER = LoggerFactory.getLogger(DeviceTypeService.class);

    @Autowired
    private DeviceTypeMapper deviceTypeMapper;

    public List<DeviceTypeDto> getDeviceTypes() {
        List<DeviceType> deviceTypes = deviceTypeMapper.selectDeviceTypes();
        return deviceTypes.stream().map(deviceType -> new DeviceTypeDto(deviceType)).collect(Collectors.toList());
    }

    public Optional<DeviceTypeDto> getDeviceTypesByProductId(String productId) {
        DeviceType deviceType = deviceTypeMapper.selectDeviceTypeByProductId(productId);
        if (deviceType == null) {
            return Optional.empty();
        }
        return Optional.of(new DeviceTypeDto(deviceType));
    }

    public Optional<DeviceTypeDto> getDeviceTypeById(int deviceTypeId) {
        DeviceType deviceType = deviceTypeMapper.selectDeviceTypeById(deviceTypeId);
        if (deviceType == null) {
            return Optional.empty();
        }
        return Optional.of(new DeviceTypeDto(deviceType));
    }
}
