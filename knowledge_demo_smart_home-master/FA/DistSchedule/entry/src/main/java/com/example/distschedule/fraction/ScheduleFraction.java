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

package com.example.distschedule.fraction;

import com.daimajia.swipe.SwipeLayout;
import com.daimajia.swipe.util.Attributes;
import com.example.distschedule.ResourceTable;
import com.example.distschedule.ScheduleManageAbility;
import com.example.distschedule.bean.RespondBase;
import com.example.distschedule.bean.ScheduleResult;
import com.example.distschedule.provider.ScheduleListProvider;
import com.example.distschedule.rx2.OKHttpUtilsRx2;
import com.example.distschedule.toast.XToastUtils;
import com.example.distschedule.utils.PreferencesUtil;
import com.example.distschedule.utils.Util;
import ohos.aafwk.content.Intent;
import ohos.aafwk.content.Operation;
import ohos.agp.components.*;
import ohos.agp.utils.LayoutAlignment;
import ohos.agp.window.dialog.CommonDialog;
import org.apache.commons.lang3.StringUtils;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.*;

/**
 * 日程页面
 *
 * @since 2021-08-28
 */
public class ScheduleFraction extends FractionBase {
    private final String[] weeks = {"星期", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六", "星期天"};
    private Component containerLayout;
    private DirectionalLayout scheduleContainer;
    private ListContainer listContainer;
    private DirectionalLayout emptyContainer;
    private ScheduleListProvider listContainerProvider;

    @Override
    protected Component onComponentAttached(LayoutScatter scatter, ComponentContainer container, Intent intent) {
        containerLayout = LayoutScatter.getInstance(getApplicationContext()).parse(ResourceTable.Layout_fraction_schedule, null, false);
        initComponents();
        return containerLayout;
    }

    private void initComponents() {
        scheduleContainer = (DirectionalLayout) containerLayout.findComponentById(ResourceTable.Id_schedule_dl_container);
        emptyContainer = (DirectionalLayout) containerLayout.findComponentById(ResourceTable.Id_schedule_dl_empty);
        containerLayout.findComponentById(ResourceTable.Id_top_img_add).setClickedListener(this::toAddSchedule);
        containerLayout.findComponentById(ResourceTable.Id_top_img_search).setClickedListener(this::toSearchSchedule);
        initScheduleList();
    }

    private void toSearchSchedule(Component component) {
        Intent intent = new Intent();
        Operation operation =
                new Intent.OperationBuilder()
                        .withBundleName(getBundleName())
                        .withAbilityName(ScheduleManageAbility.class.getName())
                        .withAction("action.searchSchedule")
                        .build();
        intent.setOperation(operation);
        startAbility(intent, 0);
    }

    private void updateScheduleList(List<ScheduleResult> scheduleList) {
        if (listContainerProvider != null) {
            listContainerProvider.setData(scheduleList);
        }
    }

    private void initScheduleList() {
        listContainer = (ListContainer) containerLayout.findComponentById(ResourceTable.Id_schedule_ls_list);
        listContainerProvider = new ScheduleListProvider(this);
        listContainerProvider.setMode(Attributes.Mode.Single);

        listContainer.setItemProvider(listContainerProvider);
        listContainer.setItemClickedListener((listContainer, component, position, l) -> {
            // 正常状态点击则滑出删除按钮，否则弹出错误提示框
            ScheduleResult schedule = listContainerProvider.getList().get(position);
            if (schedule.isError()) {
                createErrorDialog(schedule.getStateDetail());
            } else {
                ((SwipeLayout) (listContainer.getComponentAt(position))).open(true);
            }
        });
        listContainerProvider.setDeleteItemListener((position, id) -> {
            deleteSchedule(id);
            ((SwipeLayout) (listContainer.getComponentAt(position))).close(true);
        });
    }

    private void createErrorDialog(String stateDetail) {
        CommonDialog commonDialog = new CommonDialog(getContext());
        commonDialog.setSize(Util.vp2px(getContext(), 336), Util.vp2px(getContext(), 246));
        commonDialog.setAlignment(LayoutAlignment.BOTTOM);
        commonDialog.setOffset(Util.vp2px(getContext(), 0), Util.vp2px(getContext(), 20));
        commonDialog.setCornerRadius(Util.vp2px(getContext(), 8));
        Component container = LayoutScatter.getInstance(getContext()).parse(ResourceTable.Layout_dialog_schedule_state,
                null, false);
        ((Text) container.findComponentById(ResourceTable.Id_scheduleDialog_text_detail)).setText(stateDetail);
        container.findComponentById(ResourceTable.Id_scheduleDialog_btn_comfirm).setClickedListener(component -> commonDialog.destroy());
        commonDialog.setContentCustomComponent(container);
        commonDialog.setAutoClosable(true);
        commonDialog.show();
    }

    private void deleteSchedule(String scheduleId) {
        OKHttpUtilsRx2.INSTANCE.getApi().deleteScheduleById(scheduleId, PreferencesUtil.getUserId(getContext()))
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase>() {
                    @Override
                    public void onNext(RespondBase respond) {
                        super.onNext(respond);
                        if (respond.isSuccess()) {
                            XToastUtils.toast("删除成功");
                            getTodaySchedule();
                        }
                    }
                });
    }

    private void toAddSchedule(Component component) {
        Intent intent = new Intent();
        Operation operation =
                new Intent.OperationBuilder()
                        .withBundleName(getBundleName())
                        .withAbilityName(ScheduleManageAbility.class.getName())
                        .build();
        intent.setOperation(operation);
        startAbility(intent, 0);
    }

    private void getTodaySchedule() {
        SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd");
        Date now = new Date();
        String today = format.format(now);
        String startTime = today + " 00:00:00";
        String endTime = today + " 23:59:59";
        setDateTitle(today + " " + weeks[getWeekOfDate(now)]);
        requestScheduleByDate(startTime, endTime, getWeekOfDate(now));
    }

    private void requestScheduleByDate(String startTime, String endTime, int dayOfWeek) {
        OKHttpUtilsRx2.INSTANCE.getApi().getSchedule(PreferencesUtil.getUserId(getContext()), startTime, endTime)
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase<List<ScheduleResult>>>() {
                    @Override
                    public void onNext(RespondBase<List<ScheduleResult>> respond) {
                        super.onNext(respond);
                        if (respond.isEmptyResult()) {
                            setEmptyState();
                        } else {

                            List<ScheduleResult> list = new ArrayList<>();
                            for (ScheduleResult item : respond.getResult()) {
                                if (StringUtils.contains(item.getRemindDay(), String.valueOf(dayOfWeek))) {
                                    // 显示周期性是当天的日程
                                    list.add(item);
                                } else if (StringUtils.isEmpty(item.getRemindDay())) {
                                    // 显示当天的日程
                                    list.add(item);
                                }
                            }
                            if (list.isEmpty()) {
                                setEmptyState();
                            } else {
                                setScheduleListContainer(list);
                            }
                        }
                    }
                });
    }

    /**
     * 主容器设置为设备列表显示页面
     */
    private void setScheduleListContainer(List<ScheduleResult> scheduleList) {
        emptyContainer.setVisibility(Component.HIDE);
        scheduleContainer.setVisibility(Component.VISIBLE);
        updateScheduleList(scheduleList);
    }

    private void setDateTitle(String text) {
        ((Text) containerLayout.findComponentById(ResourceTable.Id_schedule_text_dateTitle)).setText(text);
        containerLayout.findComponentById(ResourceTable.Id_schedule_dl_currentDate).setClickedListener(component -> createDatePick());
    }

    private void createDatePick() {
        CommonDialog commonDialog = new CommonDialog(getContext());
        Component dialog = LayoutScatter.getInstance(getContext())
                .parse(ResourceTable.Layout_dialog_date_pick, null, true);
        commonDialog.setSize(AttrHelper.vp2px(320, getContext()), AttrHelper.vp2px(264, getContext()));
        commonDialog.setContentCustomComponent(dialog);
        dialog.findComponentById(ResourceTable.Id_dialog_btn_confirm).setClickedListener(component -> {
            DatePicker datePicker = (DatePicker) dialog.findComponentById(ResourceTable.Id_dialog_dp_picker);
            String dateStr = String.format(Locale.getDefault(), "%s-%s-%s", datePicker.getYear(), datePicker.getMonth(), datePicker.getDayOfMonth());
            String startTime = dateStr + " 00:00:00";
            String endTime = dateStr + " 23:59:59";
            SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd");
            try {
                Date date = format.parse(dateStr);
                setDateTitle(dateStr + " " + weeks[getWeekOfDate(date)]);
                requestScheduleByDate(startTime, endTime, getWeekOfDate(date));
                commonDialog.destroy();
            } catch (ParseException e) {
                e.printStackTrace();
            }
        });
        dialog.findComponentById(ResourceTable.Id_dialog_btn_cancel).setClickedListener(component -> commonDialog.destroy());
        commonDialog.show();
    }

    private int getWeekOfDate(Date date) {
        Calendar calendar = Calendar.getInstance();
        calendar.setTime(date);
        int dayOfWeek = calendar.get(Calendar.DAY_OF_WEEK) - 1;
        if (dayOfWeek == 0) {
            dayOfWeek = 7;
        }
        return dayOfWeek;
    }

    /**
     * 主容器设置为空状态页面
     */
    private void setEmptyState() {
        emptyContainer.setVisibility(Component.VISIBLE);
        scheduleContainer.setVisibility(Component.HIDE);
    }

    @Override
    protected void onActive() {
        getTodaySchedule();
        super.onActive();
    }
}