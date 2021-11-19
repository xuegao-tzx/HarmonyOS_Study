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


import com.example.distschedule.dao.model.DeviceType;
import org.apache.ibatis.annotations.Mapper;
import org.apache.ibatis.annotations.Select;
import org.springframework.stereotype.Component;

import java.util.List;


@Mapper
@Component
public interface DeviceTypeMapper {
    @Select("SELECT * FROM device_type WHERE id = #{deviceTypeId}")
    DeviceType selectDeviceTypeById(int deviceTypeId);

    @Select("SELECT * FROM device_type")
    List<DeviceType> selectDeviceTypes();

    @Select("SELECT * FROM device_type WHERE product_id = #{productId}")
    DeviceType selectDeviceTypeByProductId(String productId);
}
