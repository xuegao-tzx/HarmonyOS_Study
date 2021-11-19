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

import com.example.distschedule.dao.mapper.FamilyMapper;
import com.example.distschedule.dao.model.Family;
import com.example.distschedule.dao.model.UserFamily;
import com.example.distschedule.dto.*;
import com.example.distschedule.error.ErrorCode;
import com.example.distschedule.exception.DistscheduleFamilyException;
import com.example.distschedule.exception.DistschedulePermissionException;
import com.example.distschedule.exception.DistscheduleUserException;
import org.apache.commons.lang3.StringUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.LinkedList;
import java.util.List;
import java.util.Optional;
import java.util.UUID;

@Service
public class FamilyService {
    private static final Logger LOGGER = LoggerFactory.getLogger(FamilyService.class);
    @Autowired
    private FamilyMapper familyMapper;
    @Autowired
    private UserService userService;

    public List<FamilyDto> getFamiliesByUserId(String userId) {
        List<Family> createdFamilies = familyMapper.selectCreatedFamiliesByUserId(userId);
        List<Family> memberFamilies = familyMapper.selectJoinedFamiliesByUserId(userId);

        List<FamilyDto> familyDtos = new LinkedList<>();

        for (Family family : createdFamilies) {
            FamilyDto familyDto = new FamilyDto(family);
            familyDto.setCreator(true);
            familyDtos.add(familyDto);
        }

        for (Family family : memberFamilies) {
            FamilyDto familyDto = new FamilyDto(family);
            familyDto.setCreator(false);
            familyDtos.add(familyDto);
        }

        return familyDtos;
    }

    @Transactional
    public int deleteFamily(String userId, String familyId) throws DistscheduleFamilyException {
        //1.校验家庭是否存在，不存在表示删除成功
        Family family = familyMapper.selectFamilyById(familyId);
        if (family == null) {
            return 1;
        }
        //2.验证权限
        if (!StringUtils.equals(family.getCreatorId(), userId)) {
            throw new DistscheduleFamilyException(ErrorCode.ILLEGAL_PERMISSION);
        }

        //3.删除家庭成员，并验证原子性
        familyMapper.deleteFamilyMembers(familyId);

        //4.删除家庭
        return familyMapper.deleteFamily(userId, familyId);
    }

    public String saveFamily(String userId, FamilyCreateDto familyCreateDto) throws DistscheduleUserException {
        Optional<UserDto> creatorDto = userService.getUserById(userId);

        if (!creatorDto.isPresent()) {
            throw new DistscheduleUserException(ErrorCode.USER_NOT_FOUND);
        }
        // 当前禁止一个人有多个家庭
        List<Family> createdFamilies = familyMapper.selectCreatedFamiliesByUserId(userId);
        List<Family> memberFamilies = familyMapper.selectJoinedFamiliesByUserId(userId);
        if (createdFamilies.size() > 0 || memberFamilies.size() > 0) {
            throw new DistscheduleUserException(ErrorCode.FAMILY_EXIST);
        }

        Family family = new Family();
        family.setId(UUID.randomUUID().toString());
        family.setCreatorId(userId);
        family.setName(familyCreateDto.getName());

        int res = familyMapper.saveFamily(family);
        if (res != 0) {
            return family.getId();
        }
        return "";
    }

    public Optional<FamilyDto> getFamilyById(String familyId) {
        Family family = familyMapper.selectFamilyById(familyId);
        if (family == null) {
            return Optional.empty();
        }
        return Optional.of(new FamilyDto(family));
    }

    public int updateFamily(String userId, String familyId, UpdateFamilyDto familyDto) throws DistschedulePermissionException, DistscheduleFamilyException {
        Family oldFamily = familyMapper.selectFamilyById(familyId);
        if (oldFamily == null) {
            throw new DistscheduleFamilyException(ErrorCode.FAMILY_NOT_FOUND);
        }

        if (!StringUtils.equals(oldFamily.getCreatorId(), userId)) {
            throw new DistschedulePermissionException(ErrorCode.ILLEGAL_PERMISSION);
        }

        Family newFamily = new Family();
        newFamily.setId(familyId);
        newFamily.setName(familyDto.getName());
        newFamily.setCreatorId(userId);
        return familyMapper.updateFamily(newFamily);
    }

    public int saveFamilyMember(String userId, String familyId, String phone, String role) throws DistscheduleUserException {
        Optional<UserDto> userOptional = userService.getUserByPhone(phone, true);

        if (!userOptional.isPresent()) {
            throw new DistscheduleUserException(ErrorCode.USER_NOT_FOUND);
        }
        UserDto user = userOptional.get();

        Optional<FamilyDto> familyOptional = this.getFamilyById(familyId);

        if (!familyOptional.isPresent()) {
            throw new DistscheduleUserException(ErrorCode.FAMILY_NOT_FOUND);
        }
        FamilyDto family = familyOptional.get();

        //权限校验
        if (!StringUtils.equals(userId, family.getCreatorId())) {
            throw new DistscheduleUserException(ErrorCode.ILLEGAL_PERMISSION);
        }

        // 创建者不能再建立关系
        if (StringUtils.equals(user.getId(), family.getCreatorId())) {
            throw new DistscheduleUserException(ErrorCode.FAMILY_MEMBER_CANNOT_CREATOR_FAIL);
        }

        UserFamily userFamily = familyMapper.getUserFamily(user.getId(), familyId);

        if (userFamily == null) {
            return familyMapper.saveUserFamily(user.getId(), familyId, role);
        } else {
            return familyMapper.updateUserFamily(user.getId(), familyId, role);
        }
    }

    public int deleteFamilyMember(String userId, String familyMemberId, String familyId) throws DistscheduleUserException {
        Optional<FamilyDto> familyOptional = this.getFamilyById(familyId);

        if (!familyOptional.isPresent()) {
            throw new DistscheduleUserException(ErrorCode.FAMILY_NOT_FOUND);
        }
        FamilyDto family = familyOptional.get();

        //权限校验
        if (!StringUtils.equals(userId, family.getCreatorId())) {
            throw new DistscheduleUserException(ErrorCode.ILLEGAL_PERMISSION);
        }

        UserFamily userFamily = familyMapper.getUserFamily(familyMemberId, familyId);
        if (userFamily == null) {
            return 1;
        } else {
            return familyMapper.deleteFamilyMember(familyMemberId, familyId);
        }
    }

    public List<UserFamilyDto> getFamilyMembersById(String familyId) throws DistscheduleUserException {
        UserFamily creatorFamily = familyMapper.selectCreatorFamily(familyId);

        if (creatorFamily == null) {
            throw new DistscheduleUserException(ErrorCode.FAMILY_NOT_FOUND);
        }
        List<UserFamily> userFamilies = familyMapper.selectFamilyMembersById(familyId);

        List<UserFamilyDto> userFamilyDtos = new LinkedList<>();
        UserFamilyDto creatorUserFamilyDto = new UserFamilyDto(creatorFamily);
        creatorUserFamilyDto.setRole("Creator");
        userFamilyDtos.add(creatorUserFamilyDto);

        for (UserFamily userFamily : userFamilies) {
            UserFamilyDto userFamilyDto = new UserFamilyDto(userFamily);
            userFamilyDtos.add(userFamilyDto);
        }

        return userFamilyDtos;
    }
}
