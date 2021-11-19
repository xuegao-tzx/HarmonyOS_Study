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
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "securec.h"
#include "ohos_types.h"
#include "cmsis_os2.h"
#include "common.h"
#include "keypad.h"
#include "netcfg.h"
#include "oled.h"
#include "pwm.h"
#include "sensor.h"
#include "led.h"
#include "defines.h"
#include "winder.h"
#include "network_config_service.h"

typedef struct {
    uint8 power_off;
    uint8 speed_level;
    uint8 speed_mode;
    uint8 shake_scope;

    bool timer_flag;
    uint8 timer_hour;
    uint8 timer_mins;
    uint32 timer_count;

    osTimerId_t timerID;
}WinderInfo;

static WinderInfo g_winder;
static bool g_netstatus = false;
static const char *g_mode_str[] = {"sleep", "natural", "auto"};
static const char *g_scop_str[] = {"forbid", "30", "60", "90", "120"};

static void ShowPowerStatus(void)
{
    char power_sta[BUF_SIZE] = {0};
    if (sprintf_s(power_sta, BUF_SIZE, "power   %s", g_winder.power_off == 1 ? "OFF" : "ON") < 0) {
        WINERR("sprintf_s failed! \n");
        return;
    }
    OledShowStr("                           ", strlen("                       "), POWER_XPOS, POWER_YPOS, 1);
    OledShowStr(power_sta, strlen(power_sta), POWER_XPOS, POWER_YPOS, 1);
}

static void ShowNetStatus(void)
{
    char netsta_buf[BUF_SIZE] = {0};
    if (sprintf_s(netsta_buf, BUF_SIZE, "net  %s", g_netstatus ? "connectted" : "disconnect") < 0) {
        WINERR("sprintf_s failed! \n");
        return;
    }
    OledShowStr("                           ", strlen("                       "), NETSTA_XPOS, NETSTA_YPOS, 1);
    OledShowStr(netsta_buf, strlen(netsta_buf), NETSTA_XPOS, NETSTA_YPOS, 1);
}

static void ShowSpeedMode(void)
{
    char mode_buf[BUF_SIZE] = {0};
    if (sprintf_s(mode_buf, BUF_SIZE, "mode    %s", g_mode_str[g_winder.speed_mode - 1]) < 0) {
        WINERR("sprintf_s failed! \n");
        return;
    }
    OledShowStr("                           ", strlen("                       "), MODE_XPOS, MODE_YPOS, 1);
    OledShowStr(mode_buf, strlen(mode_buf), MODE_XPOS, MODE_YPOS, 1);
}

static void ShowSpeedLevel(void)
{
    char speed_buf[BUF_SIZE] = {0};
    if (sprintf_s(speed_buf, BUF_SIZE, "wind    %d", g_winder.speed_level) < 0) {
        WINERR("sprintf_s failed! \n");
        return;
    }
    OledShowStr("                           ", strlen("                       "), SPEED_XPOS, SPEED_YPOS, 1);
    OledShowStr(speed_buf, strlen(speed_buf), SPEED_XPOS, SPEED_YPOS, 1);
}

static void ShowShakeScope(void)
{
    char scope_buf[BUF_SIZE] = {0};
    if (sprintf_s(scope_buf, BUF_SIZE, "shake   %s", g_scop_str[g_winder.shake_scope]) < 0) {
        WINERR("sprintf_s failed! \n");
        return;
    }
    OledShowStr("                           ", strlen("                       "), SCOPE_XPOS, SCOPE_YPOS, 1);
    OledShowStr(scope_buf, strlen(scope_buf), SCOPE_XPOS, SCOPE_YPOS, 1);
}

static void ShowPowerOffTimer(void)
{
    char timer_buf[BUF_SIZE] = {0};
    if (g_winder.timer_flag) {
        if (sprintf_s(timer_buf, BUF_SIZE, "Timer   %02dH %02dM",
            g_winder.timer_hour, g_winder.timer_mins) < 0) {
            WINERR("sprintf_s failed! \n");
            return;
        }
    } else {
        if (sprintf_s(timer_buf, BUF_SIZE, "Timer   OFF") < 0) {
            WINERR("sprintf_s failed! \n");
            return;
        }
    }
    OledShowStr("                           ", strlen("                       "), TIMER_XPOS, TIMER_YPOS, 1);
    OledShowStr(timer_buf, strlen(timer_buf), TIMER_XPOS, TIMER_YPOS, 1);
}

static void WinderShowInfo(void)
{
    ShowPowerStatus();
    ShowNetStatus();
    ShowSpeedMode();
    ShowSpeedLevel();
    ShowShakeScope();
    ShowPowerOffTimer();
}

static void UpdateSpeedLevel(void)
{
    ShowSpeedLevel();
}

static void UpdateSpeedMode(void)
{
    ShowSpeedMode();
}

static void WinderDealSpeed(int value1, int value2)
{
    (void)value2;
    bool updateMode = false;
    WINDBG("g_winder.speed_level = %d, value=%d \n", g_winder.speed_level, value1);
    if (g_winder.speed_level != value1) {
        g_winder.speed_level = value1;
        PwmSetLevel(g_winder.speed_level);
        if (g_winder.speed_level < PWM_LEVEL_3 && g_winder.speed_mode != WINDER_MODE_SLEEP) {
            g_winder.speed_mode = WINDER_MODE_SLEEP;
            updateMode = true;
        } else if (g_winder.speed_level >= PWM_LEVEL_3 && g_winder.speed_mode != WINDER_MODE_NATURAL) {
            g_winder.speed_mode = WINDER_MODE_NATURAL;
            updateMode = true;
        }

        ShowSpeedLevel();
        if (updateMode) {
            UpdateSpeedMode();
        }
    }
}

static void WinderDealMode(int value1, int value2)
{
    (void)value2;
    bool updateSpeed = false;
    WINDBG("g_winder.speed_mode : %d, value : %d \n", g_winder.speed_mode, value1);
    if (g_winder.speed_mode != value1) {
        g_winder.speed_mode = value1;
        if (g_winder.speed_mode == WINDER_MODE_SLEEP && g_winder.speed_level > PWM_LEVEL_2) {
            g_winder.speed_level = PWM_LEVEL_1;
            updateSpeed = true;
        } else if (g_winder.speed_mode == WINDER_MODE_NATURAL && g_winder.speed_level < PWM_LEVEL_3) {
            g_winder.speed_level = PWM_LEVEL_3;
            updateSpeed = true;
        }
        PwmSetLevel(g_winder.speed_level);
        ShowSpeedMode();
        if (updateSpeed) {
            UpdateSpeedLevel();
        }
    }
}

static void WinderDealTimer(int value1, int value2)
{
    if (g_winder.timer_hour != value1 || g_winder.timer_mins != value2) {
        g_winder.timer_hour = value1;
        g_winder.timer_mins = value2;

        if (g_winder.timer_hour == 0 && g_winder.timer_mins == 0) {
            g_winder.timer_flag = 0;
        } else {
            g_winder.timer_flag = 1;
            g_winder.timer_count = 0;
        }

        ShowPowerOffTimer();
    }
}

static void WinderDealScope(int value1, int value2)
{
    (void)value1;
    if (g_winder.shake_scope != value2) {
        g_winder.shake_scope = value2;
        ShowShakeScope();
    }
}

static void WinderDealPoweroff(int value1, int value2)
{
    (void)value2;
    if (value1 == 0) {
        g_winder.power_off = 1;
        PwmSetLevel(PWM_LEVEL_0);
    } else {
        g_winder.power_off = 0;
        PwmSetLevel(g_winder.speed_level);
    }
    ShowPowerStatus();
}

static int my_atoi(char *str, int length)
{
    char buf[MESSAGE_LEN] = {0};
    if (length >= MESSAGE_LEN) {
        WINERR("invliad length! \n");
        return -1;
    }

    if (strncpy_s(buf, MESSAGE_LEN, str, length) != 0) {
        WINERR("strncpy_s failed! \n");
        return 0;
    }

    return atoi(buf);
}

#define ARRAYSIZE(a)    (sizeof((a)) / sizeof((a)[0]))

typedef union {
    char msg[MESSAGE_LEN + 1];
    struct {
        char type[MSG_VAL_LEN];
        char value1[MSG_VAL_LEN];
        char value2[MSG_VAL_LEN];
    } msg_segment;
} MsgInfo;

typedef struct {
    int type;
    void (*ProcessFunc)(int value1, int value2);
} MsgProcess;

static MsgInfo g_msgInfo;
static MsgProcess g_msgProcess[] = {
    {MESSAGE_POWER_OFF, WinderDealPoweroff},    // 风扇开关指令
    {MESSAGE_POWER_OFF_S, WinderDealPoweroff},
    {MESSAGE_SPEED_SET, WinderDealSpeed},       // 设置风扇档位指令
    {MESSAGE_SPEED_SET_S, WinderDealSpeed},
    {MESSAGE_SPEED_MODE, WinderDealMode},       // 风扇模式指令
    {MESSAGE_SPEED_MODE_S, WinderDealMode},
    {MESSAGE_SCOP_SET, WinderDealScope},         // 风扇摇头角度指令
    {MESSAGE_SCOP_SET_S, WinderDealScope},
    {MESSAGET_TIMER_SET, WinderDealTimer},       // 设置定时时间指令
    {MESSAGET_TIMER_SET_S, WinderDealTimer},
};

static void WinderProcessAppMessage(const char *data, int data_len)
{
    if (data_len != MESSAGE_LEN) {
        WINERR("data len invalid! \n");
        return;
    }

    if (strncpy_s(g_msgInfo.msg, MESSAGE_LEN + 1, data, data_len) != 0) {
        WINERR("strncpy_s failed! \n");
        return;
    }

    for (uint8 i = 0; i < ARRAYSIZE(g_msgProcess); i++) {
        if (g_msgProcess[i].type == my_atoi(g_msgInfo.msg_segment.type, MSG_VAL_LEN)) {
            g_msgProcess[i].ProcessFunc(my_atoi(g_msgInfo.msg_segment.value1, MSG_VAL_LEN),
                                        my_atoi(g_msgInfo.msg_segment.value2, MSG_VAL_LEN));
        }
    }
}

static int WinderSpeedChange(int position, char *message_buf, int length)
{
    uint8 value;
    if (message_buf == NULL || length < MESSAGE_LEN) {
        WINERR("NULL POINT!\n");
        return -1;
    }

    if (position == SPEED_PLUS) {
        if (g_winder.speed_level == PWM_LEVEL_5)
            return 1;
        value = g_winder.speed_level + 1;
    } else {
        if (g_winder.speed_level == PWM_LEVEL_1)
            return 1;
        value = g_winder.speed_level - 1;
    }

    *message_buf++ = ASCII_1;
    *message_buf++ = MESSAGE_SPEED_SET + ASCII_0;
    *message_buf++ = ASCII_0;
    *message_buf++ = value + ASCII_0;
    *message_buf++ = ASCII_0;
    *message_buf++ = ASCII_0;

    return 0;
}

static int WinderSpeedModeChange(char *message_buf, int length)
{
    uint8 value;
    WINDBG("\n");
    if (message_buf == NULL || length < MESSAGE_LEN) {
        WINERR("NULL POINT!\n");
        return -1;
    }
    value = g_winder.speed_mode + 1;
    if (value >= WINDER_MODE_NBR) {
        value = WINDER_MODE_SLEEP;
    }
    *message_buf++ = ASCII_1;
    *message_buf++ = MESSAGE_SPEED_MODE + ASCII_0;
    *message_buf++ = ASCII_0;
    *message_buf++ = value + ASCII_0;
    *message_buf++ = ASCII_0;
    *message_buf++ = ASCII_0;

    return 0;
}

static int WinderSharkScopeChange(char *message_buf, int length)
{
    uint8 value;
    WINDBG("\n");
    if (message_buf == NULL || length < MESSAGE_LEN) {
        WINERR("NULL POINT!\n");
        return -1;
    }
    value = g_winder.shake_scope + 1;
    if (value > WINDER_MAX_SCOP) {
        value = 0;
    }
    *message_buf++ = ASCII_1;
    *message_buf++ = MESSAGE_SPEED_MODE + ASCII_0;
    *message_buf++ = ASCII_0;
    *message_buf++ = ASCII_1;
    *message_buf++ = ASCII_0;
    *message_buf++ = value + ASCII_0;

    return 0;
}

static int WinderTimerChnage(char *message_buf, int length)
{
    WINDBG("\n");
    if (message_buf == NULL || length < MESSAGE_LEN) {
        WINERR("NULL POINT!\n");
        return -1;
    }

    if (g_winder.timer_flag == false) {
        g_winder.timer_flag = true;
        g_winder.timer_mins = TIMER_HALF_HOUR;
        g_winder.timer_count = 0;
    } else {
        g_winder.timer_mins += TIMER_HALF_HOUR;
        if (g_winder.timer_mins >= TIMER_ONE_HOUR) {
            if (++g_winder.timer_hour > TIMER_MAX_HOURS) {
                g_winder.timer_hour = 0;
                g_winder.timer_mins = 0;
                g_winder.timer_flag = false;
                g_winder.timer_count = 0;
                return 0;
            }
            g_winder.timer_mins -= TIMER_ONE_HOUR;
        }
    }

    g_winder.timer_count = 0;
    *message_buf++ = ASCII_1;
    *message_buf++ = MESSAGET_TIMER_SET + ASCII_0;
    *message_buf++ = (g_winder.timer_hour / MULTIPLE) + ASCII_0;
    *message_buf++ = (g_winder.timer_hour % MULTIPLE) + ASCII_0;
    *message_buf++ = (g_winder.timer_mins / MULTIPLE) + ASCII_0;
    *message_buf++ = (g_winder.timer_mins % MULTIPLE) + ASCII_0;

    return 0;
}

static int WinderSpeedPlus(char *buff, int length)
{
    WINDBG("\n");
    return WinderSpeedChange(SPEED_PLUS, buff, length);
}

static int WinderSpeedMinus(char *buff, int length)
{
    WINDBG("\n");
    return WinderSpeedChange(SPEED_MINUS, buff, length);
}

typedef struct {
    int keycode;
    int (*KeyFunc)(char *buff, int length);
}KeyProcess;

static KeyProcess g_keyProcess[] = {
    {KEY_PAD_PLUS, WinderSpeedPlus},        // 风速加按键
    {KEY_PAD_MINUS, WinderSpeedMinus},      // 风速减按键
    {KEY_PAD_MODE, WinderSpeedModeChange},  // 风模式按键
    {KEY_PAD_SCOP, WinderSharkScopeChange}, // 摇头角度按键
    {KEY_PAD_MENU, WinderTimerChnage},      // 定时按键
};

static int WinderKeyEventHandle(KeyEvent *event)
{
    char message_buf[MESSAGE_LEN + 1] = {0};
    bool message_ready = false;

    WINDBG("keycode = %d \n", event->code);
    for (uint8 i = 0; i < ARRAYSIZE(g_keyProcess); i++) {
        if (event->code == g_keyProcess[i].keycode) {
            if (g_keyProcess[i].KeyFunc(message_buf, MESSAGE_LEN) == 0) {
                message_ready = true;
            }
        }
    }

    if (message_ready) {    // 判断到有效按键
        WinderProcessAppMessage((const char *)message_buf, MESSAGE_LEN); // 消息处理函数
        WINDBG("message_buf : %s \n\n", message_buf);
        SendRawData((char *)message_buf);   // 将消息发到FA
    }

    return 0;
}

static int WinderNetEventHandler(NET_EVENT_TYPE event, void *data)
{
    switch (event) {
        case NET_EVENT_CONNECTTED:                                      // 网络连接成功
            g_netstatus = true;
            ShowNetStatus();                                            // 显示网络已连接
            break;
        case NET_EVENT_RECV_DATA:                                       // 接收到网络信息(FA发送的消息)
            WinderProcessAppMessage((const char *)data, strlen(data));  // 处理对应的信息
            break;
        default:
            break;
    }
    return 0;
}

static void WinderTimerHandler(void *arg)
{
    (void)arg;

    if (g_winder.timer_flag) {
        g_winder.timer_count++;
        if (g_winder.timer_count >= (uint32)((g_winder.timer_hour * (TIMER_60_SECOND * TIMER_60_SECOND)) +
            g_winder.timer_mins * TIMER_60_SECOND)) {
            g_winder.power_off = 1;
        }
    } else {
        g_winder.timer_count = 0;
    }
}

static void WinderStartTimer(void)
{
    g_winder.timerID = osTimerNew(&WinderTimerHandler, osTimerPeriodic, NULL, NULL);
    osTimerStart(g_winder.timerID, TICKS_NUMBER);
}

static int WinderGetTempLevel(int temp)
{
    int temp_array[] = {23, 26, 28, 30, 33};      // each winder level's temp in the array
    int i;

    for (i = 0; i < ARRAYSIZE(temp_array); i++) {
        if (temp < temp_array[i]) {
            return i;
        }
    }

    return 0;
}

static void WinderInit(void)
{
    DeviceInit();       // 硬件相关初始化（比如I2C的初始化等）
    PwmInit();          // PWM初始化，风力大小通过PWM来控制
    SensorInit();       // 温度感应器的初始化

    if (OledInit() != 0) {      // OLED初始化
        WINERR("OledInit failed! \n");
        while (1) {
        }
    }
}

static void WinderLoop(void)
{
    while (1) {
        if (g_winder.power_off) {
            PwmSetLevel(PWM_LEVEL_0);
            // do power off ?
        }
        if (g_winder.speed_mode == WINDER_MODE_AUTO) {      // 检测到时自动模式(即根据环境温度自动设置风力)
            int speed;
            int temp = SensorGetValue();                    // 获取当前温度
            speed = WinderGetTempLevel(temp);               // 根据温度获取对应的风力档位
            if (speed != g_winder.speed_level) {            // 判断通过温度获取的风力档位与当前是否一致
                g_winder.speed_level = speed;               // 将通过温度获取的风力档位设置为当前档位
                PwmSetLevel(g_winder.speed_level);          // 设置风扇档位
                ShowSpeedLevel();                           // 显示当前风力档位
            }
        }
        osDelay(WINDER_LOOP_DELAY);
    }
}

static void *WinderTask(const char *arg)
{
    (void)arg;
    WINDBG("WinderTask Enter! \n");
    WinderInit();
    (void)memset_s(&g_winder, sizeof(g_winder), 0x00, sizeof(g_winder));
    g_winder.speed_mode = WINDER_MODE_SLEEP;    // 设置出事风为睡眠风

    if (KeyRegisterInit(WinderKeyEventHandle) != 0) {       // 注册按键回调
        WINERR("KeyRegisterInit failed!!! \n");
    }
    WinderStartTimer();                     // 开启定时器
    NetCfgRegister(WinderNetEventHandler);  // 进入配网状态并注册网络监听事件
    WinderShowInfo();       // 显示风扇初始状态

    WinderLoop();
}

void WinderDemoEntry(void)
{
    osThreadAttr_t attr;
    attr.name = "WinderTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = WINDER_TASK_STACK_SIZE;
    attr.priority = WINDER_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)WinderTask, NULL, &attr) == NULL) {
        WINERR("Falied to create WinderTask!\n");
    }
}

SYS_RUN(WinderDemoEntry);
