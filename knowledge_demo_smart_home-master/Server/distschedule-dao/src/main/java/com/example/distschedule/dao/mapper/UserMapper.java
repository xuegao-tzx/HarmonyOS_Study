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

import com.example.distschedule.dao.model.User;
import org.apache.ibatis.annotations.*;
import org.springframework.stereotype.Component;

import java.util.List;

@Mapper
@Component
public interface UserMapper {

    /**
     * 根据手机查询用户
     *
     * @return 用户列表
     */
    @Select("SELECT * FROM user WHERE phone = #{phone}")
    User selectUserByPhone(@Param("phone") String phone);

    /**
     * 创建用户
     * @param user 新用户
     * @return 是否创建成功
     */
    @Insert("INSERT INTO `user` (`id`,`phone`,`name`) VALUES (#{user.id},#{user.phone},#{user.name})")
    int saveUser(@Param("user") User user);

    /**
     * 根据用户Id查询用户
     *
     * @return 用户列表
     */
    @Select("SELECT * FROM user WHERE id = #{userId}")
    User selectUserByPId(String userId);

    @Update("UPDATE user SET name = #{name} , update_time = NOW()  WHERE id = #{id}")
    int updateUser(User user);
}
