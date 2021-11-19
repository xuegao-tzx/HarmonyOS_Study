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


import com.example.distschedule.dto.ResponseResult;
import com.example.distschedule.dto.UserDeviceDto;
import com.example.distschedule.dto.UserDto;
import com.example.distschedule.error.ErrorCode;
import com.example.distschedule.exception.DistscheduleUserException;
import com.example.distschedule.service.UserService;
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

import javax.validation.constraints.NotNull;
import javax.validation.constraints.Size;
import java.util.Optional;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

@Api(tags = "用户接口")
@RestController
@Validated
@RequestMapping(value = "/user")
public class UserController {
    private static final Logger LOGGER = LoggerFactory.getLogger(UserController.class);

    @Autowired
    private UserService userService;

    @ApiOperation(value = "根据手机获取用户信息")
    @ApiImplicitParams({
            @ApiImplicitParam(name = "phone", value = "User telephone.Such as 13412345678", required = true, paramType = "path", dataTypeClass = String.class),
    })
    @GetMapping("/phone/{phone}")
    public ResponseResult<UserDto> getUserByPhone(@PathVariable("phone") String phone) {
        LOGGER.info("getUserByPhone:phone=" + phone);
        //手机号码正则表达式
        String str = "^[1][3,5,7,8][0-9]\\d{8}$";
        Pattern patternPhtone = Pattern.compile(str);
        Matcher matcherPhone = patternPhtone.matcher(phone);

        if (StringUtils.isBlank(phone) || !matcherPhone.matches()) {
            return ResponseResult.fail(ErrorCode.ILLEGAL_PARAM);
        }

        Optional<UserDto> userOptional = userService.getUserByPhone(phone, true);

        if (userOptional.isPresent()) {
            return ResponseResult.success(userOptional.get());
        }

        return ResponseResult.fail(ErrorCode.ILLEGAL_STATE);
    }

    @ApiOperation(value = "更新用户信息")
    @ApiImplicitParams({
            @ApiImplicitParam(name = "userId", value = "User Id", required = true, paramType = "path", dataTypeClass = String.class),
            @ApiImplicitParam(name = "name", value = "User name", required = true, paramType = "query", dataTypeClass = String.class),
    })
    @PutMapping("/{userId}")
    public ResponseResult<String> updateUserById(@Size(min = 36, max = 36, message = "illegal user id format") @PathVariable("userId") String userId,
                                                 @Size(min = 1, max = 32, message = "illegal name size, name size should less than 32")
                                                 @NotNull(message = "name should not empty")
                                                 @RequestParam(required = true) String name) {
        LOGGER.info("updateUserById:userId=" + userId + ",name=" + name);

        UserDto userDto = new UserDto();
        userDto.setId(userId);
        userDto.setName(name);
        int res = 0;
        try {
            res = userService.updateUser(userDto);
        } catch (DistscheduleUserException e) {
            LOGGER.error(e.getMessage());
            return ResponseResult.fail(e.getErrorCode());
        }

        if (res == 0) {
            return ResponseResult.fail(ErrorCode.USER_UPDATE_FAIL);
        } else {
            return ResponseResult.success("success");
        }
    }
}
