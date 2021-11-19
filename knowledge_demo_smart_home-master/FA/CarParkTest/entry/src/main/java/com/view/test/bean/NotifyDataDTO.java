/*
 * Copyright (c) 2021 KaiHong Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.view.test.bean;

import java.util.List;

public class NotifyDataDTO {
    private HeaderDTO header;
    private BodyDTO body;

    public HeaderDTO getHeader() {
        return header;
    }

    public void setHeader(HeaderDTO header) {
        this.header = header;
    }

    public BodyDTO getBody() {
        return body;
    }

    public void setBody(BodyDTO body) {
        this.body = body;
    }

    public static class HeaderDTO {
        private String app_id;
        private String device_id;
        private String node_id;
        private String product_id;
        private String gateway_id;

        public String getApp_id() {
            return app_id;
        }

        public void setApp_id(String app_id) {
            this.app_id = app_id;
        }

        public String getDevice_id() {
            return device_id;
        }

        public void setDevice_id(String device_id) {
            this.device_id = device_id;
        }

        public String getNode_id() {
            return node_id;
        }

        public void setNode_id(String node_id) {
            this.node_id = node_id;
        }

        public String getProduct_id() {
            return product_id;
        }

        public void setProduct_id(String product_id) {
            this.product_id = product_id;
        }

        public String getGateway_id() {
            return gateway_id;
        }

        public void setGateway_id(String gateway_id) {
            this.gateway_id = gateway_id;
        }
    }
}
