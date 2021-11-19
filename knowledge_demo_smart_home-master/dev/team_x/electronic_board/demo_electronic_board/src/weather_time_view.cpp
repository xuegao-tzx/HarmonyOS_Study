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

#include "weather_time_view.h"
#include "ui_config.h"

namespace OHOS {

WeatherTimeView::WeatherTimeView(UIViewGroup* viewGroup)
{
    viewGroup_ = viewGroup;
    weatherTaskView_ = new WeatherTaskView(viewGroup);
}

WeatherTimeView::~WeatherTimeView()
{
    if (viewWeather_ != nullptr) {
        DeleteChildren(viewWeather_);
        viewWeather_ = nullptr;
    }
    if (viewTime_ != nullptr) {
        DeleteChildren(viewTime_);
        viewTime_ = nullptr;
    }
    if (weatherTaskView_ != nullptr) {
        delete weatherTaskView_;
    }
}

UILabel *GetLabel(const char *text, BoardRect r, int font,
    UITextLanguageAlignment horPos, UITextLanguageAlignment verPos)
{
    UILabel *label = new UILabel();
    label->SetPosition(r.x, r.y, r.w, r.h);
    label->SetAlign(horPos, verPos);
    label->SetFont((const char *)FONT_PATH, font);
    label->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::White()));
    label->SetStyle(STYLE_BACKGROUND_OPA, OPATICY_ALL);
    label->SetText(text);
    return label;
}

void WeatherTimeView::SetUpTimeView()
{
    viewTime_ = new UIViewGroup();
    viewTime_->SetPosition(TIME_VIEW_X, TIME_VIEW_Y, TIME_VIEW_W, TIME_VIEW_H);
    viewTime_->SetStyle(STYLE_BACKGROUND_OPA, TOTAL_OPACITY);
    viewGroup_->Add(viewTime_);

    BoardRect timeViewRect[WeatherTaskView::LABEL_T_AREA + 1] = {
        {TIME_HOUR_X, TIME_HOUR_Y, TIME_HOUR_W, TIME_HOUR_H},
        {TIME_MINUTE_X, TIME_MINUTE_Y, TIME_MINUTE_W, TIME_MINUTE_H},
        {TIME_DOT_X, TIME_DOT_Y, TIME_DOT_W, TIME_DOT_H},
        {WEA_TEXT_X, WEA_TEXT_Y, WEA_TEXT_W, WEA_TEXT_H},
        {TEMP_TEXT_X, TEMP_TEXT_Y, TEMP_TEXT_W, TEMP_TEXT_H},
        {AREA_TEXT_X, AREA_TEXT_Y, AREA_TEXT_W, AREA_TEXT_H}
    };
    int fontId[WeatherTaskView::LABEL_T_AREA + 1] =
        {TIME_FONT_ID, TIME_FONT_ID, TIME_FONT_ID, WEA_FONT_ID, TEMP_FONT_ID, AREA_FONT_ID};
    char *timeViewText[WeatherTaskView::LABEL_T_AREA + 1] = {"00", "00", ":", "晴", "28°/46%", "深圳"};
    for (int i = 0; i <= WeatherTaskView::LABEL_T_AREA; i++) {
        UILabel *label = GetLabel(timeViewText[i], timeViewRect[i], fontId[i],
                                TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
        viewTime_->Add(label);
        weatherTaskView_->SetLabel(label, i);
    }

    UIImageView *weaIcon = new UIImageView();
    weaIcon->SetPosition(WEA_ICON_X, WEA_ICON_Y, WEA_ICON_W, WEA_ICON_H);
    weaIcon->SetSrc(WEATHER_ICON);
    weaIcon->SetStyle(STYLE_BACKGROUND_OPA, TOTAL_OPACITY);
    viewTime_->Add(weaIcon);
    weatherTaskView_->SetImage(weaIcon);

    UIImageView *areaIcon = new UIImageView();
    areaIcon->SetPosition(AREA_ICON_X, AREA_ICON_Y, AREA_ICON_W, AREA_ICON_H);
    areaIcon->SetSrc(AREA_ICON);
    areaIcon->SetStyle(STYLE_BACKGROUND_OPA, TOTAL_OPACITY);
    viewTime_->Add(areaIcon);

    UIImageView *tempIcon = new UIImageView();
    tempIcon->SetPosition(TEMP_ICON_X, TEMP_ICON_Y, TEMP_ICON_W, TEMP_ICON_H);
    tempIcon->SetSrc(TEMP_ICON);
    tempIcon->SetStyle(STYLE_BACKGROUND_OPA, TOTAL_OPACITY);
    viewTime_->Add(tempIcon);
}

void WeatherTimeView::SetUpWeatherView()
{
    viewWeather_ = new UIViewGroup();
    viewWeather_->SetPosition(WEATHER_VIEW_X, WEATHER_VIEW_Y, WEATHER_VIEW_W, WEATHER_VIEW_H);
    viewWeather_->SetStyle(STYLE_BACKGROUND_OPA, TOTAL_OPACITY);
    viewGroup_->Add(viewWeather_);

    BoardRect weatherViewRect[7] = {
        {PM25_VALUE_X, PM25_VALUE_Y, PM25_VALUE_W, PM25_VALUE_H},
        {PM25_TITLE_X, PM25_TITLE_Y, PM25_TITLE_W, PM25_TITLE_H},
        {TEMP_VALUE_X, TEMP_VALUE_Y, TEMP_VALUE_W, TEMP_VALUE_H},
        {TEMP_TITLE_X, TEMP_TITLE_Y, TEMP_TITLE_W, TEMP_TITLE_H},
        {AIR_QUALITY_VALUE_X, AIR_QUALITY_VALUE_Y, AIR_QUALITY_VALUE_W, AIR_QUALITY_VALUE_H},
        {AIR_QUALITY_TITLE_X, AIR_QUALITY_TITLE_Y, AIR_QUALITY_TITLE_W, AIR_QUALITY_TITLE_H},
        {NOTICE_TEXT_X, NOTICE_TEXT_Y, NOTICE_TEXT_W, NOTICE_TEXT_H}
    };
    char *weatherViewText[7] = {
        "2.5",
        "PM值",
        "32°/48%",
        "室外温湿度",
        "优",
        "空气质量",
        "今天天气很好，祝你心情愉快"
    };
    int count = sizeof(weatherViewText) / sizeof(weatherViewText[0]);
    const int parityFlag = 2;

    for (int i = 0; i < count; i++) {
        int font = (i % parityFlag == 0) ? VALUE_FONT_ID : TITLE_FONT_ID;
        UITextLanguageAlignment pos = (i == count - 1) ? TEXT_ALIGNMENT_LEFT : TEXT_ALIGNMENT_CENTER;
        UILabel *label = GetLabel(weatherViewText[i], weatherViewRect[i], font, pos, TEXT_ALIGNMENT_CENTER);
        viewWeather_->Add(label);
        if (i % parityFlag == 0) {
            int idx = WeatherTaskView::LABEL_T_PM25 + i / parityFlag;
            weatherTaskView_->SetLabel(label, idx);
        }
    }

    UIImageView *noticeIcon = new UIImageView();
    noticeIcon->SetPosition(NOTICE_ICON_X, NOTICE_ICON_Y, NOTICE_ICON_W, NOTICE_ICON_H);
    noticeIcon->SetSrc(NOTICE_ICON);
    noticeIcon->SetStyle(STYLE_BACKGROUND_OPA, TOTAL_OPACITY);
    viewWeather_->Add(noticeIcon);
}

void WeatherTimeView::SetStyle(Style sty)
{
    viewGroup_->SetStyle(STYLE_BACKGROUND_OPA, TOTAL_OPACITY);
    viewGroup_->Invalidate();
}

void WeatherTimeView::SetPosion(int16_t width, int16_t height, int16_t x, int16_t y)
{
    viewGroup_->SetPosition(x, y, width, height);
}

void WeatherTimeView::SetUpView()
{
    SetUpTimeView();
    SetUpWeatherView();
    viewGroup_->Invalidate();
    weatherTaskView_->TaskStart();
}

int WeatherTimeView::GetTime(int *hour, int *minute)
{
    return weatherTaskView_->GetTime(hour, minute);
}

int WeatherTimeView::GetDate(char *buff, int size)
{
    if (buff == nullptr || size <= 0 || weatherTaskView_ == nullptr) {
        return -1;
    }
    if (weatherTaskView_->GetDate(buff, size) < 0) {
        return -1;
    }

    return 0;
}

int WeatherTimeView::GetWeekDay(char *buff, int size)
{
    if (buff == nullptr || size <= 0 || weatherTaskView_ == nullptr) {
        return -1;
    }
    if (weatherTaskView_->GetWeekDay(buff, size) < 0) {
        return -1;
    }

    return 0;
}

void WeatherTimeView::SetTitle(UILabel *label)
{
    weatherTaskView_->SetTitleLabel(label);
}
}
