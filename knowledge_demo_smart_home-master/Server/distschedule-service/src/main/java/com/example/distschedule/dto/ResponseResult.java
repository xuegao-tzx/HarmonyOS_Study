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
 
package com.example.distschedule.dto;

import com.example.distschedule.error.ErrorCode;
import com.example.distschedule.exception.DistscheduleBaseException;
import com.example.distschedule.exception.DistscheduleFamilyException;

public class ResponseResult<T> {
    private final boolean success;
    private final String message;
    private final String errCode;
    private final T result;

    private ResponseResult(boolean success, String message, String errCode, T result) {
        this.success = success;
        this.message = message;
        this.errCode = errCode;
        this.result = result;
    }

    public static <E> ResponseResult<E> success(E results) {
        return new ResponseResult<>(true, "", "", results);
    }

    public static <E> ResponseResult<E> fail(String message) {
        return new ResponseResult<>(false, message, ErrorCode.UNKNOWN.toString(), null);
    }

    public static <E> ResponseResult<E> fail(String message, ErrorCode code) {
        return new ResponseResult<>(false, message, code.toString(), null);
    }

    public static <E> ResponseResult<E> fail(ErrorCode code) {
        return new ResponseResult<>(false, code.getDescription(), code.toString(), null);
    }

    public static ResponseResult<String> fail(DistscheduleBaseException e) {
        return new ResponseResult<>(false, e.getErrorCode().getDescription(), e.getErrorCode().toString(), null);
    }

    public boolean isSuccess() {
        return success;
    }

    public String getMessage() {
        return message;
    }

    public T getResult() {
        return result;
    }

    public String getErrCode() {
        return errCode;
    }
}
