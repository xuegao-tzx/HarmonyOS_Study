/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
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

package com.example.distschedule.utils;

import com.alibaba.fastjson.JSONArray;
import com.alibaba.fastjson.JSONObject;
import com.example.distschedule.bean.Command;
import com.example.distschedule.database.DeviceData;
import org.apache.commons.lang3.StringUtils;

import java.util.ArrayList;
import java.util.List;

public class CommandUtil {
    private CommandUtil() {

    }

    /**
     * 获取台灯命令
     *
     * @param isOn      是否打开
     * @param dutyCycle 亮度
     * @return 返回命令值
     */
    public static Command getLampStatusCommand(boolean isOn, int dutyCycle) {
        JSONObject params = new JSONObject();
        if (isOn) {
            params.put("LampStatus", "ON");
            params.put("DutyCycle", dutyCycle);
        } else {
            params.put("LampStatus", "OFF");
            params.put("DutyCycle", 1);
        }
        return new Command("SetLampStatus",
                "SmartLamp", JSONObject.toJSONString(params));
    }

    public static String parseDeviceCommand(String jsonStr) {
        List<String> resultList = new ArrayList<>();
        JSONArray deviceConfig = JSONArray.parseArray(jsonStr);
        if (deviceConfig != null) {
            deviceConfig.forEach(object -> {
                JSONObject jsonObject = (JSONObject) object;
                JSONObject command = jsonObject.getJSONObject("command");
                String deviceId = jsonObject.getString("deviceId");

                try {
                    String productID = deviceId.split("_")[0];
                    String commandString = DeviceData.INSTANCE.getDeviceData().get(productID).getCommandString(command);
                    resultList.add(commandString);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            });
        }
        return StringUtils.join(resultList, ";");

    }

    public static String parseFanCommand(JSONObject command) {
        String fanStatus = command.getString("FanStatus");
        if ("OFF".equals(fanStatus)) {
            return "关闭";
        } else {
            int speedLevel = command.getIntValue("SpeedLevel");
            return "风速：" + speedLevel + "档";
        }
    }

    public static String parseCurtainCommand(JSONObject command) {
        String lampStatus = command.getString("LampStatus");
        if ("OPEN".equals(lampStatus)) {
            return "打开窗帘";
        } else {
            return "关闭窗帘";
        }
    }

    public static String parseWateringCommand(JSONObject command) {
        String status = command.getString("MotorStatus");
        if ("OFF".equals(status)) {
            return "关闭护花使者";
        } else {
            int capacity = command.getIntValue("Capacity");
            return "开始浇水:" + capacity + "ml";
        }
    }

    public static String parseRobotCleanCommand(JSONObject command) {
        String lampStatus = command.getString("GoTOClean");
        if ("OFF".equals(lampStatus)) {
            return "关闭扫地机器人";
        } else {
            return "打开扫地机器人";
        }
    }

    public static String parseLampCommand(JSONObject command) {
        String lampStatus = command.getString("LampStatus");
        if ("OFF".equals(lampStatus)) {
            return "关灯";
        } else {
            int dutyCycle = command.getIntValue("DutyCycle");
            return "开灯 亮度：" + dutyCycle;
        }
    }

    public static Command getLampDutyCycleCommand(int dutyCycle) {
        JSONObject params = new JSONObject();
        if (dutyCycle >= 100) {
            params.put("DutyCycle", 99);
        } else if (dutyCycle <= 0) {
            params.put("DutyCycle", 1);
        } else {
            params.put("DutyCycle", dutyCycle);
        }
        return new Command("SetLampDutyCycle", "SmartLamp", JSONObject.toJSONString(params));
    }

    /**
     * 获取扫地机器人命令
     *
     * @param isOn 是否打开
     * @return Command
     */

    public static Command getRobotCleanStatusCommand(boolean isOn) {
        JSONObject params = new JSONObject();
        if (isOn) {
            params.put("GoTOClean", "ON");
        } else {
            params.put("GoTOClean", "OFF");
        }
        return new Command("CleanCmd", "CleanData", JSONObject.toJSONString(params));
    }

    /**
     * 获取窗帘命令
     *
     * @param isOn 是否打开
     * @return Command
     */

    public static Command getCurtainStatusCommand(boolean isOn) {
        JSONObject params = new JSONObject();
        if (isOn) {
            params.put("CurtainStatus", "OPEN");
        } else {
            params.put("CurtainStatus", "CLOSE");
        }
        return new Command("SetCurtainStatus", "SmartCurtain", JSONObject.toJSONString(params));
    }

    /**
     * 获取养花机命令
     *
     * @param isOn 是否打开
     * @return Command
     */
    public static Command getWateringStatusCommand(boolean isOn, int waterVolume) {
        JSONObject params = new JSONObject();
        if (isOn) {
            params.put("MotorStatus", "ON");
            params.put("Capacity", waterVolume);
        } else {
            params.put("MotorStatus", "OFF");
            params.put("Capacity", 0);
        }
        return new Command("SetMotor", "SmartWatering", JSONObject.toJSONString(params));
    }

    /**
     * 获取烟感报警器命令
     *
     * @param isOn 是否打开
     * @return Command
     */
    public static Command getSmokeStatusCommand(boolean isOn) {
        JSONObject params = new JSONObject();
        if (isOn) {
            params.put("BeepStatus", "ON");
        } else {
            params.put("BeepStatus", "OFF");
        }
        return new Command("SetBeep", "SmartSmokeSensation", JSONObject.toJSONString(params));
    }

    /**
     * 获取烟感报警器阈值
     *
     * @param threshold 报警阈值
     * @return Command
     */
    public static Command getSmokeThresholdCommand(int threshold) {
        JSONObject params = new JSONObject();

        params.put("Threshold", threshold);
        return new Command("SetThreshold", "SmartSmokeSensation", JSONObject.toJSONString(params));
    }

    /**
     * 获取可燃气体报警器命令
     *
     * @param isOn 是否打开
     * @return Command
     */
    public static Command getFlammableGasStatusCommand(boolean isOn) {
        JSONObject params = new JSONObject();
        if (isOn) {
            params.put("BeepStatus", "ON");
        } else {
            params.put("BeepStatus", "OFF");
        }
        return new Command("SetBeep", "GasDetection", JSONObject.toJSONString(params));
    }

    /**
     * 获取可燃气体报警器阈值
     *
     * @param threshold 报警阈值
     * @return Command
     */
    public static Command getFlammableGasThresholdCommand(int threshold) {
        JSONObject params = new JSONObject();

        params.put("Threshold", threshold);
        return new Command("SetThreshold", "GasDetection", JSONObject.toJSONString(params));
    }

    /**
     * 获取甲醛报警器命令
     *
     * @param isOn 是否打开
     * @return Command
     */
    public static Command getFormaldehydeStatusCommand(boolean isOn) {
        JSONObject params = new JSONObject();
        if (isOn) {
            params.put("BeepStatus", "ON");
        } else {
            params.put("BeepStatus", "OFF");
        }
        return new Command("SetBeep", "TVOCDetection", JSONObject.toJSONString(params));
    }

    /**
     * 获取甲醛报警器阈值
     *
     * @param threshold 报警阈值
     * @return Command
     */
    public static Command getFormaldehydeThresholdCommand(int threshold) {
        JSONObject params = new JSONObject();

        params.put("Threshold", threshold);
        return new Command("SetThreshold", "TVOCDetection", JSONObject.toJSONString(params));
    }

    /**
     * 获取电风扇开关命令
     *
     * @param isOn 是否打开
     * @return Command
     */
    public static Command getFanStatusCommand(boolean isOn, int speed) {
        JSONObject params = new JSONObject();
        if (isOn) {
            params.put("FanStatus", "ON");
            params.put("SpeedLevel", speed);
        } else {
            params.put("FanStatus", "OFF");
            params.put("SpeedLevel", 0);
        }
        return new Command("SetFanStatus", "SmartFan", JSONObject.toJSONString(params));
    }

    public static Command getFanSpeedCommand(int speed) {
        JSONObject params = new JSONObject();
        params.put("SpeedLevel", speed);
        return new Command("SetSpeedLevel", "SmartFan", JSONObject.toJSONString(params));
    }

    /**
     * 添加电话号码：
     *
     * @param name   联系人名称
     * @param number 联系人号码
     * @return Command
     */
    public static Command addWatchPhoneNumber(String name, String number) {
        JSONObject params = new JSONObject();
        params.put("name", name);
        params.put("PhoneNumber", number);
        return new Command("AddPhoneNumber", "SmartWatch", JSONObject.toJSONString(params));
    }

    /**
     * 删除电话号码
     *
     * @param name   联系人名称
     * @param number 联系人号码
     * @return Command
     */
    public static Command deleteWatchPhoneNumber(String name, String number) {
        JSONObject params = new JSONObject();
        params.put("name", name);
        params.put("PhoneNumber", number);
        return new Command("DeletePhoneNumber", "SmartWatch", JSONObject.toJSONString(params));
    }


    /**
     * 获取手表GPS开关命令
     *
     * @param isOn 是否打开
     * @return Command
     */
    public static Command getWatchGPSStatusCommand(boolean isOn) {
        JSONObject params = new JSONObject();
        if (isOn) {
            params.put("GpsPowerStatus", "ON");

        } else {
            params.put("GpsPowerStatus", "OFF");
        }
        return new Command("GpsControl", "SmartWatch", JSONObject.toJSONString(params));
    }

}
