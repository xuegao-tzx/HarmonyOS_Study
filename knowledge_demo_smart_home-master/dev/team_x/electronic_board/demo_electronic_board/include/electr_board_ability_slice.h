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

#ifndef __ELECTR_BOARD_ABILITY_SLICE_H__
#define __ELECTR_BOARD_ABILITY_SLICE_H__

#include <ability_info.h>
#include <ability_loader.h>
#include <bundle_info.h>
#include <components/root_view.h>
#include <components/ui_image_view.h>
#include <components/ui_label.h>
#include <components/ui_scroll_view.h>
#include <components/ui_label_button.h>
#include <components/ui_swipe_view.h>
#include <common/task.h>
#include <list.h>
#include <time.h>
#include "native_base.h"
#include "ui_config.h"
#include "event_listener.h"
#include "schedule_info.h"
#include "common.h"
#include "weather_time_view.h"
#include "schedule_list_view.h"
#include "schedule_server.h"

namespace OHOS {
class ScheduleListTask : public Task {
public:
    ScheduleListTask(WeatherTimeView *tmView, ScheduleListView *view,
        List<ScheduleInfo> *list) : weatherTimeView(tmView),
        scheduleView(view), itemList(list) {
        curIdx = -1;
        Task::Init();
    }
    ~ScheduleListTask(){}

    void TaskStart()
    {
        Task::SetPeriod(1000);    /* 1000=1s */
        Task::TaskExecute();
    }

    void SetList(List<ScheduleInfo> *list)
    {
        itemList = list;
    }

    void Callback() override
    {
        int idx = GetCurrentIdx();
        if (idx >=0 && idx != curIdx) {
            SetScrollViewIdx(idx);
            curIdx = idx;
        }
    }

private:
    List<ScheduleInfo> *itemList {nullptr};
    ScheduleListView *scheduleView {nullptr};
    WeatherTimeView *weatherTimeView {nullptr};
    static constexpr int ONE_HOUR = 60;
    int curIdx;

    int GetCurrentIdx()
    {
        int retval = -1;
        int i = 0;
        int hour = 0;
        int minute = 0;
        weatherTimeView->GetTime(&hour, &minute);
        if (itemList == nullptr || itemList->Size() == 0) {
            return -1;
        }

        ListNode<ScheduleInfo> *pNode = itemList->Head();
        for (i = 0; i < itemList->Size(); i++) {
            if (pNode == nullptr) {
                return -1;
            }

            if ((hour * ONE_HOUR + minute) < (pNode->data_.getEndTimeHours() * ONE_HOUR + pNode->data_.getEndTimeMinutes())) {
                retval = i;
                break;
            }
            pNode = pNode->next_;
        }
        if (i == itemList->Size()) {
            retval = itemList->Size() - 1;
        }

        return retval;
    }

    void SetScrollViewIdx(int idx)
    {
        if (scheduleView == nullptr) {
            return;
        }
        scheduleView->ScheduleListViewSetSelect(idx);
        scheduleView->ScheduleListViewNotify();
    }
};

class ElectrBoardAbilitySlice : public AbilitySlice, public NativeBase {
public:
    ElectrBoardAbilitySlice() = default;
    ~ElectrBoardAbilitySlice() override;

    void ScheduleUpdateList(ListHandle list);
protected:
    void OnStart(const Want &want) override;
    void OnInactive() override;
    void OnActive(const Want &want) override;
    void OnBackground() override;
    void OnStop() override;

    void SetDateItemView();
    void SetScheduleView();
    void SetBackButton();
    void SetScrollView();
    void SetTitle();

private:
    RootView *rootview_ {nullptr};
    UILabel *labelTitle_ {nullptr};

    UIImageView *backButtonB_ {nullptr};
    UIImageView *backButton_ {nullptr};
    EventListener *buttonListener_ {nullptr};

    UIViewGroup *dateInfo_ {nullptr};
    WeatherTimeView *weatherTimeView_ {nullptr};

    UIViewGroup *scheduleInfo_ {nullptr};
    ScheduleListView *scheduleView_ {nullptr};
    ScheduleListTask *scheduleTask_ {nullptr};

    List<ScheduleInfo> scheduleList_;
};
}

#endif // __ELECTR_BOARD_ABILITY_SLICE_H__
