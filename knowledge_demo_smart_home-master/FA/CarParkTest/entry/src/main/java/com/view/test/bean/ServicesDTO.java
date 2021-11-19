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

public class ServicesDTO {
    private String service_id;
    private ServicesDTO.PropertiesDTO properties;
    private String event_time;

    public String getService_id() {
        return service_id;
    }

    public void setService_id(String service_id) {
        this.service_id = service_id;
    }

    public ServicesDTO.PropertiesDTO getProperties() {
        return properties;
    }

    public void setProperties(ServicesDTO.PropertiesDTO properties) {
        this.properties = properties;
    }

    public String getEvent_time() {
        return event_time;
    }

    public void setEvent_time(String event_time) {
        this.event_time = event_time;
    }

    public static class PropertiesDTO {
        private int allParks;
        private int Location;
        private int isPark;

        public int getAllParks() {
            return allParks;
        }

        public void setAllParks(int allParks) {
            this.allParks = allParks;
        }

        public int getLocation() {
            return Location;
        }

        public void setLocation(int location) {
            this.Location = location;
        }

        public int getIsPark() {
            return isPark;
        }

        public void setIsPark(int isPark) {
            this.isPark = isPark;
        }
    }
}
