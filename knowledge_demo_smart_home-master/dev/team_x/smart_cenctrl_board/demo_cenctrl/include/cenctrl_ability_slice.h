/*
* Copyright(c) 2021 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the license is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permisssions and
* limitations under the License.
*/

#ifndef _OHOS_CENCTRL_ABILITY_SLICE_H_
#define _OHOS_CENCTRL_ABILITY_SLICE_H_

#include <ability_info.h>
#include <ability_loader.h>
#include <bundle_info.h>
#include <components/root_view.h>
#include <components/ui_image_view.h>
#include <components/ui_label.h>
#include <components/ui_label_button.h>
#include <components/ui_toggle_button.h>
#include <components/ui_scroll_view.h>
#include <components/ui_slider.h>
#include <layout/list_layout.h>
#include <layout/grid_layout.h>
#include <components/ui_swipe_view.h>
#include <components/text_adapter.h>
#include <components/ui_list.h>
#include <list.h>

#include "event_listener.h"
#include "ui_config.h"
#include "native_base.h"
#include "camera_ability_slice.h"
#include "item_list_ctrl.h"
#include "socket_service.h"
#include "cenctrl_log.h"

#define MAX_LIST_ITEM 5
namespace OHOS {
const char* g_strPageName[MAX_LIST_ITEM] = {"首页", "灯光", "空调", "窗帘", "相机"};
static UILabel* g_pLabelButton[MAX_LIST_ITEM] = {nullptr};

class ClickListener : public UIView::OnClickListener {
public:
    ClickListener(UIViewGroup* view, int index, CameraAbilitySlice* cameraView, UISwipeView* swipeView)
    {
        swipeView_ = swipeView;
        view_ = view;
        index_ = index;
        cameraView_ = cameraView;
    }

    virtual ~ClickListener(){}
    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        swipeView_->SetCurrentPage(index_);
        for (int i = 0; i < MAX_LIST_ITEM; i++) {
            if (view_->GetChildById(g_strPageName[i])) {
                (view_->GetChildById(g_strPageName[i]))->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::White()));
            } else {
                LOG(CENCTRL_ERROR, "click err\n");
            }
            if (g_pLabelButton[i]) {
                g_pLabelButton[i]->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::White()));
                g_pLabelButton[i]->Invalidate();
            }
        }

        if (view_->GetChildById(g_strPageName[index_])) {
            (view_->GetChildById(g_strPageName[index_]))->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::Green()));
        }

        if (g_pLabelButton[index_]) {
            g_pLabelButton[index_]->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::Green()));
        }

        if (memcmp("相机", g_strPageName[index_], strlen(g_strPageName[index_])) == 0 && cameraView_) {
            LOG(CENCTRL_DEBUG, "camera on click OnActive\n");
            cameraView_->OnActive();
        } else if (cameraView_) {
            LOG(CENCTRL_DEBUG, "camera on click OnBackground\n");
            cameraView_->OnBackground();
        }

        g_pLabelButton[index_]->Invalidate();
        swipeView_->Invalidate();
        view_->Invalidate();

        return true;
    }
private:
    UISwipeView* swipeView_{ nullptr };
    UIViewGroup* view_ { nullptr };
    int index_;
    CameraAbilitySlice* cameraView_{ nullptr };
};

class SwipeListener : public UISwipeView::OnSwipeListener {
public:
    SwipeListener(UIViewGroup* view, CameraAbilitySlice* cameraView, UISwipeView* swipeView)
        : view_(view), cameraView_(cameraView), swipeView_(swipeView){}
    ~SwipeListener(){}
    virtual void OnSwipe(UISwipeView& view) override
    {
        int index = swipeView_->GetCurrentPage();
        LOG(CENCTRL_DEBUG, "ONSwipe, page index = %d\n", index);
        if (index > MAX_LIST_ITEM || index < 0) {
            LOG(CENCTRL_ERROR, "page err. index = %d\n", index);
            return;
        }
        for (int i = 0; i<MAX_LIST_ITEM; i++) {
            if (view_->GetChildById(g_strPageName[i])) {
                (view_->GetChildById(g_strPageName[i]))->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::White()));
            }
            if (g_pLabelButton[i]) {
                g_pLabelButton[i]->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::White()));
                g_pLabelButton[i]->Invalidate();
            }
        }
        if ((view_->GetChildById(g_strPageName[index]))) {
            (view_->GetChildById(g_strPageName[index]))->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::Green()));
        }
        if (g_pLabelButton[index]) {
            g_pLabelButton[index]->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::Green()));
        }
        if (memcmp("相机", g_strPageName[index], strlen(g_strPageName[index])) == 0 && cameraView_) {
            LOG(CENCTRL_DEBUG, "camera on click OnActive\n");
            cameraView_->OnActive();
        } else if (cameraView_) {
            LOG(CENCTRL_DEBUG, "camera on swipe OnBackground\n");
            cameraView_->OnBackground();
        }

        g_pLabelButton[index]->Invalidate();
        view_->Invalidate();
    }
private:
    UISwipeView* swipeView_{ nullptr };
    UIViewGroup* view_{ nullptr };
    CameraAbilitySlice* cameraView_{ nullptr };
};
class CenctrlAbilitySlice : public AbilitySlice,
                            public UIView::OnClickListener,
                            public NativeBase {
public:
    CenctrlAbilitySlice() = default;
    virtual ~CenctrlAbilitySlice();
    void SetUpView();
protected:
    void OnStart(const Want& want) override;
    void OnInactive() override;
    void OnActive(const Want& want) override;
    void OnBackground() override;
    void OnStop() override;

    void InitServerSocket();
    void SetBackground();
    // modify by 2020.10.13
    void SetItemList();
    UILabel* SetUpButton(int index, std::string buttonName, int x, UIViewGroup* view);
    // set swipe view
    void SetUpSwipeView();
    void SetUpCommonView(std::string labelName, std::string strRes, GridLayout** ppLayout);
    void SetUpFirstViewLabel(UIViewGroup* view);
    void SetUpFirstView();
    void SetUpLightView();
    void SetUpAirConditionView();
    void SetUpCurtainsView();
    void SetUpCameraView();
    void SetUpItem(std::string discrible, std::string iconDir, GridLayout* layout,
                    std::string devName = "", std::string changeIconDir = "");
    static int DisplayTP(int data, int devStatus);
    static int DisplayTH(int data, int devStatus);
private:
    List<EventListener*> listeners_;
    List<ClickListener*> clickListeners_;
    List<ItemCtrl*> itemListeners_;

    UIImageView* uiImageView_{ nullptr };
    RootView *rootview_{ nullptr };

    UISwipeView* swipeView_{ nullptr };
    SwipeListener* swipeListener_{ nullptr };
    UIViewGroup* itemView_{ nullptr };
    CameraAbilitySlice* cameraView_;
};

class CtrlCallBack {
public:
    ~CtrlCallBack();
    static void SetInstancePram(List<ItemCtrl*>*itemList);
    static void CtrlOnConnect(char* devName, int status);
    static void CtrlOnDisconnect(char* devName, int status);
    static void CtrlOn(char* devName, int status);
    static void CtrlOff(char* devName, int status);
private:
    CtrlCallBack(){};
    static List<ItemCtrl*>* pItemListeners_;
};
}   // namespace OHOS
#endif

