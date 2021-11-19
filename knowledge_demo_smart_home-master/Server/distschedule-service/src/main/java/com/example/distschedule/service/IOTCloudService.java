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

import com.huaweicloud.sdk.core.auth.BasicCredentials;
import com.huaweicloud.sdk.core.auth.ICredential;
import com.huaweicloud.sdk.iotda.v5.IoTDAClient;
import com.huaweicloud.sdk.iotda.v5.region.IoTDARegion;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

@Service
public class IOTCloudService {
    private static final Logger LOGGER = LoggerFactory.getLogger(IOTCloudService.class);
    @Value("${cloud.iot.ak}")
    private String cloudIotAk;
    @Value("${cloud.iot.sk}")
    private String cloudIotSk;
    @Value("${cloud.iot.appId}")
    private String cloudIotAppId;

    /**
     * 使用华为IOT云SDK，获取IOT客户端
     *
     * @return client
     */
    public IoTDAClient getIoTDAClient() {
        // 创建认证
        ICredential auth = new BasicCredentials()
                .withAk(cloudIotAk)
                .withSk(cloudIotSk);
        // 创建IoTDAClient实例并初始化
        IoTDAClient client = IoTDAClient.newBuilder()
                .withCredential(auth)
                .withRegion(IoTDARegion.CN_NORTH_4)
                .build();
        return client;
    }
}
