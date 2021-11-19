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

import com.example.distschedule.dao.model.Family;
import com.example.distschedule.dao.model.UserFamily;
import org.apache.ibatis.annotations.*;
import org.springframework.stereotype.Component;

import java.util.List;

@Mapper
@Component
public interface FamilyMapper {
    /**
     * 根据用户Id,查询用户创建的家庭
     *
     * @return 家庭列表
     */
    @Select("SELECT id,name,creator_id,create_time,update_time FROM family WHERE creator_id = #{userId}")
    List<Family> selectCreatedFamiliesByUserId(String userId);

    /**
     * 根据用户Id,查询用户加入的家庭
     *
     * @return 家庭列表
     */
    @Select("SELECT family.id,family.name,family.creator_id,family.create_time,family.update_time FROM family,user_family WHERE user_family.user_id = #{userId} and  user_family.family_id = family.id")
    List<Family> selectJoinedFamiliesByUserId(String userId);

    @Delete("DELETE from family where id = #{familyId} and creator_id = #{userId}")
    int deleteFamily(@Param("userId") String userId, @Param("familyId") String familyId);

    @Select("SELECT id,name,creator_id,create_time,update_time FROM family WHERE id = #{familyId}")
    Family selectFamilyById(String familyId);

    @Delete("DELETE from user_family where family_id = #{familyId}")
    int deleteFamilyMembers(String familyId);

    @Insert("INSERT INTO `family` (`id`,`name`,`creator_id`) VALUES (#{family.id},#{family.name},#{family.creatorId})")
    int saveFamily(@Param("family") Family family);

    @Update("UPDATE family SET name = #{name} , update_time = NOW()  WHERE id = #{id}")
    int updateFamily(Family family);

    @Select("SELECT id,user_id,family_id FROM user_family WHERE user_id = #{userId} AND family_id = #{familyId}")
    UserFamily getUserFamily(@Param("userId")String userId, @Param("familyId")String familyId);

    @Insert("INSERT INTO `user_family` (`user_id`,`family_id`,`role`) VALUES (#{userId},#{familyId},#{role})")
    int saveUserFamily(@Param("userId")String userId, @Param("familyId")String familyId, @Param("role")String role);

    @Update("UPDATE user_family SET role = #{role} WHERE user_id = #{userId} AND family_id = #{familyId}")
    int updateUserFamily(@Param("userId")String userId, @Param("familyId")String familyId, @Param("role")String role);

    @Delete("DELETE from user_family where user_id = #{userId} AND family_id = #{familyId}")
    int deleteFamilyMember(@Param("userId")String userId, @Param("familyId")String familyId);

    @Select("SELECT user_family.id,user_family.user_id,user_family.family_id,user_family.role,user.phone as user_phone,user.name as user_name FROM user_family left join user on user_family.user_id=user.id WHERE family_id = #{familyId}")
    List<UserFamily> selectFamilyMembersById(String familyId);

    @Select("SELECT family.id as family_id,user.phone as user_phone,user.name as user_name,user.id as user_id FROM family left join user on family.creator_id=user.id WHERE family.id = #{familyId}")
    UserFamily selectCreatorFamily(String familyId);
}
