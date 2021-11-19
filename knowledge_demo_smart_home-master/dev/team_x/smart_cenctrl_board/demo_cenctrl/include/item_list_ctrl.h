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

#ifndef OHOS_ITEM_LIST_CONCTRL_H
#define OHOS_ITEM_LIST_CONCTRL_H

#include <iostream>
#include <components/ui_label_button.h>
#include <components/ui_label.h>
#include <components/ui_image_view.h>
#include <components/ui_view_group.h>

#include "socket_service.h"
#include "ui_config.h"

namespace OHOS {
using OnClickFunc = std::function<bool(UIView& view, const Event& event)>;
using OnItemCtrlFunc = std::function<bool()>;

class ItemCtrl : public UIView::OnClickListener {
public:
    ItemCtrl();
    virtual ~ItemCtrl();

    virtual void Setup(UIViewGroup* view, UILabel* label, UIImageView* image, std::string strUnSelect,
                        std::string strSelect, std::string devName);
    bool OnClick(UIView& view, const ClickEvent& event) override;
    void OnConnect(char* devName, int status);
    void OnDisconnect(char* devName, int status);
    void On(char* devName, int status);
    void Off(char* devName, int status);

    std::string devName_;
private:
    UILabel* label_;
    UIImageView* image_;
    UIViewGroup* view_;
    std::string strSelect_;
    std::string strUnSelect_;
    bool currState_{ false };
};
}   // namespace OHOS
#endif
