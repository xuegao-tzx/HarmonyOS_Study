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
 
package com.example.distschedule.config;

import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.Profile;
import org.springframework.context.annotation.PropertySource;

@Configuration
public class ServiceConfig {
    @Configuration
    @PropertySource("classpath:service.properties")
    static class Defaults {
    }

    @Configuration
    @Profile("dev")
    @PropertySource({"classpath:service.properties", "classpath:service-dev.properties"})
    static class Dev {
    }

    @Configuration
    @Profile("alpha")
    @PropertySource({"classpath:service.properties", "classpath:service-alpha.properties"})
    static class Alpha {
    }

    @Configuration
    @Profile("beta")
    @PropertySource({"classpath:service.properties", "classpath:service-beta.properties"})
    static class Beta {
    }

    @Configuration
    @Profile("gamma")
    @PropertySource({"classpath:service.properties", "classpath:service-gamma.properties"})
    static class Gamma {
    }

    @Configuration
    @Profile("prod")
    @PropertySource({"classpath:service.properties", "classpath:service-prod.properties"})
    static class Prod {
    }
}
