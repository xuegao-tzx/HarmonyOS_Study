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
import com.example.distschedule.bean.FamilyMemberResult;
import ohos.agp.components.*;
import ohos.agp.components.element.PixelMapElement;
import ohos.app.Context;
import ohos.global.resource.NotExistException;

import java.io.IOException;
import java.util.List;

public class MemberOfFamilyListProvider extends BaseItemProvider {
    private List<FamilyMemberResult> familyMemberList;
    private Context context;
    // 默认不显示右上角的删除图标
    private boolean isShowDelete = false;
    private boolean isShowNickname = false;

    private OnDeleteMemberListener onDeleteMemberListener;

    public MemberOfFamilyListProvider(Context context) {
        this.context = context;
    }

    public void setShowNickname(boolean ischecked) {
        isShowNickname = ischecked;
        if (familyMemberList != null) {
            notifyDataSetItemRangeChanged(0, familyMemberList.size());
        }
    }

    public void setOnDeleteMemberListener(OnDeleteMemberListener onDeletMemberListener) {
        this.onDeleteMemberListener = onDeletMemberListener;
    }

    @Override
    public int getCount() {
        return familyMemberList == null ? 0 : familyMemberList.size();
    }

    @Override
    public Object getItem(int position) {
        if (familyMemberList != null && position >= 0 && position < familyMemberList.size()) {
            return familyMemberList.get(position);
        }
        return null;
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public Component getComponent(int position, Component convertComponent, ComponentContainer componentContainer) {
        FamilyMemberResult familyMemberResult = familyMemberList.get(position);
        final Component cpt;
        if (convertComponent == null) {
            cpt = LayoutScatter.getInstance(context).parse(ResourceTable.Layout_item_memberoffamily, null, false);
        } else {
            cpt = convertComponent;
        }

        Image memberOfIcon = (Image) cpt.findComponentById(ResourceTable.Id_memberOfFamily_item_icon);
        Text memberOfName = (Text) cpt.findComponentById(ResourceTable.Id_memberOfFamily_item_name);
        Image iconDelete = (Image) cpt.findComponentById(ResourceTable.Id_memberOfFamilyitem_img_delete);

        if (isShowDelete) {
            iconDelete.setVisibility(Component.VISIBLE);
        } else {
            iconDelete.setVisibility(Component.HIDE);
        }


        try {
            if (familyMemberResult.getTag() == 1) {
                iconDelete.setVisibility(Component.HIDE);
                memberOfIcon.setImageElement(new PixelMapElement(context.getResourceManager().getResource(ResourceTable.Media_icon_invite_member)));
                memberOfName.setText("邀请");

            } else if (familyMemberResult.getTag() == 2) {
                iconDelete.setVisibility(Component.HIDE);
                memberOfIcon.setImageElement(new PixelMapElement(context.getResourceManager().getResource(ResourceTable.Media_icon_yichu_member)));
                memberOfName.setText("移除");
            } else {
                memberOfIcon.setImageElement(new PixelMapElement(context.getResourceManager().getResource(ResourceTable.Media_img_touxiang_papa)));
                if (isShowNickname) {
                    memberOfName.setText(familyMemberResult.getRole());
                } else {
                    memberOfName.setText(familyMemberResult.getUserName());
                }

            }
        } catch (IOException | NotExistException e) {
            e.printStackTrace();
        }


        if (onDeleteMemberListener != null) {
            iconDelete.setClickedListener(component -> onDeleteMemberListener.onDeleteClick(position));
        }

        return cpt;
    }

    public void setDate(List<FamilyMemberResult> list) {
        this.familyMemberList = list;
        if (list != null) {
            isShowDelete = false;
            notifyDataSetItemRangeChanged(0, list.size());
        }
    }

    public List<FamilyMemberResult> getData() {
        return familyMemberList;
    }

    public void setDeleteIcon() {
        isShowDelete = !isShowDelete;
        notifyDataChanged();
    }

    public interface OnDeleteMemberListener {
        void onDeleteClick(int position);
    }
}
