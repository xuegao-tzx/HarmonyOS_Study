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

import com.daimajia.swipe.SimpleSwipeListener;
import com.daimajia.swipe.SwipeLayout;
import com.daimajia.swipe.adapters.BaseSwipeAdapter;
import com.example.distschedule.ResourceTable;
import com.example.distschedule.bean.ScheduleResult;
import com.example.distschedule.utils.CommandUtil;
import ohos.agp.colors.RgbColor;
import ohos.agp.components.Component;
import ohos.agp.components.ComponentContainer;
import ohos.agp.components.LayoutScatter;
import ohos.agp.components.Text;
import ohos.agp.components.element.ShapeElement;
import ohos.app.Context;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;

public class SearchScheduleListProvider extends BaseSwipeAdapter {
    private Context context;
    private OnDeleteItemListener onDeleteItemListener;
    private List<ScheduleResult> list;

    public SearchScheduleListProvider(Context context, List<ScheduleResult> list) {
        super();
        this.context = context;
        this.list = list;
    }

    public SearchScheduleListProvider(Context context) {
        super();
        this.context = context;
    }

    public void setDeleteItemListener(OnDeleteItemListener onDeleteItemListener) {
        this.onDeleteItemListener = onDeleteItemListener;
    }

    @Override
    public int getSwipeLayoutResourceId(int position) {
        return ResourceTable.Id_swipe;
    }

    @Override
    public Component generateView(int position, ComponentContainer parent) {
        Component v = LayoutScatter.getInstance(context).parse(ResourceTable.Layout_search_schedule_card, null, false);
        SwipeLayout swipeLayout = (SwipeLayout) v.findComponentById(getSwipeLayoutResourceId(position));
        ScheduleResult item = list.get(position);
        Text textDate = (Text) v.findComponentById(ResourceTable.Id_schedule_text_date);
        textDate.setText(item.getShowDate());
        ((Text) v.findComponentById(ResourceTable.Id_schedule_text_name)).setText(item.getName());
        ((Text) v.findComponentById(ResourceTable.Id_schedule_text_duration)).setText(item.getDurationString());
        ((Text) v.findComponentById(ResourceTable.Id_scheduleCard_text_command)).setText(CommandUtil.parseDeviceCommand(item.getDeviceConfig()));
        if (isScheduleOngoing(item.getStartTime(), item.getEndTime())) {
            ShapeElement sideElement = new ShapeElement(context, ResourceTable.Graphic_background_device_card_side_ongoing);
            ShapeElement background = new ShapeElement();
            background.setRgbColor(new RgbColor(10, 89, 247, 20));
            v.findComponentById(ResourceTable.Id_scheduleCard_dl_side).setBackground(sideElement);
            v.findComponentById(ResourceTable.Id_scheduleCard_dl_card).setBackground(background);
        }
        Component right = v.findComponentById(ResourceTable.Id_card_dl_bottom);
        swipeLayout.addDrag(SwipeLayout.DragEdge.Right, right);
        swipeLayout.addSwipeListener(new SimpleSwipeListener() {
            @Override
            public void onOpen(SwipeLayout layout) {
            }
        });
        swipeLayout.setOnDoubleClickListener((layout, surface) -> {

        });
        v.findComponentById(ResourceTable.Id_card_dl_delete).setClickedListener(component -> {
            onDeleteItemListener.onDeleteClick(position, list.get(position).getId());
        });
        return v;
    }

    private boolean isScheduleOngoing(String startTime, String endTime) {
        SimpleDateFormat format = new SimpleDateFormat("HH:mm:ss");
        try {
            Date current = format.parse(format.format(new Date()));
            Date start = format.parse(startTime.split(" ")[1]);
            Date end = format.parse(endTime.split(" ")[1]);
            return current.after(start) && current.before(end);
        } catch (ParseException e) {
            e.printStackTrace();
            return false;
        }

    }

    @Override
    public void fillValues(int position, Component convertView) {

    }

    public void setData(List<ScheduleResult> list) {
        this.list = list;
        if (list != null && !list.isEmpty()) {
            notifyDataSetItemRangeChanged(0, list.size());
        }
    }

    @Override
    public int getCount() {
        return list == null ? 0 : list.size();
    }

    @Override
    public Object getItem(int position) {
        if (list != null && position >= 0 && position < list.size()) {
            return list.get(position);
        } else {
            return list;
        }
    }

    @Override
    public long getItemId(int i) {
        return i;
    }

    public interface OnDeleteItemListener {
        void onDeleteClick(int position, String id);
    }
}
