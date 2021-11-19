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
 
-- 用户表
CREATE TABLE `user`
(
    `id`          CHAR(36)    NOT NULL,
    `phone`       VARCHAR(16) NOT NULL,
    `name`        VARCHAR(32),
    `create_time` DATETIME DEFAULT NOW(),
    `update_time` DATETIME DEFAULT NOW(),
    UNIQUE KEY (phone),
    PRIMARY KEY (`id`)
)
    COMMENT ='用户信息表'
    ENGINE = InnoDB
    DEFAULT CHARSET = utf8;

-- 家庭表
CREATE TABLE `family`
(
    `id`          CHAR(36) NOT NULL,
    `name`        VARCHAR(32),
    `creator_id`  CHAR(36) NOT NULL,
    `create_time` DATETIME DEFAULT NOW(),
    `update_time` DATETIME DEFAULT NOW(),
    INDEX (creator_id),
    PRIMARY KEY (`id`)
)
    COMMENT ='家庭信息表'
    ENGINE = InnoDB
    DEFAULT CHARSET = utf8;

-- 用户家庭表
CREATE TABLE `user_family`
(
    `id`        int         NOT NULL AUTO_INCREMENT,
    `user_id`   CHAR(36)    NOT NULL,
    `family_id` CHAR(36)    NOT NULL,
    `role`      VARCHAR(32) NOT NULL,
    INDEX (user_id),
    INDEX (family_id),
    PRIMARY KEY (`id`)
)
    COMMENT ='用户家庭表'
    ENGINE = InnoDB
    DEFAULT CHARSET = utf8;

-- 设备表
CREATE TABLE `device`
(
    `id`          VARCHAR(70) NOT NULL,
    `type_id`     int         NOT NULL,
    `owner_id`    CHAR(36)    NOT NULL,
    `name`        VARCHAR(32) NOT NULL,
    `state`       VARCHAR(32) NOT NULL,
    `property`    VARCHAR(2048),
    `create_time` DATETIME DEFAULT NOW(),
    `update_time` DATETIME DEFAULT NOW(),
    INDEX (type_id),
    INDEX (owner_id),
    PRIMARY KEY (`id`)
)
    COMMENT ='设备表'
    ENGINE = InnoDB
    DEFAULT CHARSET = utf8;

-- 用户设备表
CREATE TABLE `user_device`
(
    `id`        int      NOT NULL AUTO_INCREMENT,
    `user_id`   CHAR(36) NOT NULL,
    `device_id` VARCHAR(70) NOT NULL,
    INDEX (user_id),
    INDEX (device_id),
    PRIMARY KEY (`id`)
)
    COMMENT ='用户设备表'
    ENGINE = InnoDB
    DEFAULT CHARSET = utf8;

-- 设备类型表
CREATE TABLE `device_type`
(
    `id`         int         NOT NULL AUTO_INCREMENT,
    `name`       VARCHAR(32) NOT NULL,
    `name_en`    VARCHAR(32) NOT NULL,
    `fields`     VARCHAR(32),
    `service_id` VARCHAR(32) NOT NULL,
    `product_id` VARCHAR(32) NOT NULL,
    `category`   int         NOT NULL,
    PRIMARY KEY (`id`)
)
    COMMENT ='设备类型表'
    ENGINE = InnoDB
    DEFAULT CHARSET = utf8;

-- 场景表
CREATE TABLE `scene`
(
    `id`          CHAR(36) NOT NULL,
    `name`        VARCHAR(32),
    `create_time` DATETIME DEFAULT NOW(),
    `update_time` DATETIME DEFAULT NOW(),
    PRIMARY KEY (`id`)
)
    COMMENT ='场景表'
    ENGINE = InnoDB
    DEFAULT CHARSET = utf8;

-- 日程表
CREATE TABLE `schedule`
(
    `id`            CHAR(36) NOT NULL,
    `name`          VARCHAR(32),
    `start_time`    DATETIME NOT NULL,
    `end_time`      DATETIME NOT NULL,
    `device_config` TEXT,
    `remind_day`    VARCHAR(16),
    `state_detail`  TEXT,
    `scene_id`      CHAR(36) NOT NULL,
    `creator_id`    CHAR(36) NOT NULL,
    `create_time`   DATETIME DEFAULT NOW(),
    `update_time`   DATETIME DEFAULT NOW(),
    INDEX (creator_id),
    PRIMARY KEY (`id`)
)
    COMMENT ='日程表'
    ENGINE = InnoDB
    DEFAULT CHARSET = utf8;

-- 用户日程表
CREATE TABLE `user_schedule`
(
    `id`          int      NOT NULL AUTO_INCREMENT,
    `user_id`     CHAR(36) NOT NULL,
    `schedule_id` CHAR(36) NOT NULL,
    INDEX (user_id),
    INDEX (schedule_id),
    PRIMARY KEY (`id`)
)
    COMMENT ='用户日程表'
    ENGINE = InnoDB
    DEFAULT CHARSET = utf8;

INSERT INTO device_type (`name`, `name_en`, `service_id`, `product_id`, `category`)
values ('台灯', 'lamp', 'SmartLamp', '6128c7b60ad1ed0286680f19', 1);
INSERT INTO device_type (`name`, `name_en`, `service_id`, `product_id`, `category`)
values ('扫地机器人', 'sweepingRobot', 'CleanData', '61371e952cce4f0286262229', 1);
INSERT INTO device_type (`name`, `name_en`, `service_id`, `product_id`, `category`)
values ('自动浇花', 'smartWater', 'SmartWatering', '6135e8fc2cce4f028625ccc9', 1);
INSERT INTO device_type (`name`, `name_en`, `service_id`, `product_id`, `category`)
values ('自动窗帘', 'smartCurtain', 'SmartCurtain', '6136ceba0ad1ed02866fa3b2', 1);
INSERT INTO device_type (`name`, `name_en`, `service_id`, `product_id`, `category`)
values ('手机', 'smartPhone', 'smartPhone', '00000', 0);
INSERT INTO device_type (`name`, `name_en`, `service_id`, `product_id`, `category`)
values ('手表', 'smartWatch', 'smartWatch', '00000', 0);
INSERT INTO device_type (`name`, `name_en`, `service_id`, `product_id`, `category`)
values ('平板', 'smartTablet', 'smartTablet', '00000', 0);
INSERT INTO device_type (`name`, `name_en`, `service_id`, `product_id`, `category`)
values ('烟感报警器', 'smokeDetector', 'SmartSmokeSensation', '6128bdd12cce4f02861e6d98', 3);
INSERT INTO device_type (`name`, `name_en`, `service_id`, `product_id`, `category`)
values ('可燃气体报警器', 'flammableGas', 'GasDetection', '61445c41f74139027d2233e5', 3);
INSERT INTO device_type (`name`, `name_en`, `service_id`, `product_id`, `category`)
values ('甲醛报警器', 'formaldehydeAlarm', 'TVOCDetection', '61445cdadcbb87027db095d1', 3);
INSERT INTO device_type (`name`, `name_en`, `service_id`, `product_id`, `category`)
values ('智能垃圾箱', 'smartTrashCan', 'Trashcan', '614456cff74139027d2230ec', 3);
INSERT INTO device_type (`name`, `name_en`, `service_id`, `product_id`, `category`)
values ('儿童手表', 'childrenWatch', 'SmartWatch', '613ad28b0109930287315230', 0);
INSERT INTO device_type (`name`, `name_en`, `service_id`, `product_id`, `category`)
values ('智能风扇', 'fan', 'SmartFan', '6150601d88056b027dd2ca47', 1);
