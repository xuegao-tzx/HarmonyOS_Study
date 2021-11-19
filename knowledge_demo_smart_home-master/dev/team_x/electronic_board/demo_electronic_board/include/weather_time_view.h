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

#ifndef __WEATHER_TIME_VIEW_H__
#define __WEATHER_TIME_VIEW_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <common/task.h>
#include <components/ui_image_view.h>
#include <components/ui_label.h>
#include <time.h>
#include <securec.h>
#include "native_base.h"
#include "common.h"

namespace OHOS {

class WeatherTaskView : public Task {
public:
    static constexpr int LABEL_T_HOUR = 0;
    static constexpr int LABEL_T_MINUTE = 1;
    static constexpr int LABEL_T_DOT = 2;
    static constexpr int LABEL_T_WEATHER = 3;
    static constexpr int LABEL_T_TEMPI = 4;
    static constexpr int LABEL_T_AREA = 5;

    static constexpr int LABEL_T_PM25 = 6;
    static constexpr int LABEL_T_TEMPO = 7;
    static constexpr int LABEL_T_AIR = 8;
    static constexpr int LABEL_T_NOTICE = 9;

    static constexpr int LABEL_T_LAST = 10;

    WeatherTaskView(UIViewGroup *view) : viewGroup(view)
    {
        (void)memset_s(&weatherInfo_, sizeof(weatherInfo_), 0x00, sizeof(weatherInfo_));
        (void)memset_s(&weekday_, sizeof(weekday_), 0x00, sizeof(weekday_));
        (void)memset_s(&date_, sizeof(date_), 0x00, sizeof(date_));
        Task::Init();
    }

    virtual ~WeatherTaskView(){}

    void TaskStart()
    {
        Task::SetPeriod(1000);    /* 1000=1s */
        Task::TaskExecute();
    }

    void SetImage(UIImageView *img)
    {
        weatherIcon = img;
    }

    void SetLabel(UILabel *label, int idx)
    {
        switch (idx) {
            case LABEL_T_HOUR:
                labelHour = label;
                break;
            case LABEL_T_MINUTE:
                labelMinute = label;
                break;
            case LABEL_T_DOT:
                labelDot = label;
                break;
            case LABEL_T_WEATHER:
                labelWeather = label;
                break;
            case LABEL_T_TEMPI:
                labelTemp = label;
                break;
            case LABEL_T_AREA:
                labelArea = label;
                break;
            case LABEL_T_PM25:
                labelPm25 = label;
                break;
            case LABEL_T_TEMPO:
                labelOutTemp = label;
                break;
            case LABEL_T_AIR:
                labelAir = label;
                break;
            case LABEL_T_NOTICE:
                labelNotice = label;
                break;
            default:
                break;
        }
    }

    void Callback() override
    {
        struct tm rtcTime;
        int netConnect = SystemGetNetStatus();

        if (SystemGetTime(&rtcTime) < 0) {
            return;
        }
        UpdateTimeView(rtcTime.tm_hour, rtcTime.tm_min);

        minute_ = rtcTime.tm_min;
        if (hour_ != rtcTime.tm_hour || netStatus_ != netConnect) {
            netStatus_ = netConnect;
            if (netStatus_ == 0 || SystemGetWeather(&weatherInfo_) < 0) {
                SetNetStatusTitle();
                return;
            }

            if (UpdateWeatherView() == 0) {
                hour_ = rtcTime.tm_hour;
            }
            if (strlen(date_) == 0) {
                if (strcpy_s(date_, sizeof(date_), weatherInfo_.date) < 0) {
                    printf("[ERROR][%s|%d] \n", __func__,__LINE__);
                }
            }
            if (strlen(weekday_) == 0) {
                if (strcpy_s(weekday_, sizeof(weekday_), weatherInfo_.week) < 0) {
                    printf("[ERROR][%s|%d] weekday : %\n", __func__,__LINE__);
                }
            }
        }
        if (viewGroup != nullptr) {
            viewGroup->Invalidate();
        }
    }

    void SetTitleLabel(UILabel *label)
    {
        labelTitle = label;
    }

    int GetTime(int *hour, int *minute)
    {
        if (hour == NULL || minute == NULL) {
            return -1;
        }

        *hour = hour_;
        *minute = minute_;

        return 0;
    }

    int GetDate(char *buff, int size)
    {
        if (strlen(date_) <= 0) {
            return -1;
        }
        if (buff == NULL || size < strlen(date_)) {
            return -1;
        }
        (void)strcpy_s(buff, size, date_);

        return 0;
    }

    int GetWeekDay(char *buff, int size)
    {
        if (strlen(weekday_) <= 0) {
            return -1;
        }
        if (buff == NULL || size < strlen(weekday_)) {
            return -1;
        }
        (void)strcpy_s(buff, size, weekday_);

        return 0;
    }

private:
    static constexpr int WEA_ITEM_COUNT = 4;
    WeatherInfo weatherInfo_;
    char weekday_[32];
    char date_[32];
    int hour_ = -1;
    int minute_ = -1;
    int dotFlag = 0;
    int netStatus_ = 0;
    UIViewGroup *viewGroup {nullptr};
    UILabel *labelHour {nullptr};
    UILabel *labelDot {nullptr};
    UILabel *labelMinute {nullptr};

    UILabel *labelArea {nullptr};
    UILabel *labelWeather {nullptr};
    UIImageView *weatherIcon {nullptr};
    UILabel *labelTemp {nullptr};

    UILabel *labelPm25 {nullptr};
    UILabel *labelOutTemp {nullptr};
    UILabel *labelAir {nullptr};

    UILabel *labelNotice {nullptr};
    UILabel *labelTitle {nullptr};

    static constexpr char *ICON_PATH = "/storage/app/run/com.huawei.electrboard/electrBoard/res/drawable/";
    const char *WEATHER_INFO[4] = {
        "晴",
        "云",
        "雷",
        "雨"
    };
    const char *WEATHER_ICON[4] = {
        "icon_sunny.png",
        "icon_cloudy.png",
        "icon_storm.png",
        "icon_rainy.png",
    };
    const char *WEATHER_NOTICE[4] = {
        "今天太阳有点大，请注意防晒",
        "今天天气很好，祝你心情愉快",
        "今天可能下雨，出门请注意带伞",
        "今天可能下雨，出门请注意带伞"
    };

    void UpdateTimeView(int h, int m)
    {
        char hour[4] = {0};
        char minute[4] = {0};
        (void)sprintf_s(hour, sizeof(hour), "%02d", h);
        (void)sprintf_s(minute, sizeof(minute), "%02d", m);

        if (labelHour != nullptr) {
            labelHour->SetText(hour);
        }

        if (labelDot != nullptr) {
            if (dotFlag == 0) {
                labelDot->SetText(":");
                dotFlag = 1;
            } else {
                labelDot->SetText("  ");
                dotFlag = 0;
            }
        }

        if (labelMinute != nullptr) {
            labelMinute->SetText(minute);
        }
    }

    int UpdateWeatherView()
    {
        int idx = 0;
        int ret = 0x01ff;

        if (labelArea != nullptr) {
            labelArea->SetText(weatherInfo_.city);
            ret = ret >> 1;
        }
        if (labelWeather != nullptr) {
            labelWeather->SetText(weatherInfo_.weather);
            ret = ret >> 1;
        }
        if (labelTemp != nullptr) {
            char msg[128] = {0};
            sprintf(msg, "%s°/%s%", weatherInfo_.temperature, weatherInfo_.humidity);
            labelTemp->SetText(msg);
            ret = ret >> 1;
        }
        if (labelPm25 != nullptr) {
            labelPm25->SetText(weatherInfo_.airPM25);
            ret = ret >> 1;
        }
        if (labelOutTemp != nullptr) {
            char msg[128] = {0};
            sprintf(msg, "%s°/%s%", weatherInfo_.temperature, weatherInfo_.humidity);
            labelOutTemp->SetText(msg);
            ret = ret >> 1;
        }
        if (labelAir != nullptr) {
            labelAir->SetText(weatherInfo_.airQuality);
            ret = ret >> 1;
        }

        for (int i = 0; i < WEA_ITEM_COUNT; i++) {
            if (strstr(weatherInfo_.weather, WEATHER_INFO[i])) {
                idx = i;
                break;
            }
        }

        if (weatherIcon != nullptr) {
            char iconPath[256] = {0};
            (void)sprintf_s(iconPath, sizeof(iconPath), "%s/%s", ICON_PATH, WEATHER_ICON[idx]);
            weatherIcon->SetSrc(iconPath);
            ret = ret >> 1;
        }

        if (labelNotice != nullptr) {
            labelNotice->SetText(WEATHER_NOTICE[idx]);
            ret = ret >> 1;
        }

        if (labelTitle != nullptr) {
            char msg[128] = {0};
            (void)sprintf_s(msg, sizeof(msg), "%s %s", date_, weekday_);
            labelTitle->SetText((char *)msg);
            labelTitle->Invalidate();
            ret = ret >> 1;
        }

        return ret;
    }

    void SetNetStatusTitle()
    {
        if (labelTitle != nullptr) {
            labelTitle->SetText((char *)"网络未连接");
            labelTitle->Invalidate();
        }
    }
};

class WeatherTimeView : public NativeBase {
public:
    static constexpr int WEATHER_TIME_VIEW_XPOS = 25;
    static constexpr int WEATHER_TIME_VIEW_YPOS = 107;
    static constexpr int WEATHER_TIME_VIEW_WIDTH = 328;
    static constexpr int WEATHER_TIME_VIEW_HEIGHT = 357;

    WeatherTimeView() = delete;
    explicit WeatherTimeView(UIViewGroup* viewGroup);
    virtual ~WeatherTimeView();
    void SetStyle(Style sty);
    void SetPosion(int16_t width, int16_t height, int16_t x, int16_t y);
    void SetUpView();

    int GetTime(int *hour, int *minute);
    int GetDate(char *buff, int size);
    int GetWeekDay(char *buff, int size);
    void SetTitle(UILabel *label);
protected:
    void SetUpWeatherView();
    void SetUpTimeView();
private:
    UIViewGroup* viewGroup_ { nullptr };
    UIViewGroup* viewWeather_ { nullptr };
    UIViewGroup* viewTime_ { nullptr };

    WeatherTaskView *weatherTaskView_ {nullptr};

    static constexpr char *WEATHER_ICON =
        "/storage/app/run/com.huawei.electrboard/electrBoard/res/drawable/icon_sunny.png";
    static constexpr char *AREA_ICON =
        "/storage/app/run/com.huawei.electrboard/electrBoard/res/drawable/icon_location.png";
    static constexpr char *TEMP_ICON =
        "/storage/app/run/com.huawei.electrboard/electrBoard/res/drawable/icon_temp_indoor.png";
    static constexpr char *NOTICE_ICON =
        "/storage/app/run/com.huawei.electrboard/electrBoard/res/drawable/icon_point.png";

    static constexpr int TOTAL_OPACITY = 0;
    static constexpr int HALF_OPACITY = 50;
    static constexpr int NONE_OPACITY = 100;

    /* time view group info */
    static constexpr int TIME_VIEW_X = 0;
    static constexpr int TIME_VIEW_Y = 18;
    static constexpr int TIME_VIEW_W = WEATHER_TIME_VIEW_WIDTH;
    static constexpr int TIME_VIEW_H = 161;

    /* time info */
    static constexpr int TIME_HOUR_X = 12;
    static constexpr int TIME_HOUR_Y = 18;
    static constexpr int TIME_HOUR_W = 60;
    static constexpr int TIME_HOUR_H = 68;
    static constexpr int TIME_DOT_X = (TIME_HOUR_X + TIME_HOUR_W + 2);
    static constexpr int TIME_DOT_Y = (TIME_HOUR_Y - 2);
    static constexpr int TIME_DOT_W = 24;
    static constexpr int TIME_DOT_H = TIME_HOUR_H;
    static constexpr int TIME_MINUTE_X = (TIME_DOT_X + TIME_DOT_W);
    static constexpr int TIME_MINUTE_Y = 18;
    static constexpr int TIME_MINUTE_W = TIME_HOUR_W;
    static constexpr int TIME_MINUTE_H = TIME_HOUR_H;
    /* time font */
    static constexpr int TIME_FONT_ID = 48;

    /* area info */
    static constexpr int AREA_ICON_X = (229 - WEATHER_TIME_VIEW_XPOS);
    static constexpr int AREA_ICON_Y = (149 - WEATHER_TIME_VIEW_YPOS);
    static constexpr int AREA_ICON_W = 24;
    static constexpr int AREA_ICON_H = 24;

    static constexpr int AREA_TEXT_X = (AREA_ICON_X + AREA_ICON_W + 1);
    static constexpr int AREA_TEXT_Y = (149 - WEATHER_TIME_VIEW_YPOS);
    static constexpr int AREA_TEXT_W = (WEATHER_TIME_VIEW_WIDTH - AREA_TEXT_X);
    static constexpr int AREA_TEXT_H = 24;
    /* area font */
    static constexpr int AREA_FONT_ID = 20;

    /* weather info */
    static constexpr int WEA_ICON_X = (48 - WEATHER_TIME_VIEW_XPOS);
    static constexpr int WEA_ICON_Y = (192 - WEATHER_TIME_VIEW_YPOS - 2);
    static constexpr int WEA_ICON_W = 32;
    static constexpr int WEA_ICON_H = 32;

    static constexpr int WEA_TEXT_X = (WEA_ICON_X + WEA_ICON_W);
    static constexpr int WEA_TEXT_Y = (192 - WEATHER_TIME_VIEW_YPOS);
    static constexpr int WEA_TEXT_W = 100;
    static constexpr int WEA_TEXT_H = 32;
    /* weather font */
    static constexpr int WEA_FONT_ID = 20;

    /* temperature info */
    static constexpr int TEMP_ICON_X = (229 - WEATHER_TIME_VIEW_XPOS);
    static constexpr int TEMP_ICON_Y = (192 - WEATHER_TIME_VIEW_YPOS);
    static constexpr int TEMP_ICON_W = 24;
    static constexpr int TEMP_ICON_H = 24;

    static constexpr int TEMP_TEXT_X = (TEMP_ICON_X + TEMP_ICON_W + 1);
    static constexpr int TEMP_TEXT_Y = (190 - WEATHER_TIME_VIEW_YPOS);
    static constexpr int TEMP_TEXT_W = 100;
    static constexpr int TEMP_TEXT_H = 24;
    /* temperature font */
    static constexpr int TEMP_FONT_ID = 20;

    /* weather view group info */
    static constexpr int WEATHER_VIEW_X = 0;
    static constexpr int WEATHER_VIEW_Y = 196;
    static constexpr int WEATHER_VIEW_W = WEATHER_TIME_VIEW_WIDTH;
    static constexpr int WEATHER_VIEW_H = 161;

    /* PM2.5 info */
    static constexpr int PM25_TITLE_X = 0;
    static constexpr int PM25_TITLE_Y = (318 - 303);
    static constexpr int PM25_TITLE_W = (122 - WEATHER_TIME_VIEW_XPOS);
    static constexpr int PM25_TITLE_H = 20;

    static constexpr int PM25_VALUE_X = PM25_TITLE_X;
    static constexpr int PM25_VALUE_Y = (PM25_TITLE_Y + PM25_TITLE_H + 1);
    static constexpr int PM25_VALUE_W = PM25_TITLE_W;
    static constexpr int PM25_VALUE_H = 28;

    /* first vertical line */
    static constexpr int WEATHER_LINE_X1 = (PM25_TITLE_W + 1);
    static constexpr int WEATHER_LINE_Y1 = (PM25_TITLE_Y - 4);
    static constexpr int WEATHER_LINE_W1 = 1;
    static constexpr int WEATHER_LINE_H1 = 40;

    /* outside TEMP info */
    static constexpr int TEMP_TITLE_X = (WEATHER_LINE_X1);
    static constexpr int TEMP_TITLE_Y = PM25_TITLE_Y;
    static constexpr int TEMP_TITLE_W = PM25_TITLE_W;
    static constexpr int TEMP_TITLE_H = PM25_TITLE_H;

    static constexpr int TEMP_VALUE_X = TEMP_TITLE_X;
    static constexpr int TEMP_VALUE_Y = (TEMP_TITLE_Y + TEMP_TITLE_H);
    static constexpr int TEMP_VALUE_W = TEMP_TITLE_W;
    static constexpr int TEMP_VALUE_H = PM25_VALUE_H;

    /* second vertical line */
    static constexpr int WEATHER_LINE_X2 = (TEMP_TITLE_X + TEMP_TITLE_W + 1);
    static constexpr int WEATHER_LINE_Y2 = WEATHER_LINE_Y1;
    static constexpr int WEATHER_LINE_W2 = WEATHER_LINE_W1;
    static constexpr int WEATHER_LINE_H2 = WEATHER_LINE_H1;

    /* air quality info */
    static constexpr int AIR_QUALITY_TITLE_X = (WEATHER_LINE_X2);
    static constexpr int AIR_QUALITY_TITLE_Y = PM25_TITLE_Y;
    static constexpr int AIR_QUALITY_TITLE_W = PM25_TITLE_W;
    static constexpr int AIR_QUALITY_TITLE_H = PM25_TITLE_H;

    static constexpr int AIR_QUALITY_VALUE_X = AIR_QUALITY_TITLE_X;
    static constexpr int AIR_QUALITY_VALUE_Y = (AIR_QUALITY_TITLE_Y + AIR_QUALITY_TITLE_H);
    static constexpr int AIR_QUALITY_VALUE_W = AIR_QUALITY_TITLE_W;
    static constexpr int AIR_QUALITY_VALUE_H = PM25_VALUE_H;
    /* font size */
    static constexpr int TITLE_FONT_ID = 16;
    static constexpr int VALUE_FONT_ID = 20;

    /* notice info */
    static constexpr int NOTICE_ICON_X = (55 - WEATHER_TIME_VIEW_XPOS) - 12;
    static constexpr int NOTICE_ICON_Y = (420 - 303);
    static constexpr int NOTICE_ICON_W = 8;
    static constexpr int NOTICE_ICON_H = 8;
    static constexpr int NOTICE_TEXT_X = (NOTICE_ICON_X + NOTICE_ICON_W+4);
    static constexpr int NOTICE_TEXT_Y = (416 - 303);
    static constexpr int NOTICE_TEXT_W = (WEATHER_TIME_VIEW_WIDTH - NOTICE_TEXT_X);
    static constexpr int NOTICE_TEXT_H = 24;
    static constexpr int NOTICE_FONT_ID = 16;
};
}

#endif  /* __WEATHER_TIME_VIEW_H__ */
