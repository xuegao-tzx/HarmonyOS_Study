/*
 * Copyright (C) 2021 HiHope Open Source Organization .
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
 *
 * limitations under the License.
 */

#ifndef __APP_DEMO_ROBOT_CAR_H__
#define __APP_DEMO_ROBOT_CAR_H__


#define     CAR_CONTROL_DEMO_TASK_STAK_SIZE   (1024*10)
#define     CAR_CONTROL_DEMO_TASK_PRIORITY    (25)
#define     DISTANCE_BETWEEN_CAR_AND_OBSTACLE (20)
#define     KEY_INTERRUPT_PROTECT_TIME        (30)
#define     CAR_TURN_LEFT                     (0)
#define     CAR_TURN_RIGHT                    (1)

typedef enum {
    CAR_STOP_STATUS = 0,
    CAR_OBSTACLE_AVOIDANCE_STATUS, 
    CAR_TRACE_STATUS,
    CAR_WIFI_CONNECT_ING,
    CAR_WIFI_CONNECT_OK,
    CAR_IOT_CONNECT_OK,
} CarStatus;

void switch_init(void);
void interrupt_monitor(void);
#endif