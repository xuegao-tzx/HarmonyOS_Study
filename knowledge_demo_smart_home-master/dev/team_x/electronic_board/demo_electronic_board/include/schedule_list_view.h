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

#ifndef __SCHEDULE_LIST_VIEW_H__
#define __SCHEDULE_LIST_VIEW_H__

#include <components/ui_image_view.h>
#include <components/ui_label.h>
#include <components/ui_scroll_view.h>
#include <components/ui_label_button.h>
#include <components/ui_swipe_view.h>
#include <common/task.h>
#include <list.h>

#include "ui_config.h"
#include "schedule_info.h"
#include "weather_time_view.h"

namespace OHOS {

class ScheduleListItem {
public:
    static constexpr int FONT_MAX = 22;
    static constexpr int FONT_MID = 20;
    static constexpr int FONT_MIN = 16;

    static constexpr int LIST_ITEM_WIDTH = 568;
    static constexpr int LIST_ITEM_HEIGHT = 96;

    static constexpr int ITEM_ICON_X = 20;
    static constexpr int ITEM_ICON_Y = 34;  // 36
    static constexpr int ITEM_ICON_W = 24;
    static constexpr int ITEM_ICON_H = 24;

    static constexpr int ITEM_TIME_X = 50;
    static constexpr int ITEM_TIME_Y = 32;
    static constexpr int ITEM_TIME_W = 135;
    static constexpr int ITEM_TIME_H = 30;

    static constexpr int ITEM_NAME_X = 194;
    static constexpr int ITEM_NAME_Y = 18;
    static constexpr int ITEM_NAME_W = 300;
    static constexpr int ITEM_NAME_H = 26;

    static constexpr int ITEM_DETAIL_X = 194;
    static constexpr int ITEM_DETAIL_Y = 52;
    static constexpr int ITEM_DETAIL_W = 300;
    static constexpr int ITEM_DETAIL_H = 45;

    static constexpr int ITEM_IMAGE_X = 517;
    static constexpr int ITEM_IMAGE_Y = 36;
    static constexpr int ITEM_IMAGE_W = 24;
    static constexpr int ITEM_IMAGE_H = 24;

    static constexpr char *UNSELECT_ICON_PATH =
        "/storage/app/run/com.huawei.electrboard/electrBoard/res/drawable/icon_shcedule_unselected.png";
    static constexpr char *SELECT_ICON_PATH =
        "/storage/app/run/com.huawei.electrboard/electrBoard/res/drawable/icon_shcedule_selected.png";
    static constexpr char *ITEM_BACKGROUND =
        "/storage/app/run/com.huawei.electrboard/electrBoard/res/drawable/BG_playing.png";

    ScheduleListItem(UIViewGroup *view, ScheduleInfo *info, int idx) : viewGroup(view), itemIdx(idx)
    {
        isSelected = false;
        (void)memcpy_s(&info_, sizeof(ScheduleInfo), info, sizeof(ScheduleInfo));
        CreateItem();
    }

    ~ScheduleListItem()
    {
        if (icon != nullptr) {
            delete icon;
            icon = nullptr;
        }
        if (labelTime != nullptr) {
            delete labelTime;
            labelTime = nullptr;
        }
        if (labelName != nullptr) {
            delete labelName;
            labelName = nullptr;
        }
        if (labelDetail != nullptr) {
            delete labelDetail;
            labelDetail = nullptr;
        }
    }

    void SetSelect(bool select)
    {
        UpdateItem(select);
    }

    bool IsSelect()
    {
        return isSelected;
    }

    int GetIdx()
    {
        return itemIdx;
    }
protected:
    void UpdateItem(bool select)
    {
        if (isSelected != select) {
            isSelected = select;
            if (isSelected) {
                icon->SetSrc(SELECT_ICON_PATH);
                background->SetVisible(true);
            } else {
                icon->SetSrc(UNSELECT_ICON_PATH);
                background->SetVisible(false);
            }
            icon->Invalidate();
            background->Invalidate();
        }
    }

    void CreateItem()
    {
        int retval = 0;

        background = new UIImageView();
        background->SetPosition(0, 0, LIST_ITEM_WIDTH, LIST_ITEM_HEIGHT);
        background->SetSrc(ITEM_BACKGROUND);
        background->SetStyle(STYLE_BACKGROUND_OPA, OPATICY_ALL);    // all opacity
        viewGroup->Add(background);
        background->SetVisible(false);

        icon = new UIImageView();
        icon->SetPosition(ITEM_ICON_X, ITEM_ICON_Y, ITEM_ICON_W, ITEM_ICON_H);
        icon->SetSrc(UNSELECT_ICON_PATH);
        icon->SetStyle(STYLE_BACKGROUND_OPA, OPATICY_ALL);    // all opacity
        viewGroup->Add(icon);

        char tmBuf[128] = {0};
        char startTime[32] = {0};
        char endTime[32] = {0};
        if ((info_.getStartTime(startTime, sizeof(startTime)) < 0) ||
            (info_.getEndTime(endTime, sizeof(endTime)) < 0)) {
            (void)sprintf_s(tmBuf, sizeof(tmBuf), "%02d:%02d-%02d:%02d", info_.getStartTimeHours(), 
                    info_.getStartTimeMinutes(), info_.getEndTimeHours(), info_.getEndTimeMinutes());
        } else {
            (void)sprintf_s(tmBuf, sizeof(tmBuf), "%s-%s", startTime, endTime);
        }
    
        labelTime = new UILabel();
        labelTime->SetPosition(ITEM_TIME_X, ITEM_TIME_Y, ITEM_TIME_W, ITEM_TIME_H);
        labelTime->SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
        labelTime->SetFont((const char *)FONT_PATH, FONT_MAX);
        labelTime->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::White()));
        labelTime->SetStyle(STYLE_BACKGROUND_OPA, OPATICY_ALL);
        labelTime->SetText((char *)tmBuf);
        viewGroup->Add(labelTime);

        char nameBuf[128] = {0};
        labelName = new UILabel();
        labelName->SetPosition(ITEM_NAME_X, ITEM_NAME_Y, ITEM_NAME_W, ITEM_NAME_H);
        labelName->SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
        labelName->SetFont((const char *)FONT_PATH, FONT_MID);
        labelName->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::White()));
        labelName->SetStyle(STYLE_BACKGROUND_OPA, OPATICY_ALL);

        retval = info_.getName(nameBuf, sizeof(nameBuf));
        if (retval == 0) {
            labelName->SetText((char *)nameBuf);
        } else {
            labelName->SetText("UNKnow Name");
        }

        viewGroup->Add(labelName);

        char details[128] = {0};
        labelDetail = new UILabel();
        labelDetail->SetPosition(ITEM_DETAIL_X, ITEM_DETAIL_Y, ITEM_DETAIL_W, ITEM_DETAIL_H);
        labelDetail->SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
        labelDetail->SetFont((const char *)FONT_PATH, FONT_MIN);
        labelDetail->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::White()));
        labelDetail->SetStyle(STYLE_BACKGROUND_OPA, OPATICY_ALL);
        labelDetail->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);

        retval = info_.getDetails(details, sizeof(details));
        if (retval == 0) {
            labelDetail->SetText((char *)details);
        } else {
            labelDetail->SetText("UnKnow details");
        }

        viewGroup->Add(labelDetail);
    }
private:
    UIViewGroup *viewGroup {nullptr};
    UIImageView *background {nullptr};
    UIImageView *icon {nullptr};
    UILabel *labelTime {nullptr};
    UILabel *labelName {nullptr};
    UILabel *labelDetail {nullptr};
    ScheduleInfo info_;

    bool isSelected;
    int itemIdx;
};

class ScheduleListView : public NativeBase {
public:
    ScheduleListView(UIViewGroup *group, WeatherTimeView *tmView);
    ~ScheduleListView();

    void ScheduleListViewSetList(List<ScheduleInfo> *list);
    void ScheduleListViewSetup();

    void ScheduleListViewNotify();
    void ScheduleListViewScroll();
    void ScheduleListViewSetSelect(int idx);

protected:
    void CreateScrollView();
    void ScheduleListViewUpdate();
    void SetScrollViewSelect();
    int GetCurrentIdx();
private:
    UIViewGroup *viewGroup {nullptr};
    WeatherTimeView *weatherTimeView {nullptr};
    UILabel *listTitle {nullptr};
    UIScrollView *scrollView {nullptr};
    UIImageView *background_ {nullptr};
    UILabel *emptyText_ {nullptr};
    List<ScheduleInfo> *itemList;

    int curSelect;
    int lastSelect;

    static constexpr int LABEL_TITLE_X = 20;
    static constexpr int LABEL_TITLE_Y = 0;
    static constexpr int LABEL_TITLE_W = 480;
    static constexpr int LABEL_TITLE_H = 68;

    static constexpr int SCROLL_VIEW_X = 0;
    static constexpr int SCROLL_VIEW_Y = 68;
    static constexpr int SCROLL_VIEW_W = 568;
    static constexpr int SCROLL_VIEW_H = 288;

    static constexpr int SCROLL_BACK_ICON_X = ((568 - 104) / 2);
    static constexpr int SCROLL_BACK_ICON_Y = ((288 - 104 - 36) / 2);
    static constexpr int SCROLL_BACK_ICON_W = 104;
    static constexpr int SCROLL_BACK_ICON_H = 104;

    static constexpr int SCROLL_EMPTY_TEXT_X = 0;
    static constexpr int SCROLL_EMPTY_TEXT_Y = (SCROLL_BACK_ICON_Y + SCROLL_BACK_ICON_H + 10);
    static constexpr int SCROLL_EMPTY_TEXT_W = 568;
    static constexpr int SCROLL_EMPTY_TEXT_H = 36;

    static constexpr int SCROLL_ITEM_WIDTH = 568;
    static constexpr int SCROLL_ITEM_HEIGHT = 96;

    static constexpr char *EMPTY_ICON_PATH =
        "/storage/app/run/com.huawei.electrboard/electrBoard/res/drawable/state_no schdule.png";
};
}

#endif  /* __SCHEDULE_LIST_VIEW_H__ */
