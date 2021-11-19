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

import com.example.distschedule.dao.mapper.UserMapper;
import com.example.distschedule.dao.model.User;
import com.example.distschedule.dto.ResponseResult;
import com.example.distschedule.dto.UserDeviceDto;
import com.example.distschedule.dto.UserDto;
import com.example.distschedule.error.ErrorCode;
import com.example.distschedule.exception.DistscheduleUserException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Optional;
import java.util.UUID;

@Service
public class UserService {
    private static final Logger logger = LoggerFactory.getLogger(UserService.class);

    @Autowired
    private UserMapper userMapper;

    public Optional<UserDto> getUserByPhone(String phone) {
        return getUserByPhone(phone, false);
    }

    public Optional<UserDto> getUserByPhone(String phone, Boolean isAutoCreate) {
        User user = userMapper.selectUserByPhone(phone);

        if (user != null) {
            return Optional.of(new UserDto(user));
        }

        //自动创建用户
        if (isAutoCreate) {
            User newUser = new User();
            newUser.setId(UUID.randomUUID().toString());
            newUser.setName(phone);
            newUser.setPhone(phone);
            userMapper.saveUser(newUser);

            return this.getUserByPhone(phone);
        }

        return Optional.empty();
    }


    public int saveUser(UserDto newUserDto) {
        User user = new User();
        user.setId(UUID.randomUUID().toString());
        user.setPhone(newUserDto.getPhone());
        user.setName(newUserDto.getName());
        return userMapper.saveUser(user);
    }

    public Optional<UserDto> getUserById(String userId) {
        User user = userMapper.selectUserByPId(userId);
        if (user == null) {
            return Optional.empty();
        }
        return Optional.of(new UserDto(user));
    }

    public int updateUser(UserDto userDto) throws DistscheduleUserException {
        User oldUser = userMapper.selectUserByPId(userDto.getId());

        if (oldUser == null) {
            throw new DistscheduleUserException(ErrorCode.USER_NOT_FOUND);
        }

        User user = new User();
        user.setId(oldUser.getId());
        user.setName(userDto.getName());
        user.setPhone(oldUser.getPhone());//暂不更新手机
        return userMapper.updateUser(user);
    }
}
