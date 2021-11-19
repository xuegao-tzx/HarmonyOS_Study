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
import com.example.distschedule.exception.DistscheduleFamilyException;
import com.example.distschedule.exception.DistschedulePermissionException;
import com.example.distschedule.exception.DistscheduleUserException;
import com.example.distschedule.service.FamilyService;
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

import javax.validation.Valid;
import javax.validation.constraints.NotNull;
import javax.validation.constraints.Size;

import java.util.List;
import java.util.Optional;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

@Api(tags = "家庭接口")
@RestController
@Validated
@RequestMapping(value = "/family")
public class FamilyController {
    private static final Logger LOGGER = LoggerFactory.getLogger(FamilyController.class);
    @Autowired
    private FamilyService familyService;

    @Autowired
    private UserService userService;

    @ApiOperation(value = "根据用户id查询家庭信息")
    @GetMapping("/userId/{userId}")
    public ResponseResult<List<FamilyDto>> getFamiliesByUserId(@Size(min = 36, max = 36, message = "illegal user id format") @PathVariable("userId") String userId) {
        LOGGER.info("getFamiliesByUserId:userId=" + userId);
        return ResponseResult.success(familyService.getFamiliesByUserId(userId));
    }

    @ApiOperation(value = "创建家庭")
    @ApiImplicitParams({
            @ApiImplicitParam(name = "userId", value = "family owner id", required = true, paramType = "header", dataTypeClass = String.class),
    })
    @PostMapping
    public ResponseResult<FamilyDto> createFamily(@Valid @RequestBody FamilyCreateDto familyCreateDto,
                                                  @Size(min = 36, max = 36, message = "illegal user id format") @RequestHeader(name = "userId") String userId) {
        LOGGER.info("createFamily:userId=" + userId + ",familyCreateDto=" + familyCreateDto.toString());
        String familyId = null;
        try {
            familyId = familyService.saveFamily(userId, familyCreateDto);
        } catch (DistscheduleUserException e) {
            LOGGER.error(e.getMessage(), e);
            return ResponseResult.fail(e.getErrorCode());
        }

        Optional<FamilyDto> newFamilyOptional = familyService.getFamilyById(familyId);
        if (newFamilyOptional.isPresent()) {
            return ResponseResult.success(newFamilyOptional.get());
        }

        return ResponseResult.fail(ErrorCode.ILLEGAL_STATE);
    }

    @ApiOperation(value = "删除家庭")
    @ApiImplicitParams({
            @ApiImplicitParam(name = "userId", value = "family owner id", required = true, paramType = "header", dataTypeClass = String.class),
    })
    @DeleteMapping("/{familyId}")
    public ResponseResult<String> deleteFamilyById(
            @Size(min = 36, max = 36, message = "illegal family id format") @PathVariable("familyId") String familyId,
            @Size(min = 36, max = 36, message = "illegal user id format") @RequestHeader(name = "userId") String userId) {
        LOGGER.info("deleteFamilyById:userId=" + userId + ",familyId=" + familyId);
        int res = 0;
        try {
            res = familyService.deleteFamily(userId, familyId);
        } catch (DistscheduleFamilyException e) {
            LOGGER.error(e.getMessage(), e);
            return ResponseResult.fail(e.getErrorCode());
        }

        if (res == 0) {
            return ResponseResult.fail(ErrorCode.FAMILY_DELETE_FAIL);
        }

        return ResponseResult.success("");
    }


    @ApiOperation(value = "更新家庭")
    @ApiImplicitParams({
            @ApiImplicitParam(name = "userId", value = "family owner id", required = true, paramType = "header", dataTypeClass = String.class),
    })
    @PutMapping("/{familyId}")
    public ResponseResult<String> updateFamilyById(
            @Size(min = 36, max = 36, message = "illegal family id format")
            @PathVariable("familyId") String familyId,
            @Size(min = 36, max = 36, message = "illegal user id format") @RequestHeader(name = "userId") String userId,
            @Valid @RequestBody UpdateFamilyDto updateFamilyDto) throws DistschedulePermissionException {
        LOGGER.info("updateFamilyById:userId=" + userId + ",familyId=" + familyId + ",updateFamilyDto=" + updateFamilyDto);
        int res = 0;
        try {
            res = familyService.updateFamily(userId, familyId, updateFamilyDto);
        } catch (DistscheduleFamilyException e) {
            LOGGER.error(e.getMessage(), e);
            return ResponseResult.fail(e.getErrorCode());
        }

        if (res == 0) {
            return ResponseResult.fail(ErrorCode.FAMILY_UPDATE_FAIL);
        } else {
            return ResponseResult.success("success");
        }
    }

    @ApiOperation(value = "创建家庭成员")
    @ApiImplicitParams({
            @ApiImplicitParam(name = "userId", value = "family owner id", required = true, paramType = "header", dataTypeClass = String.class),
    })
    @PostMapping("/{familyId}/member")
    public ResponseResult<String> createFamilyMember(@RequestParam(required = true) String phone,
                                                     @Size(min = 36, max = 36, message = "illegal family id format") @PathVariable(name = "familyId") String familyId,
                                                     @Size(min = 36, max = 36, message = "illegal user id format") @RequestHeader(name = "userId") String userId,
                                                     @Size(min = 1, max = 32, message = "illegal role size, role size should less than 32")
                                                     @NotNull(message = "role should not empty") @RequestParam(required = true) String role) {
        LOGGER.info("createFamilyMember:userId=" + userId + ",familyId=" + familyId + ",role=" + role);
        //手机号码正则表达式
        String str = "^[1][3,5,7,8][0-9]\\d{8}$";
        Pattern patternPhtone = Pattern.compile(str);
        Matcher matcherPhone = patternPhtone.matcher(phone);

        if (StringUtils.isBlank(phone) || !matcherPhone.matches()) {
            return ResponseResult.fail(ErrorCode.ILLEGAL_PARAM);
        }
        //保存关系
        int res = 0;
        try {
            res = familyService.saveFamilyMember(userId, familyId, phone, role);
        } catch (DistscheduleUserException e) {
            LOGGER.error(e.getMessage(), e);
            return ResponseResult.fail(e.getErrorCode());
        }

        if (res == 0) {
            return ResponseResult.fail(ErrorCode.FAMILY_MEMBER_SAVE_FAIL);
        } else {
            return ResponseResult.success("success");
        }
    }

    @ApiOperation(value = "删除家庭成员")
    @ApiImplicitParams({
            @ApiImplicitParam(name = "userId", value = "family owner id", required = true, paramType = "header", dataTypeClass = String.class),
    })
    @DeleteMapping("/{familyId}/member")
    public ResponseResult<String> deleteFamilyMember(
            @Size(min = 36, max = 36, message = "illegal family id format") @PathVariable("familyId") String familyId,
            @Size(min = 36, max = 36, message = "illegal user id format") @RequestHeader(name = "userId") String userId,
            @Size(min = 36, max = 36, message = "illegal familyMember id format") @RequestParam(name = "familyMemberId") String familyMemberId) {
        LOGGER.info("deleteFamilyMember:userId=" + userId + ",familyId=" + familyId + ",familyMemberId=" + familyMemberId);

        int res = 0;
        try {
            res = familyService.deleteFamilyMember(userId, familyMemberId, familyId);
        } catch (DistscheduleUserException e) {
            LOGGER.error(e.getMessage(), e);
            return ResponseResult.fail(e.getErrorCode());
        }

        if (res == 0) {
            return ResponseResult.fail(ErrorCode.FAMILY_MEMBER_DELETE_FAIL);
        }

        return ResponseResult.success("");
    }

    @ApiOperation(value = "查询家庭成员信息")
    @GetMapping("/{familyId}/member")
    public ResponseResult<List<UserFamilyDto>> getFamilyMembersById(@Size(min = 36, max = 36, message = "illegal family id format") @PathVariable("familyId") String familyId) {
        LOGGER.info("getFamilyMembersById:familyId=" + familyId);
        try {
            return ResponseResult.success(familyService.getFamilyMembersById(familyId));
        } catch (DistscheduleUserException e) {
            LOGGER.error(e.getMessage(), e);
            return ResponseResult.fail(e.getErrorCode());
        }
    }
}
