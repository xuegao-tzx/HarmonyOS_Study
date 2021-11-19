/*
*Copyright(c) 2021 Huawei Device Co., Ltd.
*Licensed under the Apache License, Version 2.0 (the "License");
*you may not use this file except in compliance with the License.
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
*Unless required by applicable law or agreed to in writing, software
*distributed under the license is distributed on an "AS IS" BASIS,
*WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*See the License for the specific language governing permisssions and
*limitations under the License.
*/

#ifndef OHOS_UI_CONFIG_H
#define OHOS_UI_CONFIG_H

#include <cstdint>

namespace OHOS {
    static int16_t ROOTVIEW_W = 960; // launcher table width
    static int16_t ROOTVIEW_H = 480; // launcher table height
    static constexpr int16_t LABLE_TITLE_HEIGHT = 30; // tail lable height
    static constexpr int16_t LABLE_TAIL_HEIGHT = 30;
    static constexpr int16_t APP_COL_COUNT = 3; // a sswipe view app count in col
    static constexpr int16_t FOND_ID = 16; // app name fond id
    static constexpr int16_t BIG_FOND_ID = 28; // time big fond id
    static constexpr int16_t BIGBIG_FOND_ID = 32; // time big fond id
    static constexpr int16_t TOTAL_OPACITY = 0; // transparent
    static constexpr int16_t HALF_OPACITY = 50; // diaphanous
    static constexpr int16_t HALF_OPACITY_CHANGE = 100; // diaphanous
    static constexpr int16_t UN_OPACITY = 255; // opaque
    static constexpr int16_t BUTTON_RADIUS = 20; // app icon radius
    static constexpr int16_t LABEL_RADIUS = 0; // label icon radius
    static constexpr int16_t TITLE_LABEL_OPACITY = 255; //translucent
    static constexpr int16_t GROUP_VIEW_RADIUS = 20; // view radius

    #define ZERO_POSITION       0
    #define BLANK_H             5 // 间隙的高
    #define BLANK_W             10 // 间隙的宽
    #define LIGHT_RADIO         5 // 圆角半径
    #define HEAD_H              60
    #define HEAD_W              960
    #define SWIPE_VIEW_H        400
    #define SWIPE_VIEW_W        960
    #define IMAGE_H             40
    #define IMAGE_W             40
    #define LABEL_W             150
    #define LABEL_H             40
    #define BUTTON_W            100
    #define BUTTON_H            60
    #define VIEW_GROUP_H        60
    #define VIEW_GROUP_W        960
    #define LAYOUT_H            100
    #define LAYOUT_W            950
    #define LAYOUT_HH           300
    #define ITEM_LIST_HEIGHT    80

    // first view
    // first view location
    #define FIRST_VIEW_X        32
    #define FIRST_VIEW_Y        38
    #define FIRST_VIEW_W        400
    #define FIRST_VIEW_H        348

    #define FIRST_VIEW_LB_Y     50
    #define FIRST_VIEW_LB_W     360
    #define FIRST_VIEW_LB_H     100

    #define FIRST_VIEW_LB2_X    90
    #define FIRST_VIEW_LB2_Y    190
    #define FIRST_VIEW_LB2_W    260
    #define FIRST_VIEW_LB2_H    50

    #define FIRST_VIEW_LB3_Y    250

    #define FIRST_VIEW_IMAGE_X  40
    #define FIRST_VIEW_IMAGE_Y  200
    #define FIRST_VIEW_IMAGE_W  32
    #define FIRST_VIEW_IMAGE_H  32
    #define FIRST_VIEW_IMAGE1_Y 260

    #define FIRST_VIEW_LY_X     440
    #define FIRST_VIEW_LY_Y     38
    #define FIRST_VIEW_LY_W     500
    #define FIRST_VIEW_LY_H     348

    // 温度标签位置
    #define TEMP_X              10
    #define TEMP_Y              10
    #define TEMP_W              180
    #define TEMP_H              120

    // other view
    #define OTHER_VIEW_X        32
    #define OTHER_VIEW_Y        38
    #define OTHER_VIEW_W        244
    #define OTHER_VIEW_H        348

    #define OTHER_VIEW_LB_X     40
    #define OTHER_VIEW_LB_Y     190
    #define OTHER_VIEW_LB_W     140
    #define OTHER_VIEW_LB_H     60

    #define OTHER_VIEW_IM_X     40
    #define OTHER_VIEW_IM_Y     24
    #define OTHER_VIEW_IM_W     140
    #define OTHER_VIEW_IM_H     140

    #define BACK_IMAGE_X        20
    #define BACK_IMAGE_Y        5
    #define BACK_IMAGE_W        110
    #define BACK_IMAGE_H        70

    #define ITEM_IMAGE_X        70
    #define ITEM_IMAGE_Y        24
    #define ITEM_IMAGE_W        64
    #define ITEM_IMAGE_H        64

    #define LABEL_IMAGE_Y       100
    #define LABEL_IMAGE_W       180
    #define LABEL_IMAGE_H       50

    #define OTHER_VIEW_LY_X     260
    #define OTHER_VIEW_LY_Y     38
    #define OTHER_VIEW_LY_W     700
    #define OTHER_VIEW_LY_H     348

    // 定义按钮项的位置
    #define ITEM_VIEW_W         208
    #define ITEM_VIEW_H         165

    #define LAYOUT_ROWS         2
    #define LAYOUT_COLS         3

    #define TMP_BUF_SIZE        128

    //

    #define RES_DEEPLIGHT_MODE      "/storage/app/run/com.huawei.cenctrl/cenctrl/res/deeplight.png"
    #define RES_DINNER_MODE         "/storage/app/run/com.huawei.cenctrl/cenctrl/res/dinner.png"
    #define RES_GUEST_MODE          "/storage/app/run/com.huawei.cenctrl/cenctrl/res/guest.png"
    #define RES_OFF_MODE            "/storage/app/run/com.huawei.cenctrl/cenctrl/res/off.png"
    #define RES_AIR_HUM             "/storage/app/run/com.huawei.cenctrl/cenctrl/res/airHumidity.png"
    #define RES_AIR_QUA             "/storage/app/run/com.huawei.cenctrl/cenctrl/res/airQuality.png"

    #define RES_LIGHT               "/storage/app/run/com.huawei.cenctrl/cenctrl/res/icon_light.png"
    #define RES_LIVINGROOM_SELECT   "/storage/app/run/com.huawei.cenctrl/cenctrl/res/livingSelect.png"
    #define RES_LIVINGROOM_UNSELECT "/storage/app/run/com.huawei.cenctrl/cenctrl/res/livingUnSelect.png"
    #define RES_AISLE_SELECT        "/storage/app/run/com.huawei.cenctrl/cenctrl/res/aisleSelect.png"
    #define RES_AISLE_UNSELECT      "/storage/app/run/com.huawei.cenctrl/cenctrl/res/aisleUnSelect.png"
    #define RES_BELT_SELECT         "/storage/app/run/com.huawei.cenctrl/cenctrl/res/beltSelect.png"
    #define RES_BELT_UNSELECT       "/storage/app/run/com.huawei.cenctrl/cenctrl/res/beltUnSelect.png"
    #define RES_DININGROOM_SELECT   "/storage/app/run/com.huawei.cenctrl/cenctrl/res/diningSelect.png"
    #define RES_DININGROOM_UNSELECT "/storage/app/run/com.huawei.cenctrl/cenctrl/res/diningUnSelect.png"

    #define RES_AIRCONDITION        "/storage/app/run/com.huawei.cenctrl/cenctrl/res/icon_air_condition.png"
    #define RES_COLD_SELECT         "/storage/app/run/com.huawei.cenctrl/cenctrl/res/coldSelected.png"
    #define RES_COLD_UNSELECT       "/storage/app/run/com.huawei.cenctrl/cenctrl/res/coldUnselected.png"
    #define RES_SWITCH_SELECT       "/storage/app/run/com.huawei.cenctrl/cenctrl/res/switchSelected.png"
    #define RES_SWITCH_UNSELECT     "/storage/app/run/com.huawei.cenctrl/cenctrl/res/switchUnSelected.png"
    #define RES_TEMP_REDUCE_SELECT  "/storage/app/run/com.huawei.cenctrl/cenctrl/res/tempReduceSelected.png"
    #define RES_TEMP_REDUCE_UNSELECT    "/storage/app/run/com.huawei.cenctrl/cenctrl/res/tempReduceUnSelected.png"
    #define RES_TEMP_PLUS_SELECT    "/storage/app/run/com.huawei.cenctrl/cenctrl/res/tempPlusUnSelected.png"
    #define RES_TEMP_PLUS_UNSELECT  "/storage/app/run/com.huawei.cenctrl/cenctrl/res/tempPlusUnSelected.png"
    #define RES_WIND_SELECT         "/storage/app/run/com.huawei.cenctrl/cenctrl/res/windSelected.png"
    #define RES_WIND_UNSELECT       "/storage/app/run/com.huawei.cenctrl/cenctrl/res/windUnSelected.png"

    #define RES_CURTAIN             "/storage/app/run/com.huawei.cenctrl/cenctrl/res/icon_curtain.png"
    #define RES_CURTAIN_SELECT      "/storage/app/run/com.huawei.cenctrl/cenctrl/res/curtainSelected.png"
    #define RES_CURTAIN_UNSELECT    "/storage/app/run/com.huawei.cenctrl/cenctrl/res/curtainUnSelected.png"
    #define RES_CURTAIN_OFF_SELECT  "/storage/app/run/com.huawei.cenctrl/cenctrl/res/curtainOffSelected.png"
    #define RES_CURTAIN_OFF_UNSELECT "/storage/app/run/com.huawei.cenctrl/cenctrl/res/curtainOffUnselected.png"

    #define RES_BACKGROUND  "/storage/app/run/com.huawei.cenctrl/cenctrl/res/background.jpg"
    #define RES_BACK        "/storage/app/run/com.huawei.cenctrl/cenctrl/res/back.png"
    #define RES_FORWARD     "/storage/app/run/com.huawei.cenctrl/cenctrl/res/forward.jpg"

    #define FOND_PATH       "SourceHanSansSC-Regular.otf"
}// namespace OHOS
#endif
