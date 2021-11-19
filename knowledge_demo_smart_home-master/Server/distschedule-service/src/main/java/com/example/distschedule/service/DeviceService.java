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

import com.alibaba.fastjson.JSONObject;
import com.example.distschedule.dao.mapper.DeviceMapper;
import com.example.distschedule.dao.mapper.DeviceTypeMapper;
import com.example.distschedule.dao.model.Device;
import com.example.distschedule.dao.model.DeviceType;
import com.example.distschedule.dao.model.UserDevice;
import com.example.distschedule.dto.*;
import com.example.distschedule.enums.HandDeviceType;
import com.example.distschedule.error.ErrorCode;
import com.example.distschedule.exception.DistscheduleDeviceException;
import com.huaweicloud.sdk.core.exception.ConnectionException;
import com.huaweicloud.sdk.core.exception.RequestTimeoutException;
import com.huaweicloud.sdk.core.exception.ServiceResponseException;
import com.huaweicloud.sdk.iotda.v5.IoTDAClient;
import com.huaweicloud.sdk.iotda.v5.model.*;
import org.apache.commons.lang3.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.LinkedList;
import java.util.List;
import java.util.Optional;

@Service
public class DeviceService {
    private static final Logger LOGGER = LoggerFactory.getLogger(DeviceService.class);
    @Value("${cloud.iot.appId}")
    private String cloudIotAppId;
    @Autowired
    private DeviceMapper deviceMapper;
    @Autowired
    private DeviceTypeMapper deviceTypeMapper;
    @Autowired
    private UserService userService;
    @Autowired
    private IOTCloudService iotCloudService;

    public Optional<DeviceDto> getDeviceById(String deviceId) {
        Device device = deviceMapper.selectDeviceById(deviceId);

        if (device != null) {
            return Optional.of(new DeviceDto(device));
        }
        return Optional.empty();
    }

    @Transactional(rollbackFor = DistscheduleDeviceException.class)
    public String saveDevice(CreateDeviceDto createDeviceDto) throws DistscheduleDeviceException {
        //1.验证用户是否存在
        Optional<UserDto> creatorDto = userService.getUserById(createDeviceDto.getOwnerId());

        if (!creatorDto.isPresent()) {
            throw new DistscheduleDeviceException(ErrorCode.USER_NOT_FOUND);
        }

        //2. 获取设备类型
        DeviceType deviceType = deviceTypeMapper.selectDeviceTypeById(createDeviceDto.getTypeId());
        if (deviceType == null) {
            throw new DistscheduleDeviceException(ErrorCode.DEVICE_TYPE_NOT_EXIST_FAIL);
        }

        // 保存手持设备
        if (this.isHandDevice(createDeviceDto.getTypeId())) {
            return saveHandDevice(createDeviceDto);
        }

        //3.验证设备是否存在
        Device device = deviceMapper.selectDeviceById(createDeviceDto.getId());

        if (device != null) {
            throw new DistscheduleDeviceException(ErrorCode.DEVICE_EXIST_FAIL);
        }

        device = new Device();
        device.setId(createDeviceDto.getId());
        device.setName(createDeviceDto.getName());
        device.setOwnerId(createDeviceDto.getOwnerId());
        device.setTypeId(createDeviceDto.getTypeId());
        device.setState("NOTACTIVE");

        int res = deviceMapper.saveDevice(device);
        if (res == 0) {
            throw new DistscheduleDeviceException(ErrorCode.DEVICE_SAVE_FAIL);
        }

        //4.不是手持设备，则调用iot云保存设备
        AddDeviceRequest request = new AddDeviceRequest();
        AddDevice body = new AddDevice();

        body.setDeviceId(createDeviceDto.getId());
        body.setDeviceName(createDeviceDto.getId() + "_" + deviceType.getProductId());
        body.setNodeId(createDeviceDto.getId());
        body.setProductId(deviceType.getProductId());
        body.setAppId(cloudIotAppId);
        AuthInfo authInfo = new AuthInfo();
        authInfo.setAuthType("SECRET");//秘钥接入
        authInfo.setSecret(createDeviceDto.getSecret());
        body.setAuthInfo(authInfo);
        request.setBody(body);

        try {
            // 创建设备
            AddDeviceResponse response = iotCloudService.getIoTDAClient().addDevice(request);
            LOGGER.info(response.toString());
        } catch (ConnectionException e) {
            throw new DistscheduleDeviceException(e.getMessage(), ErrorCode.DEVICE_ADDIOTCLOUD_FAIL);
        } catch (RequestTimeoutException e) {
            throw new DistscheduleDeviceException(e.getMessage(), ErrorCode.DEVICE_ADDIOTCLOUD_FAIL);
        } catch (ServiceResponseException e) {
            throw new DistscheduleDeviceException(e.getMessage(), ErrorCode.DEVICE_ADDIOTCLOUD_FAIL);
        }
        return device.getId();
    }

    /**
     * 保存手持设备
     *
     * @return
     */
    private String saveHandDevice(CreateDeviceDto createDeviceDto) throws DistscheduleDeviceException {
        //3.验证设备是否存在
        Device device = deviceMapper.selectDeviceById(createDeviceDto.getId());

        if (device != null) {
            if (StringUtils.equals(device.getOwnerId(),createDeviceDto.getOwnerId())){
                return device.getId();
            }
            // 删除之前的设备
            deviceMapper.deleteDevice(createDeviceDto.getId());
        }

        device = new Device();
        device.setId(createDeviceDto.getId());
        device.setName(createDeviceDto.getName());
        device.setOwnerId(createDeviceDto.getOwnerId());
        device.setTypeId(createDeviceDto.getTypeId());
        device.setState("");

        int res = deviceMapper.saveDevice(device);
        if (res == 0) {
            throw new DistscheduleDeviceException(ErrorCode.DEVICE_SAVE_FAIL);
        }
        return device.getId();
    }

    public int shareDevice(String userId, String shareUserId, String deviceId) throws DistscheduleDeviceException {
        //0.自己的设备不能分享
        if (StringUtils.equals(userId, shareUserId)) {
            throw new DistscheduleDeviceException(ErrorCode.DEVICE_SHARE_OWNDEVICE_FAIL);
        }

        //1.获取设备
        Device device = deviceMapper.selectDeviceById(deviceId);
        if (device == null) {
            throw new DistscheduleDeviceException(ErrorCode.DEVICE_NOT_FOUND);
        }
        //2.权限校验
        if (!StringUtils.equals(device.getOwnerId(), userId)) {
            throw new DistscheduleDeviceException(ErrorCode.ILLEGAL_PERMISSION);
        }

        //3.查询是否已关联
        UserDevice userDevice = deviceMapper.getUserDevice(shareUserId, deviceId);
        if (userDevice != null) {
            return 1;//表示已存在，关联成功
        }

        //4.保存关联
        return deviceMapper.saveUserDevice(shareUserId, deviceId);
    }

    public int unshareDevice(String userId, String shareUserId, String deviceId) throws DistscheduleDeviceException {
        //1.获取设备
        Device device = deviceMapper.selectDeviceById(deviceId);
        if (device == null) {
            throw new DistscheduleDeviceException(ErrorCode.DEVICE_NOT_FOUND);
        }
        //2.权限校验
        if (!StringUtils.equals(device.getOwnerId(), userId)) {
            throw new DistscheduleDeviceException(ErrorCode.ILLEGAL_PERMISSION);
        }
        //3.查询是否已关联
        UserDevice userDevice = deviceMapper.getUserDevice(shareUserId, deviceId);
        if (userDevice == null) {
            return 1;//表示已不存在，取消关联成功
        }
        //4.取消关联
        return deviceMapper.deleteUserDevice(shareUserId, deviceId);
    }

    public List<DeviceDto> getDevicesByUserId(String userId) {
        List<Device> creartedDevices = deviceMapper.selectCreatedDevicesByUserId(userId);
        List<Device> sharedDevices = deviceMapper.selectSharedDevicesByUserId(userId);

        List<DeviceDto> devicesDtos = new LinkedList<>();

        for (Device device : creartedDevices) {
            DeviceDto deviceDto = new DeviceDto(device);
            devicesDtos.add(deviceDto);
        }

        for (Device device : sharedDevices) {
            DeviceDto deviceDto = new DeviceDto(device);
            devicesDtos.add(deviceDto);
        }
        return devicesDtos;
    }

    @Transactional(rollbackFor = DistscheduleDeviceException.class)
    public int deleteDevice(String userId, String deviceId) throws DistscheduleDeviceException {
        //1. 查设备
        Device device = deviceMapper.selectDeviceById(deviceId);
        if (device == null) {
            return 1;//找不到设备，删除成功，保证幂等性
        }
        //2. 验证权限
        if (!StringUtils.equals(device.getOwnerId(), userId)) {
            throw new DistscheduleDeviceException(ErrorCode.ILLEGAL_PERMISSION);
        }
        //3.删除设备分享
        deviceMapper.deleteUserDeviceByDeviceId(deviceId);

        //4.删除设备
        int res = deviceMapper.deleteDevice(deviceId);
        if (res == 0) {
            throw new DistscheduleDeviceException(ErrorCode.DEVICE_DELETE_FAIL);
        }

        //5.调用IOT云，删除设备
        if (!this.isHandDevice(device.getTypeId())) {
            IoTDAClient client = iotCloudService.getIoTDAClient();
            DeleteDeviceRequest request = new DeleteDeviceRequest();
            request.setDeviceId(deviceId);

            try {
                // 删除设备
                DeleteDeviceResponse response = client.deleteDevice(request);
                LOGGER.info("DeleteDeviceResponse:" + response.toString());
            } catch (ConnectionException e) {
                throw new DistscheduleDeviceException(e.getMessage(), ErrorCode.DEVICE_DELETEIOTCLOUD_FAIL);
            } catch (RequestTimeoutException e) {
                throw new DistscheduleDeviceException(e.getMessage(), ErrorCode.DEVICE_DELETEIOTCLOUD_FAIL);
            } catch (ServiceResponseException e) {
                if (!StringUtils.equals(e.getErrorCode(), "IOTDA.014000")) {//设备已不存在
                    throw new DistscheduleDeviceException(e.getMessage(), ErrorCode.DEVICE_DELETEIOTCLOUD_FAIL);
                }
            }
        }
        return 1;
    }

    public int sendCommand(String deviceId, DeviceCommandDto deviceCommandDto) throws DistscheduleDeviceException {
        CreateCommandRequest request = new CreateCommandRequest();
        DeviceCommandRequest body = new DeviceCommandRequest();
        body.setCommandName(deviceCommandDto.getCommandName());
        body.setServiceId(deviceCommandDto.getServiceId());
        try {
            body.setParas(JSONObject.parse(deviceCommandDto.getValue()));
        } catch (Exception e) {
            LOGGER.error(e.getMessage());
            throw new DistscheduleDeviceException("Invalid command value", ErrorCode.DEVICE_SEND_COMMAND_FAIL);
        }

        request.setDeviceId(deviceId);
        request.setBody(body);

        try {
            // 创建异步命令
            CreateCommandResponse response = iotCloudService.getIoTDAClient().createCommand(request);
            LOGGER.info("createCommand response= " + response.toString());
        } catch (ConnectionException e) {
            throw new DistscheduleDeviceException(e.getMessage(), ErrorCode.DEVICE_SEND_COMMAND_FAIL);
        } catch (RequestTimeoutException e) {
            throw new DistscheduleDeviceException(e.getMessage(), ErrorCode.DEVICE_SEND_COMMAND_FAIL);
        } catch (ServiceResponseException e) {
            throw new DistscheduleDeviceException(e.getMessage(), ErrorCode.DEVICE_SEND_COMMAND_FAIL);
        }
        return 0;
    }

    public int updateDeviceStatue(String deviceId, String status) {
        return deviceMapper.updateDeviceStatue(deviceId, status);
    }

    public List<UserDeviceDto> getShareUsersByDeviceId(String deviceId) {
        List<UserDeviceDto> userDeviceDtos = new LinkedList<>();
        List<UserDevice> userDevices = deviceMapper.selectShareUsersByDeviceId(deviceId);
        for (UserDevice userDevice : userDevices) {
            UserDeviceDto userDeviceDto = new UserDeviceDto(userDevice);
            Optional<UserDto> userDto = userService.getUserById(userDeviceDto.getUserId());
            if (userDto.isPresent()) {
                userDeviceDto.setPhone(userDto.get().getPhone());
                userDeviceDto.setUserName(userDto.get().getName());
            }
            userDeviceDtos.add(userDeviceDto);
        }
        return userDeviceDtos;
    }

    public int updateDeviceById(String userId, String deviceId, UpdateDeviceDto updateDeviceDto) throws DistscheduleDeviceException {
        Device oldDevice = deviceMapper.selectDeviceById(deviceId);
        if (oldDevice == null) {
            throw new DistscheduleDeviceException(ErrorCode.DEVICE_UPDATE_FAIL);
        }

        if (!StringUtils.equals(oldDevice.getOwnerId(), userId)) {
            throw new DistscheduleDeviceException(ErrorCode.ILLEGAL_PERMISSION);
        }

        oldDevice.setName(updateDeviceDto.getName());
        return deviceMapper.updateDevice(oldDevice);
    }

    public List<String> getUsersByDeviceId(String deviceId) throws DistscheduleDeviceException {
        List<String> userIds = new LinkedList<>();
        Device device = deviceMapper.selectDeviceById(deviceId);
        if (device == null) {
            throw new DistscheduleDeviceException(ErrorCode.DEVICE_NOT_FOUND);
        }
        userIds.add(device.getOwnerId());
        List<UserDevice> userDevices = deviceMapper.selectShareUsersByDeviceId(deviceId);
        for (UserDevice userDevice : userDevices) {
            userIds.add(userDevice.getUserId());
        }
        return userIds;
    }

    public int updateDeviceProperty(String deviceId, String property) {
        return deviceMapper.updateDeviceProperty(deviceId, property);
    }

    /**
     * 判断是否为手持设备
     *
     * @param type 设备类型
     * @return 是否为手持设备
     */
    private boolean isHandDevice(int type) {
        return HandDeviceType.getName(type) != null;
    }
}
