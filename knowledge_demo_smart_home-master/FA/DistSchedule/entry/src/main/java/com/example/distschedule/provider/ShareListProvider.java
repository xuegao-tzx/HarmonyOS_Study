/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

package com.example.distschedule.provider;

import com.example.distschedule.ResourceTable;
import com.example.distschedule.bean.UserShareResult;
import ohos.aafwk.ability.AbilitySlice;
import ohos.agp.components.*;
import org.apache.commons.lang3.StringUtils;

import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class ShareListProvider extends BaseItemProvider {
    private List<UserShareResult> list;
    private AbilitySlice slice;
    private Set<String> selectMemberId;
    private boolean isAllSelected = true;

    public ShareListProvider(AbilitySlice slice) {
        this.slice = slice;
    }

    @Override
    public int getCount() {
        return list == null ? 0 : list.size();
    }

    @Override
    public Object getItem(int position) {
        if (list != null && position >= 0 && position < list.size()) {
            return list.get(position);
        }
        return null;
    }

    @Override
    public long getItemId(int position) {
        //可添加具体处理逻辑
        return position;
    }

    @Override
    public Component getComponent(int position, Component convertComponent, ComponentContainer componentContainer) {
        final Component cpt;
        if (convertComponent == null) {
            cpt = LayoutScatter.getInstance(slice).parse(ResourceTable.Layout_item_member, null, false);
        } else {
            cpt = convertComponent;
        }
        Image checkbox = (Image) cpt.findComponentById(ResourceTable.Id_member_img_checkbox);
        ((Text) cpt.findComponentById(ResourceTable.Id_member_text_name)).setText(list.get(position).getUserName());
        cpt.findComponentById(ResourceTable.Id_member_dl_item).setClickedListener(component -> {
            String id = list.get(position).getUserId();
            Boolean isCheck = selectMemberId.contains(id);
            if (isCheck == null || !isCheck) {
                selectMemberId.add(id);
                checkbox.setPixelMap(ResourceTable.Media_icon_unselected);
            } else {
                selectMemberId.remove(id);
                checkbox.setPixelMap(ResourceTable.Media_icon_selected);
            }
        });

        String id = list.get(position).getUserId();
        if (isAllSelected) {
            selectMemberId.remove(id);
            checkbox.setPixelMap(ResourceTable.Media_icon_selected);
        } else {
            selectMemberId.add(id);
            checkbox.setPixelMap(ResourceTable.Media_icon_unselected);
        }
        return cpt;
    }

    public void setData(List<UserShareResult> list) {
        this.list = list;
        selectMemberId = new HashSet<>(list.size());
        if (list != null) {
            notifyDataSetItemRangeChanged(0, list.size());
        }
    }

    public String getMemberId() {
        if (selectMemberId == null) {
            return "";
        }
        return StringUtils.join(selectMemberId.toArray(), ",");
    }

    public void setIsAllSelected(boolean isAllSelected) {
        this.isAllSelected = isAllSelected;
        if (list != null) {
            notifyDataSetItemRangeChanged(0, list.size());
        }
    }
}
