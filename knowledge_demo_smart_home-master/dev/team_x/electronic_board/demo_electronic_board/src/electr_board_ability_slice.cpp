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

#include <algorithm>
#include <meta_data.h>
#include <window/window.h>

#include "ability_manager.h"
#include "gfx_utils/color.h"

#include "electr_board_ability_slice.h"
#include "sntp.h"
#include "schedule_net_status.h"

namespace OHOS {
REGISTER_AS(ElectrBoardAbilitySlice)

ElectrBoardAbilitySlice::~ElectrBoardAbilitySlice()
{
    ScheduleStopNetStatus();
    if (labelTitle_ != nullptr) {
        delete labelTitle_;
        labelTitle_ = nullptr;
    }
    if (buttonListener_ != nullptr) {
        delete buttonListener_;
        buttonListener_ = nullptr;
    }
    if (backButton_ != nullptr) {
        delete backButton_;
        backButton_ = nullptr;
    }
    if (backButtonB_ != nullptr) {
        delete backButtonB_;
        backButtonB_ = nullptr;
    }
    if (weatherTimeView_ != nullptr) {
        delete weatherTimeView_;
        weatherTimeView_ = nullptr;
    }
    if (dateInfo_ != nullptr) {
        DeleteChildren(dateInfo_);
        dateInfo_ = nullptr;
    }

    if (scheduleInfo_ != nullptr) {
        DeleteChildren(scheduleInfo_);
        scheduleInfo_ = nullptr;
    }

    if (scheduleTask_ != nullptr) {
        delete scheduleTask_;
        scheduleTask_ = nullptr;
    }

    if (scheduleList_.Size() > 0) {
        scheduleList_.Clear();
    }
}

UILabel *GetLabel(const char *text, BoardRect r, int font)
{
    UILabel *label = new UILabel();
    label->SetPosition(r.x, r.y, r.w, r.h);
    label->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_LEFT, UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
    label->SetFont((const char *)FONT_PATH, font);
    label->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::White()));
    label->SetStyle(STYLE_BACKGROUND_OPA, OPATICY_ALL);
    label->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);
    label->SetText(text);
    return label;
}

void ElectrBoardAbilitySlice::SetScrollView(void)
{
    scheduleView_->ScheduleListViewNotify();
}

void ElectrBoardAbilitySlice::SetScheduleView(void)
{
    scheduleInfo_ = new UIViewGroup();
    scheduleInfo_->SetPosition(SCHEDULE_START_X, SCHEDULE_START_Y, SCHEDULE_START_W, SCHEDULE_START_H);
    scheduleInfo_->SetStyle(STYLE_BACKGROUND_OPA, OPATICY_ALL);

    UIImageView *bk = new UIImageView();
    bk->SetPosition(START_X_POS, START_Y_POS, SCHEDULE_START_W, SCHEDULE_START_H);
    bk->SetStyle(STYLE_BACKGROUND_OPA, OPATICY_ALL);
    bk->SetSrc(SCH_LIST_BACKGROUND);
    scheduleInfo_->Add(bk);

    scheduleView_ = new ScheduleListView(scheduleInfo_, weatherTimeView_);
    scheduleView_->ScheduleListViewSetup();
    scheduleView_->ScheduleListViewSetList(&scheduleList_);

    rootview_->Add(scheduleInfo_);

    scheduleTask_ = new ScheduleListTask(weatherTimeView_, scheduleView_, &scheduleList_);
    scheduleTask_->TaskStart();
}

void ElectrBoardAbilitySlice::SetDateItemView(void)
{
    dateInfo_ = new UIViewGroup();
    dateInfo_->SetPosition(WeatherTimeView::WEATHER_TIME_VIEW_XPOS, WeatherTimeView::WEATHER_TIME_VIEW_YPOS,\
        WeatherTimeView::WEATHER_TIME_VIEW_WIDTH, WeatherTimeView::WEATHER_TIME_VIEW_HEIGHT);
    dateInfo_->SetStyle(STYLE_BACKGROUND_OPA, OPATICY_ALL);
    UIImageView *imageV = new UIImageView();
    imageV->SetPosition(0, 0, WeatherTimeView::WEATHER_TIME_VIEW_WIDTH, WeatherTimeView::WEATHER_TIME_VIEW_HEIGHT);
    imageV->SetStyle(STYLE_BACKGROUND_OPA, OPATICY_ALL);
    imageV->SetSrc(WEATHER_TIME_VIEW_BKGND);
    dateInfo_->Add(imageV);

    weatherTimeView_ = new WeatherTimeView(dateInfo_);
    weatherTimeView_->SetUpView();

    rootview_->Add(dateInfo_);
}

void ElectrBoardAbilitySlice::SetBackButton()
{
    backButtonB_ = new UIImageView();
    backButtonB_->SetPosition(BACK_BUTTON_B_X, BACK_BUTTON_B_Y, BACK_BUTTON_B_W, BACK_BUTTON_B_H);
    backButtonB_->SetStyle(STYLE_BACKGROUND_OPA, OPATICY_ALL);
    backButtonB_->SetTouchable(true);

    backButton_ = new UIImageView();
    backButton_->SetPosition(BACK_BUTTON_X, BACK_BUTTON_Y, BACK_BUTTON_W, BACK_BUTTON_H);
    backButton_->SetStyle(STYLE_BACKGROUND_OPA, OPATICY_ALL);
    backButton_->SetSrc(BACK_BUTTON_ICON);
    backButton_->SetTouchable(true);
    auto backBttnonClick = [this](UIView &view, const Event &event) -> bool {
        TerminateAbility();
        return true;
    };

    buttonListener_ = new EventListener(backBttnonClick, nullptr);
    backButton_->SetOnClickListener(buttonListener_);
    backButtonB_->SetOnClickListener(buttonListener_);

    rootview_->Add(backButtonB_);
    rootview_->Add(backButton_);
}

void ElectrBoardAbilitySlice::SetTitle()
{
    labelTitle_ = new UILabel();
    labelTitle_->SetPosition(TITLE_XPOS, TITLE_YPOS, TITLE_WIDTH, TITLE_HEIGHT);
    labelTitle_->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_LEFT, UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
    labelTitle_->SetFont((const char *)FONT_PATH, TITLE_FONT);
    labelTitle_->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::White()));
    labelTitle_->SetStyle(STYLE_BACKGROUND_OPA, OPATICY_ALL);
    labelTitle_->SetText("我的家");

    rootview_->Add(labelTitle_);
}

void ElectrBoardAbilitySlice::ScheduleUpdateList(ListHandle list)
{
    int total = LinkListGetSize(list);

    if (scheduleList_.Size() > 0) {
        scheduleList_.Clear();
    }

    for (int i = 0; i < total; i++) {
        ScheduleInfo info_d;
        ScheduleInfoT *info_s = (ScheduleInfoT *)LinkListGetWithIndex(list, i);
        info_d.setId(info_s->id);
        info_d.setName(info_s->name);
        info_d.setTime(info_s->startTime, info_s->endTime);
        info_d.setDetails(info_s->details);

        scheduleList_.PushBack(info_d);
    }
    // scheduleView_->ScheduleListViewNotify();
}

static int ScheduleUpdateProcess(void *data, ListHandle list)
{
    ElectrBoardAbilitySlice *s = (ElectrBoardAbilitySlice *)data;
    s->ScheduleUpdateList(list);
    return 0;
}

void ElectrBoardAbilitySlice::OnStart(const Want &want)
{
    AbilitySlice::OnStart(want);
    ScheduleStartNetStatus();
    rootview_ = RootView::GetWindowRootView();
    rootview_->SetPosition(START_X_POS, START_Y_POS);
    rootview_->Resize(SCREEN_WIDTH, SCREEN_HEIGHT);
    rootview_->SetStyle(STYLE_BACKGROUND_OPA, OPATICY_NONE);
    rootview_->SetStyle(STYLE_BACKGROUND_COLOR, Color::ColorTo32(Color::GetColorFromRGB(0x30, 0x30, 0x30)));

    SetTitle();
    SetBackButton();
    SetDateItemView();
    SetScheduleView();
    SetScrollView();
    SetUIContent(rootview_);
    rootview_->Invalidate();

    ScheduleServerStart(ScheduleUpdateProcess, (void *)this);
}

void ElectrBoardAbilitySlice::OnInactive()
{
    AbilitySlice::OnInactive();
}

void ElectrBoardAbilitySlice::OnActive(const Want &want)
{
    AbilitySlice::OnActive(want);
}

void ElectrBoardAbilitySlice::OnBackground()
{
    AbilitySlice::OnBackground();
}

void ElectrBoardAbilitySlice::OnStop()
{
    AbilitySlice::OnStop();
}
}

