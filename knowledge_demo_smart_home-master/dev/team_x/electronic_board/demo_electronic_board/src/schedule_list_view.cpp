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

#include "schedule_list_view.h"

namespace OHOS {

ScheduleListView::ScheduleListView(UIViewGroup *group, WeatherTimeView *tmView)
{
    viewGroup = group;
    weatherTimeView = tmView;

    curSelect = 0;
    lastSelect = 0;
}

ScheduleListView::~ScheduleListView()
{
    if (listTitle != nullptr) {
        delete listTitle;
        listTitle = nullptr;
    }

    if (background_ != nullptr) {
        delete background_;
        background_ = nullptr;
    }

    if (scrollView != nullptr) {
        delete scrollView;
        scrollView = nullptr;
    }
}

void ScheduleListView::ScheduleListViewSetSelect(int idx)
{
    lastSelect = curSelect;
    curSelect = idx;
}

void ScheduleListView::ScheduleListViewSetList(List<ScheduleInfo> *list)
{
    itemList = list;
}

void ScheduleListView::ScheduleListViewNotify()
{
    ScheduleListViewUpdate();
}

void ScheduleListView::ScheduleListViewSetup()
{
    listTitle = new UILabel();
    listTitle->SetPosition(LABEL_TITLE_X, LABEL_TITLE_Y, LABEL_TITLE_W, LABEL_TITLE_H);
    listTitle->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_LEFT, UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
    listTitle->SetFont((const char *)FONT_PATH, FONT_SML_ID);
    listTitle->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::White()));
    listTitle->SetStyle(STYLE_BACKGROUND_OPA, OPATICY_ALL);
    listTitle->SetText((char *)"今日日程:");
    viewGroup->Add(listTitle);
    weatherTimeView->SetTitle(listTitle);

    scrollView = new UIScrollView();
    scrollView->SetStyle(STYLE_BACKGROUND_COLOR, Color::ColorTo32(Color::Black()));
    scrollView->SetPosition(SCROLL_VIEW_X, SCROLL_VIEW_Y, SCROLL_VIEW_W, SCROLL_VIEW_H);
    scrollView->SetXScrollBarVisible(false);
    scrollView->SetHorizontalScrollState(false);
    scrollView->SetStyle(STYLE_BACKGROUND_OPA, OPATICY_ALL);
    scrollView->SetYScrollBarVisible(true);
    scrollView->SetVerticalScrollState(true);

    background_ = new UIImageView();
    background_->SetPosition(SCROLL_BACK_ICON_X, SCROLL_BACK_ICON_Y, SCROLL_BACK_ICON_W, SCROLL_BACK_ICON_H);
    background_->SetStyle(STYLE_BACKGROUND_OPA, OPATICY_ALL);
    background_->SetSrc(EMPTY_ICON_PATH);

    emptyText_ = new UILabel();
    emptyText_->SetPosition(SCROLL_EMPTY_TEXT_X, SCROLL_EMPTY_TEXT_Y, SCROLL_EMPTY_TEXT_W, SCROLL_EMPTY_TEXT_H);
    emptyText_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    emptyText_->SetFont((const char *)FONT_PATH, 24);
    emptyText_->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::White()));
    emptyText_->SetStyle(STYLE_BACKGROUND_OPA, OPATICY_ALL);
    emptyText_->SetText("暂无日程");

    viewGroup->Add(background_);
    viewGroup->Add(emptyText_);
    viewGroup->Add(scrollView);

    background_->SetVisible(false);
    emptyText_->SetVisible(false);
}

void ScheduleListView::ScheduleListViewUpdate()
{
    if (scrollView == nullptr) {
        LOG_E("null point! \n");
        return;
    }
    if (itemList->Size() == 0) {
        LOG_E("list has no item! \n");
        scrollView->SetVisible(false);
        background_->SetVisible(true);
        emptyText_->SetVisible(true);

        return;
    }

    background_->SetVisible(false);
    emptyText_->SetVisible(false);
    scrollView->SetVisible(true);
    
    scrollView->RemoveAll();
    ListNode<ScheduleInfo> *pNode = itemList->Head();
    for (int i = 0; i < itemList->Size(); i++) {
        int x = 0;
        int y = (i * SCROLL_ITEM_HEIGHT);
        int w = SCROLL_ITEM_WIDTH;
        int h = SCROLL_ITEM_HEIGHT;

        if (pNode == nullptr) {
            LOG_E("list item is NULL! (%d ---> %d)\n", i, itemList->Size());
            break;
        }
        ScheduleInfo info = pNode->data_;
        UIViewGroup *vp = new UIViewGroup();
        vp->SetPosition(x, y, w, h);
        vp->SetStyle(STYLE_BACKGROUND_OPA, OPATICY_ALL);
        ScheduleListItem *item = new ScheduleListItem(vp, &info, i);
        if (i == curSelect) {
            item->SetSelect(true);
        }
        scrollView->Add(vp);
        pNode = pNode->next_;
    }
    SetScrollViewSelect();
}

void ScheduleListView::SetScrollViewSelect()
{
    int x, y;
    x = 0;
    y = curSelect * SCROLL_ITEM_HEIGHT;
    scrollView->ScrollBy(x, -y);
}
}
