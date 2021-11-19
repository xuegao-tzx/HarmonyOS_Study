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
import com.example.distschedule.error.ErrorCode;
import com.example.distschedule.exception.DistschedulePermissionException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.context.support.DefaultMessageSourceResolvable;
import org.springframework.web.bind.MethodArgumentNotValidException;
import org.springframework.web.bind.annotation.ControllerAdvice;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.context.request.WebRequest;

import javax.validation.ConstraintViolation;
import javax.validation.ConstraintViolationException;
import java.sql.SQLException;
import java.util.Set;
import java.util.stream.Collectors;

@ControllerAdvice
public class DistscheduleGlobalExceptionHandler {
    private static final Logger LOGGER = LoggerFactory.getLogger(DistscheduleGlobalExceptionHandler.class);

    /**
     * 处理校验参数不合法的异常.
     */
    @ExceptionHandler(value = {ConstraintViolationException.class})
    @ResponseBody
    public ResponseResult<String> handleConstraintViolation(Exception ex, WebRequest request) {
        ConstraintViolationException violationException = (ConstraintViolationException) ex;
        Set<ConstraintViolation<?>> violations = violationException.getConstraintViolations();
        String errorMessage = violations.stream()
                .map(ConstraintViolation::getMessage)
                .collect(Collectors.joining(", "));
        return ResponseResult.fail(errorMessage, ErrorCode.ILLEGAL_PARAM);
    }

    @ExceptionHandler(value = {MethodArgumentNotValidException.class})
    @ResponseBody
    protected ResponseResult<String> handleMethodArgumentNotValid(Exception ex, WebRequest request) {
        MethodArgumentNotValidException methodArgumentNotValidException = (MethodArgumentNotValidException) ex;
        String errorMessage = methodArgumentNotValidException.getBindingResult()
                .getAllErrors().stream()
                .map(DefaultMessageSourceResolvable::getDefaultMessage)
                .collect(Collectors.joining(", "));
        return ResponseResult.fail(errorMessage, ErrorCode.ILLEGAL_PARAM);
    }

    @ExceptionHandler(value = {IllegalArgumentException.class, IllegalStateException.class,
            UnsupportedOperationException.class})
    @ResponseBody
    public ResponseResult<String> handleCommonException(Exception ex, WebRequest request) {
        LOGGER.error("{}", ex.getMessage());
        if (ex instanceof IllegalArgumentException) {
            return ResponseResult.fail("Illegal argument, " + ex.getMessage(), ErrorCode.ILLEGAL_PARAM);
        } else if (ex instanceof IllegalStateException) {
            return ResponseResult.fail("Illegal state, " + ex.getMessage(), ErrorCode.ILLEGAL_STATE);
        } else if (ex instanceof UnsupportedOperationException) {
            return ResponseResult.fail("Unsupported operation, " + ex.getMessage(), ErrorCode.UNSUPPORTED_OPERATION);
        }
        return ResponseResult.fail("internal error");
    }

    @ExceptionHandler(value = {SQLException.class})
    @ResponseBody
    public ResponseResult<String> handleSQLException(Exception ex, WebRequest request) {
        LOGGER.error("{}", ex.getMessage());
        return ResponseResult.fail("internal error", ErrorCode.DATABASE);
    }

    @ExceptionHandler(value = {DistschedulePermissionException.class})
    @ResponseBody
    public ResponseResult<String> handleDistschedulePermissionException(Exception ex, WebRequest request) {
        LOGGER.error("{}", ex.getMessage());
        return ResponseResult.fail(ErrorCode.ILLEGAL_PERMISSION);
    }



}
