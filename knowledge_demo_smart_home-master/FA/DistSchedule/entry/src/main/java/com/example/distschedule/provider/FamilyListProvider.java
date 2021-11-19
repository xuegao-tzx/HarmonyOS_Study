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
import com.example.distschedule.bean.FamilyResult;
import ohos.agp.components.*;
import ohos.app.Context;

import java.util.List;

public class FamilyListProvider extends BaseItemProvider {
    private List<FamilyResult> familyList;
    private Context context;

    /**
     * show
     *
     * @param familyList context
     * @param context    message
     */
    public FamilyListProvider(List<FamilyResult> familyList, Context context) {
        this.familyList = familyList;
        this.context = context;
    }

    @Override
    public int getCount() {
        return familyList == null ? 0 : familyList.size();
    }

    @Override
    public Object getItem(int position) {
        if (familyList != null && position >= 0 && position < familyList.size()) {
            return familyList.get(position);
        } else {
            return familyList;
        }
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public Component getComponent(int position, Component convertComponent, ComponentContainer componentContainer) {
        final Component cpt;
        if (convertComponent == null) {
            cpt = LayoutScatter.getInstance(context).parse(ResourceTable.Layout_item_family, null, false);
        } else {
            cpt = convertComponent;
        }
        FamilyResult resultList = familyList.get(position);
        Text text = (Text) cpt.findComponentById(ResourceTable.Id_family_name_text);
        text.setText(resultList.getName());
        return cpt;
    }

    public List<FamilyResult> getFamilyList() {
        return familyList;
    }
}
