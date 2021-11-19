/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef __UI_CONFIG_H__
#define __UI_CONFIG_H__

#include <stdio.h>

#ifdef  __cplusplus
extern "C" {
#endif

#define SCREEN_WIDTH    960
#define SCREEN_HEIGHT   480

#define START_X_POS 0
#define START_Y_POS 0

#define BACK_BUTTON_X       888
#define BACK_BUTTON_Y       32
#define BACK_BUTTON_W       48
#define BACK_BUTTON_H       48

#define BACK_BUTTON_B_X     856
#define BACK_BUTTON_B_Y     0
#define BACK_BUTTON_B_W     80
#define BACK_BUTTON_B_H     80

#define TITLE_XPOS          24
#define TITLE_YPOS          32
#define TITLE_WIDTH         372
#define TITLE_HEIGHT        44

#define TITLE_FONT          30

// the all date view info
#define DATEVIEW_START_X    72
#define DATEVIEW_START_Y    52
#define DATEVIEW_START_W    396
#define DATEVIEW_START_H    376

#define DATEVIEW_Y_POS      12
#define DATEVIEW_SMALL_H    42
#define DATEVIEW_BIG_H      72

// date
#define DATEVIEW_DATE_X         START_X_POS
#define DATEVIEW_DATE_Y         (START_Y_POS + DATEVIEW_Y_POS)
#define DATEVIEW_DATE_W         DATEVIEW_START_W
#define DATEVIEW_DATE_H         DATEVIEW_SMALL_H

// time
#define DATEVIEW_TIME_X         START_X_POS
#define DATEVIEW_TIME_Y         (DATEVIEW_Y_POS + DATEVIEW_DATE_Y + DATEVIEW_DATE_H)
#define DATEVIEW_TIME_W         DATEVIEW_START_W
#define DATEVIEW_TIME_H         DATEVIEW_BIG_H

// the room temperature
#define DATEVIEW_ROOM_TMP_X     START_X_POS
#define DATEVIEW_ROOM_TMP_Y     (DATEVIEW_Y_POS + DATEVIEW_TIME_Y + DATEVIEW_TIME_H)
#define DATEVIEW_ROOM_TMP_W     DATEVIEW_START_W
#define DATEVIEW_ROOM_TMP_H     DATEVIEW_SMALL_H

// the out side temperature
#define DATEVIEW_OUTSIDE_TMP_X  START_X_POS
#define DATEVIEW_OUTSIDE_TMP_Y  (DATEVIEW_Y_POS + DATEVIEW_ROOM_TMP_Y + DATEVIEW_ROOM_TMP_H)
#define DATEVIEW_OUTSIDE_TMP_W  DATEVIEW_START_W
#define DATEVIEW_OUTSIDE_TMP_H  DATEVIEW_SMALL_H

// weekday
#define DATEVIEW_WEEK_X         START_X_POS
#define DATEVIEW_WEEK_Y         (DATEVIEW_START_H - DATEVIEW_SMALL_H - DATEVIEW_Y_POS) 
#define DATEVIEW_WEEK_W         (DATEVIEW_START_W / 2)
#define DATEVIEW_WEEK_H         DATEVIEW_SMALL_H

// weather
#define DATEVIEW_WEATHER_X      DATEVIEW_WEEK_W
#define DATEVIEW_WEATHER_Y      DATEVIEW_WEEK_Y
#define DATEVIEW_WEATHER_W      (DATEVIEW_START_W / 2)
#define DATEVIEW_WEATHER_H      DATEVIEW_SMALL_H

// schedule view
#define SCHEDULE_START_X        368
#define SCHEDULE_START_Y        108
#define SCHEDULE_START_W        568
#define SCHEDULE_START_H        356

#define ITEM_X_POS              10
#define ITEM_Y_POS              8
#define ITEM_WIDTH              (SCHEDULE_START_W - 20)
#define ITEM_HEIGHT             96

#define SCHEDULE_TITLE_X        (START_X_POS + ITEM_X_POS)
#define SCHEDULE_TITLE_Y        START_Y_POS
#define SCHEDULE_TITLE_W        SCHEDULE_START_W
#define SCHEDULE_TITLE_H        42

#define SCHEDULE_LIST_X         START_X_POS
#define SCHEDULE_LIST_Y         (SCHEDULE_TITLE_Y + SCHEDULE_TITLE_H)
#define SCHEDULE_LIST_W         SCHEDULE_START_W
#define SCHEDULE_LIST_H         (SCHEDULE_START_H - SCHEDULE_TITLE_H)

#define LIST_ITEM_X             (START_X_POS + ITEM_X_POS)
#define LIST_ITEM_Y             START_Y_POS
#define LIST_ITEM_W             ITEM_WIDTH
#define LIST_ITEM_H             ITEM_HEIGHT

typedef struct {
    int x, y, w, h;
}BoardRect;

typedef enum {
    DATE_VIEW_ROOT = 0,
    DATE_VIEW_DATE,
    DATE_VIEW_TIME,
    DATE_VIEW_TMP_I,
    DATE_VIEW_TMP_O,
    DATE_VIEW_WEEK,
    DATE_VIEW_WEATHER,

    DATE_VIEW_NBR
} DATE_VIEW;
 
#define FONT_PATH   "SourceHanSansSC-Regular.otf"

#define FONT_BIG_ID     48
#define FONT_MID_ID     30
#define FONT_SML_ID     20

#define OPATICY_ALL     0
#define OPATICY_HALF    50
#define OPATICY_NONE    100

#define CONFIG_LOCAL_TIMEZONE   8

#define WEATHER_TIME_VIEW_BKGND     "/storage/app/run/com.huawei.electrboard/electrBoard/res/drawable/Bg_time.png"
#define SCH_LIST_BACKGROUND    "/storage/app/run/com.huawei.electrboard/electrBoard/res/drawable/list_background.png"

#define DATE_BACKGROUND_IMAGE   "/storage/app/run/com.huawei.electrboard/electrBoard/res/drawable/item_icon.png"
#define BACKGROUND_IMAGE        "/storage/app/run/com.huawei.electrboard/electrBoard/res/drawable/background.png"
#define BACK_BUTTON_ICON        "/storage/app/run/com.huawei.electrboard/electrBoard/res/drawable/icon_back_home.png"

#define LOG_D(fmt, args...) printf("[%s|%d][DEBUG]" fmt, __func__, __LINE__, ##args)
#define LOG_E(fmt, args...) printf("[%s|%d][ERROR]" fmt, __func__, __LINE__, ##args)

#ifdef  __cplusplus
}
#endif
#endif  /* __UI_CONFIG_H__ */
