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

package com.example.distschedule.slice;

import com.daimajia.swipe.SwipeLayout;
import com.example.distschedule.ResourceTable;
import com.example.distschedule.bean.RespondBase;
import com.example.distschedule.bean.ScheduleResult;
import com.example.distschedule.provider.SearchScheduleListProvider;
import com.example.distschedule.rx2.OKHttpUtilsRx2;
import com.example.distschedule.toast.XToastUtils;
import com.example.distschedule.utils.DateUtils;
import com.example.distschedule.utils.PreferencesUtil;
import com.socks.library.KLog;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.agp.components.*;
import ohos.multimodalinput.event.KeyEvent;
import org.apache.commons.lang3.StringUtils;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;

/**
 * 搜索日程
 *
 * @since 2021-08-28
 */
public class SearchScheduleAbilitySlice extends AbilitySlice implements Component.ClickedListener {

    private TextField textFiled;
    private DirectionalLayout emptyContainer;
    private ListContainer listContainer;
    private SearchScheduleListProvider searchScheduleListProvider;
    private SimpleDateFormat sdf;
    private Calendar cal;

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setUIContent(ResourceTable.Layout_ability_search_schedule);
        initComponents();
        sdf = new SimpleDateFormat("yyyy年MM月dd日");
        cal = Calendar.getInstance();
    }

    private void initComponents() {
        DependentLayout dlBack = (DependentLayout) findComponentById(ResourceTable.Id_search_dl_back);
        Image imgSearch = (Image) findComponentById(ResourceTable.Id_search_img_search);
        textFiled = (TextField) findComponentById(ResourceTable.Id_search_textfield_text);
        Image imgDelete = (Image) findComponentById(ResourceTable.Id_search_img_delete);
        listContainer = (ListContainer) findComponentById(ResourceTable.Id_search_listcontainer_schedule);
        emptyContainer = (DirectionalLayout) findComponentById(ResourceTable.Id_search_dl_empty);

        dlBack.setClickedListener(this);
        imgSearch.setClickedListener(this);
        imgDelete.setClickedListener(this);
        TableLayoutManager tableLayoutManager = new TableLayoutManager();
        tableLayoutManager.setColumnCount(1);
        listContainer.setLayoutManager(tableLayoutManager);
        searchScheduleListProvider = new SearchScheduleListProvider(this);
        listContainer.setItemProvider(searchScheduleListProvider);
        searchScheduleListProvider.setDeleteItemListener((position, id) -> {
            ((SwipeLayout) (listContainer.getComponentAt(position).findComponentById(ResourceTable.Id_swipe))).close(true);
            deleteSchedule(id);
        });

        textFiled.setKeyEventListener((component, keyEvent) -> {
            if (keyEvent.getKeyCode() == keyEvent.KEY_SEARCH || keyEvent.getKeyCode() == KeyEvent.KEY_ENTER) {
                searchSchedule();
            }
            return false;
        });
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
                            searchSchedule();
                        }
                    }
                });
    }

    @Override
    public void onActive() {
        super.onActive();
    }

    @Override
    public void onForeground(Intent intent) {
        super.onForeground(intent);
    }

    @Override
    public void onClick(Component component) {
        switch (component.getId()) {
            case ResourceTable.Id_search_dl_back:
                onBackPressed();
                break;
            case ResourceTable.Id_search_img_search:
                searchSchedule();
                break;
            case ResourceTable.Id_search_img_delete:
                textFiled.setText("");
                break;
            default:
                break;
        }
    }


    private void searchSchedule() {

        String searchName = textFiled.getText();
        if (StringUtils.isBlank(searchName)) {
            return;
        }

        OKHttpUtilsRx2.INSTANCE.getApi().searchSchedule(PreferencesUtil.getUserId(getContext()), searchName)
                .compose(OKHttpUtilsRx2.INSTANCE.ioMain())
                .subscribe(new OKHttpUtilsRx2.SubjectImpl<RespondBase<List<ScheduleResult>>>() {
                    @Override
                    public void onNext(RespondBase<List<ScheduleResult>> respond) {
                        super.onNext(respond);
                        KLog.i(getString(ResourceTable.String_log_tag), respond);
                        if (respond.isEmptyResult()) {
                            setEmptyState();
                        } else {
                            List<ScheduleResult> result = respond.getResult();
                            List<ScheduleResult> list = new ArrayList<>();
                            for (ScheduleResult scheduleResult : result) {
                                String remindDay = scheduleResult.getRemindDay();
                                // 如果不是重复 直接加到集合
                                if (StringUtils.isBlank(remindDay)) {
                                    list.add(scheduleResult);
                                    scheduleResult.setShowDate(scheduleResult.getStartTime());
                                } else {
                                    // 如果是重复  将该条数据 扩展为前后一个月的的总共九条数据
                                    String[] splitDay = remindDay.split(",");
                                    for (int n = 0; n < splitDay.length; n++) {
                                        if (splitDay[n].equals("1")) {
                                            for (int m = -4; m < 5; m++) {
                                                ScheduleResult newScheduleResult = new ScheduleResult();
                                                CopyBeanValue(newScheduleResult, scheduleResult);
                                                cal.setTime(DateUtils.getThisWeekMonday());
                                                cal.add(Calendar.DATE, 7 * m);
                                                newScheduleResult.setShowDate(sdf.format(cal.getTime()) + "  星期一");
                                                list.add(newScheduleResult);
                                            }
                                        } else if (splitDay[n].equals("2")) {
                                            for (int m = -4; m < 5; m++) {
                                                ScheduleResult newScheduleResult = new ScheduleResult();
                                                CopyBeanValue(newScheduleResult, scheduleResult);
                                                cal.setTime(DateUtils.getThisWeekTue());
                                                cal.add(Calendar.DATE, 7 * m);
                                                newScheduleResult.setShowDate(sdf.format(cal.getTime()) + "  星期二");
                                                list.add(newScheduleResult);
                                            }
                                        } else if (splitDay[n].equals("3")) {
                                            for (int m = -4; m < 5; m++) {
                                                ScheduleResult newScheduleResult = new ScheduleResult();
                                                CopyBeanValue(newScheduleResult, scheduleResult);
                                                cal.setTime(DateUtils.getThisWeekWed());
                                                cal.add(Calendar.DATE, 7 * m);
                                                newScheduleResult.setShowDate(sdf.format(cal.getTime()) + "  星期三");
                                                list.add(newScheduleResult);
                                            }
                                        } else if (splitDay[n].equals("4")) {
                                            for (int m = -4; m < 5; m++) {
                                                ScheduleResult newScheduleResult = new ScheduleResult();
                                                CopyBeanValue(newScheduleResult, scheduleResult);
                                                cal.setTime(DateUtils.getThisWeekThu());
                                                cal.add(Calendar.DATE, 7 * m);
                                                newScheduleResult.setShowDate(sdf.format(cal.getTime()) + "  星期四");
                                                list.add(newScheduleResult);
                                            }
                                        } else if (splitDay[n].equals("5")) {
                                            for (int m = -4; m < 5; m++) {
                                                ScheduleResult newScheduleResult = new ScheduleResult();
                                                CopyBeanValue(newScheduleResult, scheduleResult);
                                                cal.setTime(DateUtils.getThisWeekFri());
                                                cal.add(Calendar.DATE, 7 * m);
                                                newScheduleResult.setShowDate(sdf.format(cal.getTime()) + "  星期五");
                                                list.add(newScheduleResult);
                                            }
                                        } else if (splitDay[n].equals("6")) {
                                            for (int m = -4; m < 5; m++) {
                                                ScheduleResult newScheduleResult = new ScheduleResult();
                                                CopyBeanValue(newScheduleResult, scheduleResult);
                                                cal.setTime(DateUtils.getThisWeekSat());
                                                cal.add(Calendar.DATE, 7 * m);
                                                newScheduleResult.setShowDate(sdf.format(cal.getTime()) + "  星期六");
                                                list.add(newScheduleResult);
                                            }
                                        } else if (splitDay[n].equals("7")) {
                                            for (int m = -4; m < 5; m++) {
                                                ScheduleResult newScheduleResult = new ScheduleResult();
                                                CopyBeanValue(newScheduleResult, scheduleResult);
                                                cal.setTime(DateUtils.getThisWeekSat());
                                                cal.add(Calendar.DATE, 7 * m);
                                                newScheduleResult.setShowDate(sdf.format(cal.getTime()) + "  星期日");
                                                list.add(newScheduleResult);
                                            }
                                        }
                                    }


//                                    switch (remindDay) {
//                                        case "1":
//                                            for (int m = -4; m < 5; m++) {
//                                                ScheduleResult newScheduleResult = new ScheduleResult();
//                                                CopyBeanValue(newScheduleResult, scheduleResult);
//                                                cal.setTime(DateUtils.getThisWeekMonday());
//                                                cal.add(Calendar.DATE, 7 * m);
//                                                newScheduleResult.setShowDate(sdf.format(cal.getTime()) + "  星期一");
//                                                list.add(newScheduleResult);
//                                            }
//                                            break;
//                                        case "2":
//                                            for (int m = -4; m < 5; m++) {
//                                                ScheduleResult newScheduleResult = new ScheduleResult();
//                                                CopyBeanValue(newScheduleResult, scheduleResult);
//                                                cal.setTime(DateUtils.getThisWeekTue());
//                                                cal.add(Calendar.DATE, 7 * m);
//                                                scheduleResult.setShowDate(sdf.format(cal.getTime()) + "  星期二");
//                                                list.add(newScheduleResult);
//                                            }
//                                            break;
//                                        case "3":
//                                            for (int m = -4; m < 5; m++) {
//                                                ScheduleResult newScheduleResult = new ScheduleResult();
//                                                CopyBeanValue(newScheduleResult, scheduleResult);
//                                                cal.setTime(DateUtils.getThisWeekWed());
//                                                cal.add(Calendar.DATE, 7 * m);
//                                                scheduleResult.setShowDate(sdf.format(cal.getTime()) + "  星期三");
//                                                list.add(newScheduleResult);
//                                            }
//                                            break;
//                                        case "4":
//                                            for (int m = -4; m < 5; m++) {
//                                                ScheduleResult newScheduleResult = new ScheduleResult();
//                                                CopyBeanValue(newScheduleResult, scheduleResult);
//                                                cal.setTime(DateUtils.getThisWeekThu());
//                                                cal.add(Calendar.DATE, 7 * m);
//                                                scheduleResult.setShowDate(sdf.format(cal.getTime()) + "  星期四");
//                                                list.add(newScheduleResult);
//                                            }
//                                            break;
//                                        case "5":
//                                            for (int m = -4; m < 5; m++) {
//                                                ScheduleResult newScheduleResult = new ScheduleResult();
//                                                CopyBeanValue(newScheduleResult, scheduleResult);
//                                                cal.setTime(DateUtils.getThisWeekFri());
//                                                cal.add(Calendar.DATE, 7 * m);
//                                                scheduleResult.setShowDate(sdf.format(cal.getTime()) + "  星期五");
//                                                list.add(newScheduleResult);
//                                            }
//                                            break;
//                                        case "6":
//                                            for (int m = -4; m < 5; m++) {
//                                                ScheduleResult newScheduleResult = new ScheduleResult();
//                                                CopyBeanValue(newScheduleResult, scheduleResult);
//                                                cal.setTime(DateUtils.getThisWeekSat());
//                                                cal.add(Calendar.DATE, 7 * m);
//                                                scheduleResult.setShowDate(sdf.format(cal.getTime()) + "  星期六");
//                                                list.add(newScheduleResult);
//                                            }
//                                            break;
//                                        case "7":
//                                            for (int m = -4; m < 5; m++) {
//                                                ScheduleResult newScheduleResult = new ScheduleResult();
//                                                CopyBeanValue(newScheduleResult, scheduleResult);
//                                                cal.setTime(DateUtils.getThisWeekSat());
//                                                cal.add(Calendar.DATE, 7 * m);
//                                                scheduleResult.setShowDate(sdf.format(cal.getTime()) + "  星期日");
//                                                list.add(newScheduleResult);
//                                            }
//                                            break;
//
//                                        default:
//                                            break;
//                                    }
                                }
                            }
                            setScheduleListContainer();
                            searchScheduleListProvider.setData(list);
                        }
                    }
                });
    }

    private void CopyBeanValue(ScheduleResult newScheduleResult, ScheduleResult scheduleResult) {
        newScheduleResult.setId(scheduleResult.getId());
        newScheduleResult.setName(scheduleResult.getName());
        newScheduleResult.setUserIds(scheduleResult.getUserIds());
        newScheduleResult.setDeviceIds(scheduleResult.getDeviceIds());
        newScheduleResult.setDeviceConfig(scheduleResult.getDeviceConfig());
        newScheduleResult.setRemindDay(scheduleResult.getRemindDay());
        newScheduleResult.setCreatorId(scheduleResult.getCreatorId());
        newScheduleResult.setSceneId(scheduleResult.getSceneId());
        newScheduleResult.setStateDetail(scheduleResult.getStateDetail());
        newScheduleResult.setStartTime(scheduleResult.getStartTime());
        newScheduleResult.setEndTime(scheduleResult.getEndTime());
        newScheduleResult.setUpdateTime(scheduleResult.getUpdateTime());
        newScheduleResult.setCreateTime(scheduleResult.getCreateTime());
    }

    /**
     * 主容器设置为空状态页面
     */
    private void setEmptyState() {
        emptyContainer.setVisibility(Component.VISIBLE);
        listContainer.setVisibility(Component.HIDE);
    }

    /**
     * 主容器设置为设备列表显示页面
     */
    private void setScheduleListContainer() {
        emptyContainer.setVisibility(Component.HIDE);
        listContainer.setVisibility(Component.VISIBLE);
    }
}
