/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.example.distschedule.rx2;

import com.example.distschedule.bean.*;
import io.reactivex.Observable;
import okhttp3.RequestBody;
import ren.yale.ohos.retrofitcachelib.anno.Cache;
import retrofit2.http.*;

import java.util.List;
import java.util.concurrent.TimeUnit;

public interface ApiRx2 {

    @Cache(time = 10, timeUnit = TimeUnit.SECONDS)
    @GET("user/phone/{phone}")
    Observable<RespondBase<UserResult>> getUserInfo(@Path("phone") String phone);

    @PUT("device/{deviceId}")
    Observable<RespondBase> updateUserName(@Path("deviceId") String deviceId, @Body RequestBody updateDeviceDto, @Header("userId") String userId);

    @POST("device")
    Observable<RespondBase> addDevice(@Body RequestBody deviceDto);

    @DELETE("device/{deviceId}")
    Observable<RespondBase> deleteDevice(@Path("deviceId") String deviceId, @Header("userId") String userId);

    @POST("device/share")
    Observable<RespondBase> shareDevice(@Header("userId") String userId, @Query("shareUserId") String shareUserId, @Query("deviceId") String deviceId);

    @POST("device/unshare")
    Observable<RespondBase> unShareDevice(@Header("userId") String userId, @Query("shareUserId") String shareUserId, @Query("deviceId") String deviceId);

    @Cache(time = 10, timeUnit = TimeUnit.SECONDS)
    @GET("device/userId/{userId}")
    Observable<RespondBase<List<DeviceResult>>> getDeviceListByUserId(@Path("userId") String userId);

    @Cache(time = 10, timeUnit = TimeUnit.SECONDS)
    @GET("device/{deviceId}")
    Observable<RespondBase<DeviceResult>> getDeviceInfo(@Path("deviceId") String deviceId, @Header("userId") String userId);

    @POST("device/{deviceId}/sendCommand")
    Observable<RespondBase> sendCommand(@Path("deviceId") String deviceId, @Body RequestBody deviceCommandDto);

    @POST("schedule")
    Observable<RespondBase> addSchedule(@Body RequestBody createScheduleDto);

    @Cache(time = 10, timeUnit = TimeUnit.SECONDS)
    @GET("device/deviceId/{deviceId}/userShares")
    Observable<RespondBase<List<UserShareResult>>> getUserShare(@Path("deviceId") String deviceId);

    @Cache(time = 10, timeUnit = TimeUnit.SECONDS)
    @GET("family/userId/{userId}")
    Observable<RespondBase<List<FamilyResult>>> getFamiliesByUserId(@Path("userId") String userId);

    @Cache(time = 10, timeUnit = TimeUnit.SECONDS)
    @GET("devicetype/productId/{productId}")
    Observable<RespondBase<ProductResult>> getNameByProId(@Path("productId") String productId);

    @Cache(time = 10, timeUnit = TimeUnit.SECONDS)
    @GET("devicetype")
    Observable<RespondBase<List<DevicesType>>> getDevicesType();

    @Cache(time = 10, timeUnit = TimeUnit.SECONDS)
    @GET("schedule/query")
    Observable<RespondBase<List<ScheduleResult>>> getSchedule(@Header("userId") String userId, @Query("startTime") String startTime, @Query("endTime") String endTime);

    @Cache(time = 10, timeUnit = TimeUnit.SECONDS)
    @GET("schedule/search")
    Observable<RespondBase<List<ScheduleResult>>> searchSchedule(@Header("userId") String userId, @Query("name") String name);

    @DELETE("schedule/{scheduleId}")
    Observable<RespondBase> deleteScheduleById(@Path("scheduleId") String scheduleId, @Header("userId") String userId);

    @POST("family")
    Observable<RespondBase<FamilyResult>> createFamily(@Body RequestBody createFamilyBody, @Header("userId") String userId);

    @Cache(time = 10, timeUnit = TimeUnit.SECONDS)
    @GET("family/{familyId}/member")
    Observable<RespondBase<List<FamilyMemberResult>>> getFamilyMembersByFamilyId(@Path("familyId") String familyId);

    @DELETE("family/{familyId}")
    Observable<RespondBase> deleteFamily(@Path("familyId") String familyId, @Header("userId") String userId);

    @DELETE("family/{familyId}/member")
    Observable<RespondBase> deleteFamilyMember(@Path("familyId") String familyId, @Query("familyMemberId") String familyMemberId, @Header("userId") String userId);

    @PUT("user/{userId}")
    Observable<RespondBase> UpDateUserInfo(@Path("userId") String userId, @Query("name") String name);

    @POST("family/{familyId}/member")
    Observable<RespondBase> createFamilyMember(@Path("familyId") String familyId, @Query("phone") String phone, @Query("role") String role, @Header("userId") String userId);

    @PUT("family/{familyId}")
    Observable<RespondBase> updateFamily(@Path("familyId") String familyId, @Header("userId") String userId, @Body RequestBody deviceCommandDto);
}
