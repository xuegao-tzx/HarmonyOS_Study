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

import com.example.distschedule.dao.model.Schedule;
import com.example.distschedule.dao.model.UserSchedule;
import org.apache.ibatis.annotations.*;
import org.springframework.stereotype.Component;

import java.util.Date;
import java.util.List;

@Mapper
@Component
public interface ScheduleMapper {
    @Select("SELECT  schedule.`name`,schedule.id,`schedule`.device_config,`schedule`.remind_day,`schedule`.creator_id,`schedule`.scene_id,`schedule`.state_detail,`schedule`.start_time,`schedule`.end_time,`schedule`.create_time,`schedule`.update_time,GROUP_CONCAT(CONCAT_WS(',',user_schedule.user_id) SEPARATOR ',') user_ids FROM schedule,user_schedule WHERE schedule.id = #{scheduleId} and user_schedule.schedule_id=schedule.id GROUP BY `schedule`.id")
    Schedule selectScheduleById(String scheduleId);

    @Insert("INSERT INTO `schedule` (`id`,`name`,`device_config`,`remind_day`,`creator_id`,`scene_id`,`state_detail`,`start_time`,`end_time`) " +
            "VALUES (#{schedule.id},#{schedule.name},#{schedule.deviceConfig},#{schedule.remindDay},#{schedule.creatorId},#{schedule.sceneId},#{schedule.stateDetail},#{schedule.startTime},#{schedule.endTime})")
    int saveSchedule(@Param("schedule") Schedule schedule);

    @Delete("DELETE from schedule where id = #{scheduleId}")
    int deleteSchedule(String scheduleId);

    @Update("UPDATE schedule SET name=#{schedule.name},device_config=#{schedule.deviceConfig},remind_day=#{schedule.remindDay}," +
            "creator_id=#{schedule.creatorId},scene_id=#{schedule.sceneId},state_detail=#{schedule.stateDetail},start_time=#{schedule.startTime},end_time=#{schedule.endTime},update_time=NOW() WHERE id = #{schedule.id}")
    int updateSchedule(@Param("schedule") Schedule schedule);

    @Select("SELECT  schedule.`name`,schedule.id,`schedule`.device_config,`schedule`.remind_day,`schedule`.creator_id,`schedule`.scene_id,`schedule`.state_detail,`schedule`.start_time,`schedule`.end_time,`schedule`.create_time,`schedule`.update_time,GROUP_CONCAT(CONCAT_WS(',',user_schedule.user_id) SEPARATOR ',') user_ids FROM schedule,user_schedule WHERE schedule.remind_day='' and user_schedule.user_id = #{userId} and user_schedule.schedule_id=schedule.id and start_time>=#{startTime} and start_time<=#{endTime} GROUP BY `schedule`.id ")
    List<Schedule> selectDefaultSchedulesByDatesAndUser(@Param("userId") String userId, @Param("startTime") Date startTime, @Param("endTime") Date endTime);

    @Select("SELECT  schedule.`name`,schedule.id,`schedule`.device_config,`schedule`.remind_day,`schedule`.creator_id,`schedule`.scene_id,`schedule`.state_detail,`schedule`.start_time,`schedule`.end_time,`schedule`.create_time,`schedule`.update_time,GROUP_CONCAT(CONCAT_WS(',',user_schedule.user_id) SEPARATOR ',') user_ids FROM schedule,user_schedule WHERE remind_day!='' and user_schedule.user_id = #{userId} and user_schedule.schedule_id=schedule.id GROUP BY `schedule`.id ")
    List<Schedule> selectNotDefaultSchedulesByUser(String userId);

    @Insert({
            "<script>",
            "insert into user_schedule(user_id,schedule_id) values ",
            "<foreach collection='userSchedules' item='item' index='index' separator=','>",
            "(#{item.userId}, #{item.scheduleId})",
            "</foreach>",
            "</script>"
    })
    int saveUserSchedule(@Param("userSchedules") List<UserSchedule> userSchedules);

    @Delete("DELETE from user_schedule where schedule_id = #{scheduleId}")
    void deleteUserSchedule(@Param("scheduleId") String scheduleId);

    @Select("SELECT  schedule.`name`,schedule.id,`schedule`.device_config,`schedule`.remind_day,`schedule`.creator_id,`schedule`.scene_id,`schedule`.state_detail,`schedule`.start_time,`schedule`.end_time,`schedule`.create_time,`schedule`.update_time,GROUP_CONCAT(CONCAT_WS(',',user_schedule.user_id) SEPARATOR ',') user_ids FROM schedule,user_schedule WHERE schedule.remind_day='' and user_schedule.user_id = #{userId} and user_schedule.schedule_id=schedule.id and start_time>=#{startTime} and start_time<=#{endTime} and schedule.name like concat('%',#{name},'%') GROUP BY `schedule`.id \n")
    List<Schedule> searchDefaultSchedulesByName(@Param("userId") String userId, @Param("startTime") Date startTime, @Param("endTime") Date endTime, @Param("name") String name);

    @Select("SELECT  schedule.`name`,schedule.id,`schedule`.device_config,`schedule`.remind_day,`schedule`.creator_id,`schedule`.scene_id,`schedule`.state_detail,`schedule`.start_time,`schedule`.end_time,`schedule`.create_time,`schedule`.update_time,GROUP_CONCAT(CONCAT_WS(',',user_schedule.user_id) SEPARATOR ',') user_ids FROM schedule,user_schedule WHERE remind_day!='' and creator_id = #{userId} and user_schedule.schedule_id=schedule.id and schedule.name like concat('%',#{name},'%') GROUP BY `schedule`.id")
    List<Schedule> searchNotDefaultSchedulesByName(@Param("userId") String userId, @Param("name") String name);

    @Delete("DELETE from schedule where creator_id = #{userId}")
    void deleteSchedulesByUserId(String userId);

    @Delete("DELETE from user_schedule where user_id = #{userId}")
    void deleteUserScheduleByUserId(String userId);
}
