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
 
package com.example.distschedule.dao.mapper;

import com.example.distschedule.dao.model.Device;
import com.example.distschedule.dao.model.UserDevice;
import org.apache.ibatis.annotations.*;
import org.springframework.stereotype.Component;

import java.util.List;

@Mapper
@Component
public interface DeviceMapper {

    @Select("SELECT device.id,device.type_id,device.owner_id,device.name,device.state,device.create_time,device.update_time,device.property,user.name as owner_name,user.phone as owner_phone FROM device,user WHERE device.id = #{deviceId} and device.owner_id=user.id")
    Device selectDeviceById(String deviceId);

    @Insert("INSERT INTO `device` (`id`,`name`,`type_id`,`owner_id`,`state`) VALUES (#{device.id},#{device.name},#{device.typeId},#{device.ownerId},#{device.state})")
    int saveDevice(@Param("device") Device device);

    @Select("SELECT * FROM user_device WHERE user_id = #{userId} AND device_id = #{deviceId}")
    UserDevice getUserDevice(@Param("userId") String userId, @Param("deviceId") String deviceId);

    @Insert("INSERT INTO `user_device` (`user_id`,`device_id`) VALUES (#{userId},#{deviceId})")
    int saveUserDevice(@Param("userId") String userId, @Param("deviceId") String deviceId);

    @Delete("DELETE from user_device where user_id = #{userId} AND device_id = #{deviceId}")
    int deleteUserDevice(@Param("userId") String userId, @Param("deviceId") String deviceId);

    @Select("SELECT device.id,device.type_id,device.owner_id,device.name,device.state,device.create_time,device.update_time,device.property,user.name as owner_name,user.phone as owner_phone FROM device,user WHERE device.owner_id = #{userId} AND user.id=device.owner_id")
    List<Device> selectCreatedDevicesByUserId(String userId);

    @Select("SELECT device.id,device.type_id,device.owner_id,device.name,device.state,device.create_time,device.update_time,device.property,user.name as owner_name,user.phone as owner_phone FROM user_device,device,user WHERE user_device.user_id = #{userId} and user_device.device_id=device.id and user.id = device.owner_id")
    List<Device> selectSharedDevicesByUserId(String userId);

    @Delete("DELETE from user_device where device_id = #{deviceId}")
    int deleteUserDeviceByDeviceId(String deviceId);

    @Delete("DELETE from device where id = #{deviceId}")
    int deleteDevice(String deviceId);

    @Update("UPDATE device SET state = #{state} , update_time = NOW()  WHERE id = #{deviceId}")
    int updateDeviceStatue(@Param("deviceId") String deviceId, @Param("state") String state);

    @Select("SELECT * FROM user_device WHERE device_id = #{deviceId}")
    List<UserDevice> selectShareUsersByDeviceId(String deviceId);

    @Update("UPDATE device SET name = #{device.name} , update_time = NOW()  WHERE id = #{device.id}")
    int updateDevice(@Param("device") Device device);

    @Update("UPDATE device SET property = #{property} WHERE id = #{deviceId}")
    int updateDeviceProperty(@Param("deviceId") String deviceId, @Param("property") String property);
}
