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


#ifndef __KEY_PAD_H__
#define __KEY_PAD_H__

#define KEY_TASK_STACK_SIZE     (1024*4)
#define KEY_TASK_PRIO           (30)
#define KEY_DELAY_TIME          (10)

typedef enum {
    KEY_CODE_0,     // menu         确认键
    KEY_CODE_1,     // speed++      速度加
    KEY_CODE_2,     // speed--      速度减
    KEY_CODE_3,     // mode         风速的模式（普通模式，睡眠模式）
    KEY_CODE_4,     // scope        转的角度

    KEY_CODE_N
}KEY_CODE;

#define KEY_DOWN    1
#define KEY_UP      0

#define KEY_PAD_MENU    KEY_CODE_3
#define KEY_PAD_PLUS    KEY_CODE_1
#define KEY_PAD_MINUS   KEY_CODE_2
#define KEY_PAD_MODE    KEY_CODE_0
#define KEY_PAD_SCOP    KEY_CODE_4

#define KEY_PAD_NONE    0xFF

typedef struct {
    KEY_CODE code;
    int     value;
}KeyEvent;

typedef int (*keypad_event_call)(KeyEvent *event);

int KeyRegisterInit(keypad_event_call gcall);

#endif  /* __KEY_PAD_H__ */
