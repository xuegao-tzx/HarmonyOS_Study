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

#include <stdio.h>
#include <common/screen.h>

#include "cenctrl_ability_slice.h"
#include "ability_manager.h"
#include "socket_service.h"
#include "socket_udp.h"
#include "cenctrl_log.h"

namespace OHOS {
REGISTER_AS(CenctrlAbilitySlice)

UILabel* g_labelTemp_ { nullptr};
UILabel* g_labelTP_ { nullptr};
UILabel* g_labelTH_ { nullptr};

List<ItemCtrl*>* CtrlCallBack::pItemListeners_ = { nullptr};

void CtrlCallBack::SetInstancePram(List<ItemCtrl*>* itemList)
{
    if (itemList) {
        pItemListeners_ = itemList;
    }
}

void CtrlCallBack::CtrlOnConnect(char* devName, int status)
{
    ListNode<ItemCtrl*>* item = pItemListeners_->Begin();
    while (item != pItemListeners_->End()) {
        if (strcmp(item->data_->devName_.c_str(), devName) == 0) {
            item->data_->OnConnect(devName, status);
            break;
        }
        item = item->next_;
    }
}

void CtrlCallBack::CtrlOnDisconnect(char* devName, int status)
{
    ListNode<ItemCtrl*>* item = pItemListeners_->Begin();
    while (item != pItemListeners_->End()) {
        if (strcmp(item->data_->devName_.c_str(), devName) == 0) {
            item->data_->OnDisconnect(devName, status);
            break;
        }
        item = item->next_;
    }
}

void CtrlCallBack::CtrlOn(char* devName, int status)
{
    ListNode<ItemCtrl*>* item = pItemListeners_->Begin();
    while (item != pItemListeners_->End()) {
        if (strcmp(item->data_->devName_.c_str(), devName) == 0) {
            item->data_->On(devName, status);
            break;
        }
        item = item->next_;
    }
}

void CtrlCallBack::CtrlOff(char* devName, int status)
{
    ListNode<ItemCtrl*>* item = pItemListeners_->Begin();
    while (item != pItemListeners_->End()) {
        if (strcmp(item->data_->devName_.c_str(), devName) == 0) {
            item->data_->Off(devName, status);
            break;
        }
        item = item->next_;
    }
}

CenctrlAbilitySlice::~CenctrlAbilitySlice()
{
    if (cameraView_) {
        delete cameraView_;
        cameraView_ = nullptr;
    }

    SocketServiceDelete();
    SocketUdpDelBC();

    DeleteChildren(swipeView_);

    ListNode<EventListener*>* listen = listeners_.Begin();
    while(listen != listeners_.End()) {
        delete listen->data_;
        listen = listen->next_;
    }
    listeners_.Clear();

    ListNode<ClickListener*>* click = clickListeners_.Begin();
    while(click != clickListeners_.End()) {
        delete click->data_;
        click = click->next_;
    }
    clickListeners_.Clear();

    ListNode<ItemCtrl*>* item = itemListeners_.Begin();
    while(item != itemListeners_.End()) {
        delete item->data_;
        item = item->next_;
    }
    itemListeners_.Clear();

    if(swipeListener_) {
        delete swipeListener_;
        swipeListener_ = nullptr;
    }
}

void CenctrlAbilitySlice::SetBackground()
{
    uiImageView_ = new UIImageView();
    uiImageView_->SetPosition(0, 0, ROOTVIEW_W, ROOTVIEW_H);
    uiImageView_->SetSrc(RES_BACKGROUND);
    rootview_->Add(uiImageView_);
}

void CenctrlAbilitySlice::SetUpView()
{
    SetUpSwipeView();
    SetItemList();
    swipeListener_ = new SwipeListener(itemView_, cameraView_, swipeView_);
    swipeView_->SetOnSwipeListener(swipeListener_);
}

void CenctrlAbilitySlice::SetItemList()
{
    UIViewGroup* view = new UIViewGroup();
    view->SetPosition(0, SWIPE_VIEW_H, SWIPE_VIEW_W, ITEM_LIST_HEIGHT);
    view->SetStyle(STYLE_BACKGROUND_OPA, TOTAL_OPACITY);
    rootview_->Add(view);
    itemView_ = view;

    UIImageView* imageView = new UIImageView();
    imageView->SetPosition(BACK_IMAGE_X, BACK_IMAGE_Y, BACK_IMAGE_W, BACK_IMAGE_H);
    imageView->SetStyle(STYLE_BACKGROUND_OPA, TOTAL_OPACITY);
    imageView->SetStyle(STYLE_BACKGROUND_COLOR, Color::ColorTo32(Color::Black()));
    imageView->SetSrc(RES_BACK);
    imageView->SetTouchable(true);
    imageView->SetViewId("back");

    auto toLauncher = [this] (UIView &view, const Event &event) -> bool {
        TerminateAbility();
        return true;
    };

    EventListener* buttonBackListener = new EventListener(toLauncher, nullptr);
    listeners_.PushBack(buttonBackListener);
    imageView->SetOnClickListener(buttonBackListener);
    view->Add(imageView);
    static int positionX = 200;
    static int width = 150;
    for (int i = 0; i < MAX_LIST_ITEM; i++) {
        g_pLabelButton[i] = SetUpButton(i, g_strPageName[i], positionX + width * i, view);
    }
    g_pLabelButton[0]->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::Green()));
    view->Invalidate();
}

UILabel* CenctrlAbilitySlice::SetUpButton(int index, std::string buttonName, int x, UIViewGroup* view)
{
    static int width = 130;
    static int height = 60;
    UILabel* button = new UILabel();
    view->Add(button);
    button->SetPosition(x, BLANK_H, width, height);
    button->SetFont(FOND_PATH, BIG_FOND_ID);
    button->SetStyle(STYLE_BORDER_RADIUS, LIGHT_RADIO);
    button->SetStyle(STYLE_BACKGROUND_OPA, TOTAL_OPACITY);
    button->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::White()));
    button->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    button->SetText(buttonName.c_str());
    button->SetViewId(buttonName.c_str());
    button->SetTouchable(true);
    ClickListener* listener = new ClickListener(itemView_, index, cameraView_, swipeView_);
    button->SetOnClickListener(listener);
    clickListeners_.PushBack(listener);
    return button;
}

void CenctrlAbilitySlice::SetUpSwipeView()
{
    const int animatorTime = 20;
    swipeView_ = new UISwipeView();
    swipeView_->SetPosition(0, 0, SWIPE_VIEW_W, SWIPE_VIEW_H);
    swipeView_->SetStyle(STYLE_BACKGROUND_OPA, TOTAL_OPACITY);
    swipeView_->SetLoopState(true);
    swipeView_->SetAnimatorTime(animatorTime);

    rootview_->Add(swipeView_);
    SetUpFirstView();
    SetUpLightView();
    SetUpAirConditionView();
    SetUpCurtainsView();
    SetUpCameraView();
    swipeView_->SetCurrentPage(0);
    swipeView_->Invalidate();
}

void CenctrlAbilitySlice::SetUpItem(std::string discrible, std::string iconDir, GridLayout* layout,
                                    std::string devName, std::string changeIconDir)
{
    UIViewGroup* view = new UIViewGroup();
    view->Resize(ITEM_VIEW_W, ITEM_VIEW_H);
    view->SetStyle(STYLE_BACKGROUND_OPA, HALF_OPACITY);
    view->SetStyle(STYLE_BORDER_RADIUS, GROUP_VIEW_RADIUS);
    view->SetStyle(STYLE_BACKGROUND_COLOR, Color::ColorTo32(Color::Gray()));
    view->SetTouchable(true);
    layout->Add(view);

    UILabelButton* button = new UILabelButton();
    button->SetPosition(ZERO_POSITION, ZERO_POSITION, view->GetWidth(), view->GetHeight());
    button->SetStyle(STYLE_BACKGROUND_OPA, TOTAL_OPACITY);
    button->SetStyle(STYLE_BORDER_RADIUS, BUTTON_RADIUS);
    button->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_RADIUS, UIButton::PRESSED);
    button->SetStyleForState(STYLE_BACKGROUND_OPA, UN_OPACITY, UIButton::PRESSED);
    view->Add(button);

    UIImageView* imageView = new UIImageView();
    imageView->SetPosition(ITEM_IMAGE_X, ITEM_IMAGE_Y, ITEM_IMAGE_W, ITEM_IMAGE_H);
    imageView->SetSrc(iconDir.c_str());
    view->Add(imageView);

    UILabel* labelFont = new UILabel();
    labelFont->SetPosition(BLANK_W, LABEL_IMAGE_Y, LABEL_IMAGE_W, LABEL_IMAGE_H);
    labelFont->SetText(discrible.c_str());
    labelFont->SetFont(FOND_PATH, BIGBIG_FOND_ID);
    labelFont->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::Gray()));
    labelFont->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelFont->SetStyle(STYLE_BACKGROUND_OPA, TOTAL_OPACITY);
    view->Add(labelFont);

    ItemCtrl* item = new ItemCtrl();
    item->Setup(view, labelFont, imageView, iconDir, changeIconDir, devName);
    itemListeners_.PushBack(item);
    button->SetOnClickListener(item);
}

void CenctrlAbilitySlice::SetUpCommonView(std::string labelName, std::string strRes, GridLayout** ppLayout)
{
    UIViewGroup* comView = new UIViewGroup();
    comView->SetPosition(ZERO_POSITION, ZERO_POSITION, SWIPE_VIEW_W, SWIPE_VIEW_H);
    comView->SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    comView->SetStyle(STYLE_BACKGROUND_COLOR, Color::ColorTo32(Color::Black()));
    swipeView_->Add(comView);

    UIViewGroup* view = new UIViewGroup();
    view->SetPosition(OTHER_VIEW_X, OTHER_VIEW_Y, OTHER_VIEW_W, OTHER_VIEW_H);
    view->SetStyle(STYLE_BORDER_RADIUS, GROUP_VIEW_RADIUS);
    view->SetStyle(STYLE_BACKGROUND_OPA, HALF_OPACITY);
    view->SetStyle(STYLE_BACKGROUND_COLOR, Color::ColorTo32(Color::Gray()));
    comView->Add(view);

    UIImageView* imageView = new UIImageView();
    imageView->SetPosition(OTHER_VIEW_IM_X, OTHER_VIEW_IM_Y, OTHER_VIEW_IM_W, OTHER_VIEW_IM_H);
    imageView->SetSrc(strRes.c_str());
    view->Add(imageView);

    UILabel* labelFont = new UILabel();
    labelFont->SetPosition(OTHER_VIEW_LB_X, OTHER_VIEW_LB_Y, OTHER_VIEW_LB_W, OTHER_VIEW_LB_H);
    labelFont->SetText(labelName.c_str());
    labelFont->SetFont(FOND_PATH, BIG_FOND_ID);
    labelFont->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::White()));
    labelFont->SetStyle(STYLE_BACKGROUND_OPA, TOTAL_OPACITY);
    labelFont->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    view->Add(labelFont);

    GridLayout* layout = new GridLayout();
    layout->SetPosition(OTHER_VIEW_LY_X, OTHER_VIEW_LY_Y, OTHER_VIEW_LY_W, OTHER_VIEW_LY_H);
    layout->SetStyle(STYLE_BACKGROUND_OPA, TOTAL_OPACITY);
    layout->SetLayoutDirection(LAYOUT_HOR);
    layout->SetRows(LAYOUT_ROWS);
    layout->SetCols(LAYOUT_COLS);
    comView->Add(layout);
    *ppLayout = layout;
}

void CenctrlAbilitySlice::SetUpFirstViewLabel(UIViewGroup* view)
{
    UIImageView* imageView = new UIImageView();
    imageView->SetPosition(FIRST_VIEW_IMAGE_X, FIRST_VIEW_IMAGE_Y, FIRST_VIEW_IMAGE_W, FIRST_VIEW_IMAGE_H);
    imageView->SetSrc(RES_AIR_QUA);
    view->Add(imageView);

    UILabel* label1 = new UILabel();
    label1->SetPosition(FIRST_VIEW_LB2_X, FIRST_VIEW_LB2_Y, FIRST_VIEW_LB2_W, FIRST_VIEW_LB2_H);
    label1->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    label1->SetText("空气质量      优");
    label1->SetFont(FOND_PATH, BIG_FOND_ID);
    label1->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::White()));
    label1->SetStyle(STYLE_BORDER_RADIUS, LABEL_RADIUS);
    label1->SetStyle(STYLE_BACKGROUND_OPA, TOTAL_OPACITY);
    view->Add(label1);

    UIImageView* imageView2 = new UIImageView();
    imageView2->SetPosition(FIRST_VIEW_IMAGE_X, FIRST_VIEW_IMAGE1_Y, FIRST_VIEW_IMAGE_W, FIRST_VIEW_IMAGE_H);
    imageView2->SetSrc(RES_AIR_HUM);
    view->Add(imageView2);

    UILabel* label2 = new UILabel();
    label2->SetPosition(FIRST_VIEW_LB2_X, FIRST_VIEW_LB3_Y, FIRST_VIEW_LB2_W, FIRST_VIEW_LB2_H);
    label2->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    label2->SetText("空气湿度      50");
    label2->SetFont(FOND_PATH, BIG_FOND_ID);
    label2->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::White()));
    label2->SetStyle(STYLE_BORDER_RADIUS, LABEL_RADIUS);
    label2->SetStyle(STYLE_BACKGROUND_OPA, TOTAL_OPACITY);
    view->Add(label2);
    g_labelTP_ = label1;
    g_labelTH_ = label2;
}

void CenctrlAbilitySlice::SetUpFirstView()
{
    UIViewGroup* lightView = new UIViewGroup();
    lightView->SetPosition(ZERO_POSITION, ZERO_POSITION, SWIPE_VIEW_W, SWIPE_VIEW_H);
    lightView->SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    lightView->SetStyle(STYLE_BACKGROUND_COLOR, Color::ColorTo32(Color::Black()));
    swipeView_->Add(lightView);

    UIViewGroup* view = new UIViewGroup();
    view->SetPosition(FIRST_VIEW_X, FIRST_VIEW_Y, FIRST_VIEW_W, FIRST_VIEW_H);
    view->SetStyle(STYLE_BORDER_RADIUS, GROUP_VIEW_RADIUS);
    view->SetStyle(STYLE_BACKGROUND_OPA, HALF_OPACITY);
    view->SetStyle(STYLE_BACKGROUND_COLOR, Color::ColorTo32(Color::Gray()));
    lightView->Add(view);

    UILabel* label = new UILabel();
    label->SetPosition(ZERO_POSITION, FIRST_VIEW_LB_Y, FIRST_VIEW_LB_W, FIRST_VIEW_LB_H);
    label->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    label->SetText("温度 22°C");
    label->SetFont(FOND_PATH, BIGBIG_FOND_ID);
    label->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::White()));
    label->SetStyle(STYLE_BORDER_RADIUS, LABEL_RADIUS);
    label->SetStyle(STYLE_BACKGROUND_OPA, TOTAL_OPACITY);
    view->Add(label);
    SetUpFirstViewLabel(view);
    g_labelTemp_ = label;

    GridLayout* layout = new GridLayout();
    layout->SetPosition(FIRST_VIEW_LY_X, FIRST_VIEW_LY_Y, FIRST_VIEW_LY_W, FIRST_VIEW_LY_H);
    layout->SetLayoutDirection(LAYOUT_HOR);
    layout->SetRows(LAYOUT_ROWS);
    layout->SetCols(LAYOUT_ROWS);
    lightView->Add(layout);

    SetUpItem("深夜模式", RES_DEEPLIGHT_MODE, layout);
    SetUpItem("用餐模式", RES_DINNER_MODE, layout);
    SetUpItem("会客模式", RES_GUEST_MODE, layout);
    SetUpItem("全关模式", RES_OFF_MODE, layout);
    layout->LayoutChildren();
    rootview_->Invalidate();
}

void CenctrlAbilitySlice::SetUpLightView()
{
    GridLayout* layout = nullptr;
    SetUpCommonView("灯光", RES_LIGHT, &layout);
    SetUpItem("客厅筒灯", RES_LIVINGROOM_UNSELECT, layout, "LRLIGHT", RES_LIVINGROOM_SELECT);
    SetUpItem("餐厅筒灯", RES_DININGROOM_UNSELECT, layout, "DRLIGHT", RES_DININGROOM_SELECT);
    SetUpItem("灯带", RES_BELT_UNSELECT, layout, "LANTERN", RES_BELT_SELECT);
    SetUpItem("客厅筒灯", RES_LIVINGROOM_UNSELECT, layout, "LRLIGHT1", RES_LIVINGROOM_UNSELECT);
    SetUpItem("餐厅筒灯", RES_DININGROOM_UNSELECT, layout, "DRLIGHT1", RES_DININGROOM_SELECT);
    SetUpItem("过道筒灯", RES_AISLE_UNSELECT, layout, "AILIGHT", RES_AISLE_SELECT);
    layout->LayoutChildren();
    rootview_->Invalidate();
}

void CenctrlAbilitySlice::SetUpAirConditionView()
{
    GridLayout* layout = nullptr;
    SetUpCommonView("空调", RES_AIRCONDITION, &layout);

    UIViewGroup* myview = new UIViewGroup();
    myview->Resize(ITEM_VIEW_W, ITEM_VIEW_H);
    myview->SetStyle(STYLE_BACKGROUND_OPA, HALF_OPACITY);
    myview->SetStyle(STYLE_BORDER_RADIUS, GROUP_VIEW_RADIUS);
    myview->SetStyle(STYLE_BACKGROUND_COLOR, Color::ColorTo32(Color::Gray()));
    layout->Add(myview);

    UILabel* temp = new UILabel();
    temp->SetPosition(TEMP_X, TEMP_Y, TEMP_W, TEMP_H);
    temp->SetText("22°C");
    temp->SetFont(FOND_PATH, BIGBIG_FOND_ID);
    temp->SetStyle(STYLE_TEXT_COLOR, Color::ColorTo32(Color::White()));
    temp->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    temp->SetStyle(STYLE_BACKGROUND_OPA, TOTAL_OPACITY);
    myview->Add(temp);

    SetUpItem("制冷", RES_COLD_UNSELECT, layout);
    SetUpItem("温度+", RES_TEMP_PLUS_UNSELECT, layout);
    SetUpItem("风速", RES_WIND_UNSELECT, layout);
    SetUpItem("开关", RES_SWITCH_UNSELECT, layout);
    SetUpItem("温度-", RES_TEMP_REDUCE_UNSELECT, layout);
    layout->LayoutChildren();
    rootview_->Invalidate();
}

void CenctrlAbilitySlice::SetUpCurtainsView()
{
    GridLayout* layout = nullptr;
    SetUpCommonView("窗帘", RES_CURTAIN, &layout);

    SetUpItem("窗帘开", RES_CURTAIN_SELECT, layout);
    SetUpItem("纱窗开", RES_CURTAIN_SELECT, layout);
    SetUpItem("窗户开", RES_CURTAIN_SELECT, layout);
    SetUpItem("窗帘关", RES_CURTAIN_OFF_SELECT, layout);
    SetUpItem("纱窗关", RES_CURTAIN_OFF_SELECT, layout);
    SetUpItem("窗户关", RES_CURTAIN_OFF_SELECT, layout);
    layout->LayoutChildren();
    rootview_->Invalidate();
}

void CenctrlAbilitySlice::SetUpCameraView()
{
    cameraView_ = new CameraAbilitySlice(swipeView_);
    cameraView_->OnStart();
}

int CenctrlAbilitySlice::DisplayTP(int data, int devStatus)
{
    char buf[TMP_BUF_SIZE] = {0};
    if (sprintf_s(buf, TMP_BUF_SIZE - 1, "温度 %d°C", data) != 0) {
        return -1;
    }
    g_labelTemp_->SetText(static_cast<char *>(buf));
    g_labelTemp_->Invalidate();

    return 0;
}

int CenctrlAbilitySlice::DisplayTH(int data, int devStatus)
{
    char buf1[TMP_BUF_SIZE] = {0};
    char buf2[TMP_BUF_SIZE] = {0};
    if (sprintf_s(buf1, TMP_BUF_SIZE - 1, "空气湿度        %d", data) != 0) {
        return -1;
    }
    g_labelTH_->SetText(static_cast<char *>(buf1));
    if (sprintf_s(buf2, TMP_BUF_SIZE - 1, "空气质量    优") != 0) {
        return -1;
    }
    g_labelTP_->SetText(static_cast<char *>(buf2));

    g_labelTH_->Invalidate();
    g_labelTP_->Invalidate();
    return 0;
}

void CenctrlAbilitySlice::InitServerSocket()
{
    stCallBackParam callParam;
    callParam.OnDisplayTP = this->DisplayTP;
    callParam.OnDisplayTH = this->DisplayTH;
    callParam.On = CtrlCallBack::CtrlOn;
    callParam.Off = CtrlCallBack::CtrlOff;
    callParam.OnConnect = CtrlCallBack::CtrlOnConnect;
    callParam.OnDisconnect = CtrlCallBack::CtrlOnDisconnect;
    RegisterSocketCallback(&callParam);
    CtrlCallBack::SetInstancePram(&itemListeners_);
}

void CenctrlAbilitySlice::OnStart(const Want& want)
{
    LOG(CENCTRL_DEBUG, "### cenctrl slice start ####\n");
    AbilitySlice::OnStart(want);
    rootview_ = RootView::GetWindowRootView();
    rootview_->SetPosition(0, 0, ROOTVIEW_W, ROOTVIEW_H);
    rootview_->SetStyle(STYLE_BACKGROUND_COLOR, Color::ColorTo32(Color::Black()));
    SetUpView();
    SetUIContent(rootview_);
    rootview_->Invalidate();
    InitServerSocket();
    SocketUdpCreateBC();
    SocketServiceCreate();
}

void CenctrlAbilitySlice::OnInactive()
{
    AbilitySlice::OnInactive();
}

void CenctrlAbilitySlice::OnActive(const Want& want)
{
    AbilitySlice::OnActive(want);
}

void CenctrlAbilitySlice::OnBackground()
{
    AbilitySlice::OnBackground();
}

void CenctrlAbilitySlice::OnStop()
{
    AbilitySlice::OnStop();
}
}