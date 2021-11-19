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
#include <string.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"

#include "lcd_st7789.h"
#include "lv_port_disp.h"
#include "lvgl.h"
#include "app_gsm.h"
#include "kv_store.h"
#include "app_gsm.h"
#include "iot_profile.h"
#include "app_button.h"

#define PHONE_NUMBER_LEN     11

#define DEF_SHADOW_COLOR lv_color_hex3(0xaaa)
#define DEF_RADIUS 4

#define APP_PHONE_BOOK_SELECT_ITEM   "phonebookselect"

extern lv_img_dsc_t background;

enum{
E_BG_NULL = 0,
E_BG_ANSWER,
E_BG_RING,
E_BG_DAIL,
E_BG_HANG_UP,
E_BG_SHOW_MESSAGE,
E_BG_SHOW_PHONE_BOOK,
};


enum{
E_MS_NULL = 0,
E_MS_NOTIFY,
E_MS_GETUP,
E_MS_SLEEP,
E_MS_HOMEWORK,
E_MS_DRINK_WATER,
E_MS_TAKE_MEDICINE,
};

enum{
E_BG_PB_FATHER = 0,
E_BG_PB_MOTHER,
};

static int g_lastSts = E_BG_NULL;

extern lv_img_dsc_t call_phone_icon;
extern lv_img_dsc_t answer_icon;
extern lv_img_dsc_t select_father;
extern lv_img_dsc_t select_mother;


lv_obj_t * g_dailPage;
lv_obj_t * g_ringPage;
lv_obj_t * g_answerPage;
lv_obj_t * g_showMessagePage;
lv_obj_t * g_bookPage;

LV_FONT_DECLARE(my_font_35); /* 申明字体 */
LV_FONT_DECLARE(lv_font_roboto_28); /* 申明字体 */
LV_FONT_DECLARE(lv_font_roboto_16); /* 申明字体 */

static lv_obj_t * g_labelMother;
static lv_obj_t * g_labelFather;


extern int GetMessageShowSts(void);

extern char *Iot_getShowMessage(void);

char g_showTime[6] = "";


static char * GetShowTime()
{
    int hour = 0;
    int minute = 0;
    int totalMinu = 0;
    int totalSeconds = 0;

    totalSeconds = Iot_getStartTime();

    printf("totalSeconds =%d\n", totalSeconds);

    if (totalSeconds > 0) {
        totalMinu = totalSeconds / 60;
        minute = totalMinu%60;
        hour = (totalMinu/60) % 60;
        printf("hour=%d,minute=%d\n", hour,minute);
        sprintf(g_showTime,"%u:%02d",  hour,minute);
        printf("showTimer=%s\n", g_showTime);

        int len = strlen(g_showTime);
        g_showTime[len] = '\0';

        printf("showTimer=%s\n", g_showTime);
    } else {
        return NULL;
    }

    return &g_showTime;
}

void gui_display_background(void)
{
    lv_obj_t *  img1 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img1, &background);
}

void AppShowMessagePage(void){

    int msgType = Iot_getMessageType();

    if (msgType < 1) {
        printf("showMessage is null\n");
        return;
    }

    lv_obj_t * scr = lv_scr_act(); /*获取当前活跃的屏幕对象 */
    g_showMessagePage = lv_page_create(scr,NULL);
    lv_obj_set_size(g_showMessagePage, 240, 240); /* 设置页面的大小 */
    lv_obj_align(g_showMessagePage, NULL, LV_ALIGN_CENTER,0,0); /* 与屏幕居中对齐 */


    static lv_style_t time_style;
    lv_obj_t * labelTime = lv_label_create(g_showMessagePage,NULL); 
    lv_obj_set_width(labelTime, lv_page_get_fit_width(g_showMessagePage)); /* 设置与showMessagePage页面的填充区域宽度相等 */

    lv_obj_set_pos(labelTime, 10, 10);
    lv_style_copy(&time_style, &lv_style_transp);
    time_style.text.font = &my_font_35;
    lv_label_set_style(labelTime, 0, &time_style);

    char *showTime = GetShowTime();
    if (showTime != NULL) {
        lv_label_set_static_text(labelTime,showTime);
        printf("showMessage =%s\n",showTime);
    }
    
    static lv_style_t message_style;
    lv_obj_t * labelMessage = lv_label_create(g_showMessagePage,NULL);
    lv_obj_set_width(labelMessage,lv_page_get_fit_width(g_showMessagePage)); /* 设置与showMessagePage页面的填充区域宽度相等 */

    lv_obj_set_pos(labelMessage, 0, 60);
    lv_style_copy(&message_style, &lv_style_transp);
    message_style.text.font = &lv_font_roboto_28;
    lv_label_set_style(labelMessage, 0, &message_style);

    if (msgType > 0 ) {

        char *message = NULL;

        if (msgType == E_MS_NOTIFY) {
            message = "message reminder";
        } else if (msgType == E_MS_GETUP){
            message = "get up";
        } else if (msgType == E_MS_SLEEP){
            message = "sleep";
        } else if (msgType == E_MS_HOMEWORK){
            message = "do homework";
        } else if (msgType == E_MS_DRINK_WATER){
            message = "drink water";
        } else if (msgType == E_MS_TAKE_MEDICINE){
            message = "take medicine";
        } else {
        }
        if (message != NULL) {
            lv_label_set_static_text(labelMessage,message);
        }

        printf("showMessage =%s\n",message);
    }
}

void AppPhoneBookPage(void){

    lv_obj_t * scr = lv_scr_act(); /* 获取当前活跃的屏幕对象 */

    printf("g_phoneBookSelect= %d\n",GsmGetPhoneBookItem());

    g_bookPage = lv_page_create(scr,NULL);
    lv_obj_set_size(g_bookPage, 240, 240); /* 设置页面的大小 */
    lv_obj_align(g_bookPage, NULL, LV_ALIGN_CENTER, 0, 0); /* 与屏幕居中对齐 */

    g_labelFather = lv_label_create(g_bookPage, NULL); 
    lv_obj_set_width(g_labelFather, lv_page_get_fit_width(g_bookPage)); /* 设置与bookPage页面的填充区域宽度相等 */
    lv_obj_align(g_labelFather,g_bookPage,LV_ALIGN_IN_TOP_LEFT,60,50); /* 设置与labelFather的对齐方式 */


    g_labelMother = lv_label_create(g_bookPage,NULL);
    lv_obj_set_width(g_labelMother, lv_page_get_fit_width(g_bookPage)); /* 设置与bookPage页面的填充区域宽度相等 */
    lv_obj_align(g_labelMother, g_bookPage,LV_ALIGN_IN_TOP_LEFT, 60, 120); /* 设置与labelMother的对齐方式 */
}

void AppSaveValue(char *key, char *value)
{
    if (key == NULL || value == NULL) {
        return;
    }
    int ret = UtilsSetValue(key, value);
    if (ret != 0) {
        printf("store phoneNumber failed! ret = %d\n",ret);
        return;
    }
}

void AppPhoneBookItemShow(int item, int isExist,int isChecked)
{
    static lv_style_t styleSelect;
    static lv_style_t styleUnmodified;


    printf("g_phoneBookSelect =%d\n",GsmGetPhoneBookItem());

    lv_style_copy(&styleUnmodified, &lv_style_transp);
    styleUnmodified.text.font = &lv_font_roboto_28;
    
    lv_style_copy(&styleSelect, &lv_style_pretty_color);
    styleSelect.text.font = &lv_font_roboto_28;

    if (item == E_BG_PB_FATHER) { /* father */
        if (isExist){
            if(isChecked) {
                lv_label_set_style(g_labelFather, LV_LABEL_STYLE_MAIN, &styleSelect);
                lv_label_set_body_draw(g_labelFather, true); /* 使能背景绘制 */
                lv_label_set_text(g_labelFather, LV_SYMBOL_CALL"father"LV_SYMBOL_OK);              
            } else {
                lv_label_set_style(g_labelFather, LV_LABEL_STYLE_MAIN, &styleUnmodified);
                lv_label_set_body_draw(g_labelFather,true); /* 使能背景绘制 */
                lv_label_set_text(g_labelFather, LV_SYMBOL_CALL"father");  
            }
        } else {
            lv_label_set_style(g_labelFather, LV_LABEL_STYLE_MAIN, &styleUnmodified);
            lv_label_set_body_draw(g_labelFather, true); /* 使能背景绘制 */
            lv_label_set_text(g_labelFather, "father"); 
        }
    } else if (item == E_BG_PB_MOTHER){ /* mother */
        if (isExist) {
            if (isChecked) {
                lv_label_set_style(g_labelMother, LV_LABEL_STYLE_MAIN, &styleSelect);
                lv_label_set_body_draw(g_labelMother, true); /* 使能背景绘制 */
                lv_label_set_text(g_labelMother, LV_SYMBOL_CALL"mother"LV_SYMBOL_OK);
            } else {
                lv_label_set_style(g_labelMother, LV_LABEL_STYLE_MAIN, &styleUnmodified);
                lv_label_set_body_draw(g_labelMother, true); /* 使能背景绘制 */
                lv_label_set_text(g_labelMother, LV_SYMBOL_CALL"mother");
            }
        } else {
            lv_label_set_style(g_labelMother, LV_LABEL_STYLE_MAIN, &styleUnmodified);
            lv_label_set_body_draw(g_labelMother, true); /* 使能背景绘制 */
            lv_label_set_text(g_labelMother, "mother");
        }
    }
}

void AppPhoneBookChangeItem(void){

    if (g_labelFather == NULL || g_bookPage == NULL || g_labelMother == NULL) {
        printf("AppPhoneBookChangeItem g_labelFather error\n");
        return;
    }

    char *phoneNumFather = IotProfile_GetFatherPhoneNumber();
    char *phoneNumMother = IotProfile_GetMohterPhoneNumber();
    int totalPhoneNumber = Iot_getTotalPhoneNumber();

    if (totalPhoneNumber == 0){
        AppPhoneBookItemShow(E_BG_PB_FATHER, false, false);
        AppPhoneBookItemShow(E_BG_PB_MOTHER, false, false);
    } else if(totalPhoneNumber == 1) {
        if(phoneNumFather != NULL) {
            AppPhoneBookItemShow(E_BG_PB_FATHER, true, true);
            AppPhoneBookItemShow(E_BG_PB_MOTHER, false, false);
            GsmSetPhoneBookItem(E_BG_PB_FATHER);
        }
        if(phoneNumMother != NULL) {
            AppPhoneBookItemShow(E_BG_PB_FATHER, false, false);
            AppPhoneBookItemShow(E_BG_PB_MOTHER, true, true);
            GsmSetPhoneBookItem(E_BG_PB_MOTHER);
        }
    } else if(totalPhoneNumber == 2 ) {
        if (GsmGetPhoneBookItem() == E_BG_PB_FATHER) {
            AppPhoneBookItemShow(E_BG_PB_FATHER, true, true);
            AppPhoneBookItemShow(E_BG_PB_MOTHER, true, false);
        } else if (GsmGetPhoneBookItem() == E_BG_PB_MOTHER) {
            AppPhoneBookItemShow(E_BG_PB_FATHER, true, false);
            AppPhoneBookItemShow(E_BG_PB_MOTHER, true, true);
        }
    }

    char saveValue[12] = "";
    snprintf(saveValue,sizeof(saveValue), "%d", GsmGetPhoneBookItem());    
    AppSaveValue(APP_PHONE_BOOK_SELECT_ITEM, saveValue);
}

void AppDailPage(void){

    lv_obj_t * scr = lv_scr_act(); /* 获取当前活跃的屏幕对象 */

    static lv_style_t style;
    lv_obj_t *dailingButton;
    char *phoneNum = NULL;

    if (GsmGetPhoneBookItem() == E_PHONEBOOK_FATHER) {
        phoneNum = IotProfile_GetFatherPhoneNumber();
    } else {
        phoneNum = IotProfile_GetMohterPhoneNumber();
    }

    printf("phoneNum = %s,len=%d,GsmGetPhoneBookItem=%d\n", phoneNum,strlen(phoneNum), GsmGetPhoneBookItem());
    if (strlen(phoneNum) != 11){
        return ;
    }

    g_dailPage = lv_page_create(scr,NULL);
    lv_obj_set_size(g_dailPage, 240, 240); /* 设置页面的大小 */
    lv_obj_align(g_dailPage, NULL, LV_ALIGN_CENTER, 0, 0); /* 与屏幕居中对齐 */


    static lv_style_t phone_style;
    lv_obj_t * labelPhoneNumber = lv_label_create(g_dailPage,NULL);
    lv_obj_set_width(labelPhoneNumber,lv_page_get_fit_width(g_dailPage)); /* 设置与dailPage页面的填充区域宽度相等 */

    
    lv_obj_align(labelPhoneNumber,g_dailPage,LV_ALIGN_IN_TOP_LEFT,30,40); /* 设置与dailPage的对齐方式 */

    lv_style_copy(&phone_style, &lv_style_transp);
    phone_style.text.font = &lv_font_roboto_28;
    lv_label_set_style(labelPhoneNumber, 0, &phone_style);

    if(strlen(phoneNum) == 11) {
        lv_label_set_text(labelPhoneNumber, phoneNum);
    }

    lv_obj_t * lDialing = lv_label_create(g_dailPage, NULL); /* 这里的父对象应该是dailPage */
    lv_obj_set_width(lDialing,lv_page_get_fit_width(g_dailPage)); /* 设置与dailPage页面的填充区域宽度相等 */
    lv_obj_align(lDialing,labelPhoneNumber,LV_ALIGN_OUT_BOTTOM_LEFT, 30, 10); /* 设置与labelPhoneNumber的对齐方式 */

    lv_style_copy(&phone_style, &lv_style_transp);
    phone_style.text.font = &lv_font_roboto_28;
    lv_label_set_style(lDialing, 0, &phone_style);

    lv_label_set_text(lDialing, "Dailing...");

    /* dailing button */
    dailingButton = lv_btn_create(g_dailPage, NULL);
    lv_obj_set_size(dailingButton, 200, 60);
    lv_obj_align(dailingButton, g_dailPage,LV_ALIGN_CENTER,0, 60); /* 设置与dailPage的对齐方式 */
    lv_label_set_style(dailingButton, 0, &style);

    lv_obj_t *  img1 = lv_img_create(dailingButton, NULL);
    lv_obj_align(img1,dailingButton,LV_ALIGN_CENTER, 0, 0); /* 设置与dailingButton的对齐方式 */
    lv_img_set_src(img1, &call_phone_icon);

}

void AppRingPage(void){

    lv_obj_t * scr = lv_scr_act(); /* 获取当前活跃的屏幕对象 */

    g_ringPage = lv_page_create(scr,NULL);
    lv_obj_set_size(g_ringPage, 240, 240); /* 设置页面的大小 */
    lv_obj_align(g_ringPage, NULL, LV_ALIGN_CENTER, 0, 0); /* 与屏幕居中对齐 */

    
    static lv_style_t phone_style;
    lv_obj_t * labelPhoneName = lv_label_create(g_ringPage, NULL);
    lv_obj_set_width(labelPhoneName,lv_page_get_fit_width(g_ringPage)); /* 设置与ringPage页面的填充区域宽度相等 */

    lv_obj_align(labelPhoneName,g_ringPage,LV_ALIGN_IN_TOP_LEFT , 20, 40); /* 设置与labelPhoneName的对齐方式 */
 
    lv_style_copy(&phone_style, &lv_style_transp);
    phone_style.text.font = &lv_font_roboto_28;
    lv_label_set_style(labelPhoneName, 0, &phone_style);

    lv_label_set_text(labelPhoneName, "Incoming call");


    lv_obj_t *  imgAccept = lv_img_create(g_ringPage, NULL);
    lv_obj_align(imgAccept,g_ringPage,LV_ALIGN_IN_RIGHT_MID, 30, 50); /* 设置与ringPage的对齐方式 */
    lv_img_set_src(imgAccept, &answer_icon);

    lv_obj_t * labelAccept = lv_label_create(g_ringPage,NULL);
    lv_label_set_text(labelAccept, "Accept");
    lv_obj_align(labelAccept,imgAccept,LV_ALIGN_OUT_BOTTOM_MID, 0, 5); /* 设置与imgAccept的对齐方式 */


    lv_obj_t *  imgRejection = lv_img_create(g_ringPage, NULL);
    lv_obj_align(imgRejection,g_ringPage,LV_ALIGN_IN_LEFT_MID , 30, 50); /* 设置与ringPage的对齐方式 */
    lv_img_set_src(imgRejection, &call_phone_icon);

    lv_obj_t * labelReject = lv_label_create(g_ringPage, NULL);
    lv_label_set_text(labelReject, "Reject");
    lv_obj_align(labelReject,imgRejection,LV_ALIGN_OUT_BOTTOM_MID, 0, 5); /* 设置与imgRejection的对齐方式 */

}

void AppAnswerPage(void){

    lv_obj_t * scr = lv_scr_act(); /* 获取当前活跃的屏幕对象 */

    g_answerPage = lv_page_create(scr, NULL);
    lv_obj_set_size(g_answerPage, 240, 240); /* 设置页面的大小 */
    lv_obj_align(g_answerPage, NULL, LV_ALIGN_CENTER, 0, 0); /* 与屏幕居中对齐 */

    static lv_style_t phone_style;
    lv_obj_t * labelPhoneName = lv_label_create(g_answerPage, NULL); 
    lv_obj_set_width(labelPhoneName,lv_page_get_fit_width(g_answerPage)); /* 设置与answerPage页面的填充区域宽度相等 */
    lv_obj_align(labelPhoneName,g_answerPage,LV_ALIGN_IN_TOP_LEFT, 30, 40); /* 设置与answerPage的对齐方式 */

    lv_style_copy(&phone_style, &lv_style_transp);
    phone_style.text.font = &lv_font_roboto_28;
    lv_label_set_style(labelPhoneName, 0, &phone_style);
    lv_label_set_text(labelPhoneName, "incoming call");

    lv_obj_t *  imgRejection = lv_img_create(g_answerPage, NULL);
    lv_obj_align(imgRejection,labelPhoneName,LV_ALIGN_OUT_BOTTOM_MID, 30, 80); /* 设置与labelPhoneName的对齐方式 */
    lv_img_set_src(imgRejection, &call_phone_icon);
}

static void AppCleanAllPage(void)
{
    if (g_answerPage != NULL) {
        lv_obj_move_background(g_answerPage);
        g_answerPage = NULL;
        osDelay(25);
    }
    if (g_ringPage != NULL) {
        lv_obj_move_background(g_ringPage);
        g_ringPage = NULL;
        osDelay(25);
    }
    if (g_dailPage != NULL) {
        lv_obj_move_background(g_dailPage);
        g_dailPage = NULL;
        printf("lv_obj_move_background g_dailPage \n");
        osDelay(25);
    }

    if (g_showMessagePage != NULL) {
        lv_obj_move_background(g_showMessagePage);
        g_showMessagePage = NULL;
        printf("lv_obj_move_background g_showMessagePage \n");
        osDelay(25);
    }

    if (g_bookPage != NULL) {
        printf("lv_obj_move_background g_bookPage \n");
        lv_obj_move_background(g_bookPage);
        g_bookPage = NULL;
        osDelay(25);
    }
}

void app_pageChange(void)
{
    int showMessageSts = GetMessageShowSts();
    static int lastShortKeySts = 0;

    printf("app_pageChange:g_isRing=%d,connectSts=%d,DailSts=%d,showMessageSts=%d,g_isHungUp=%d\n",\
           GsmGetRingSts(), GsmGetConnectSts(), GsmGetDailSts(), showMessageSts, GsmGetHungUpSts());

    printf("g_isKey2LongPressed = %d,g_lastSts=%d\n",button_GetKey2LongPressSts(),g_lastSts);

    if (button_GetKey2LongPressSts()) {
        if (g_lastSts != E_BG_SHOW_PHONE_BOOK) {
            printf("g_lastSts =%d\n",g_lastSts);

            if (g_lastSts != E_BG_NULL) {
                 AppCleanAllPage();
            }
            g_lastSts = E_BG_SHOW_PHONE_BOOK;
            AppPhoneBookPage();
            AppPhoneBookChangeItem();
        } else {
            if (lastShortKeySts != button_GetKey2Sts()) {
                AppPhoneBookChangeItem();
                lastShortKeySts = button_GetKey2Sts();
            }
        }
        osDelay(25);
    } else if(GsmGetDailSts()) {
        if (g_lastSts != E_BG_DAIL) {
            printf("DailSts g_lastSts =%d\n",g_lastSts);

            if (g_lastSts != E_BG_NULL) {
                AppCleanAllPage();
            }
            g_lastSts = E_BG_DAIL;
            AppDailPage();
        }
        osDelay(25);
    } else if (GsmGetRingSts()) {
        if (g_lastSts != E_BG_RING) {
            printf("g_isRing g_lastSts =%d\n",g_lastSts);

            if (g_lastSts != E_BG_NULL) {
                AppCleanAllPage();
            }
            g_lastSts = E_BG_RING;
            AppRingPage();
        }
        osDelay(25);
    } else if ( (GsmGetRingSts()&&(GsmGetConnectSts() == true)) || (GsmGetConnectSts() == true)) {
         printf("connetStatus = %d,g_lastSts=%d\n", GsmGetConnectSts(), g_lastSts);
        if (g_lastSts != E_BG_ANSWER) {
            printf("g_isAnswering g_lastSts =%d\n", g_lastSts);
            if (g_lastSts != E_BG_NULL) {
                AppCleanAllPage();
            }
            g_lastSts = E_BG_ANSWER;
            AppAnswerPage();
        }
        osDelay(25);
    } else if (GsmGetHungUpSts() == true) {
        if(g_lastSts != E_BG_HANG_UP){
            printf("g_isHungUp g_lastSts =%d\n", g_lastSts);
            if(g_lastSts != E_BG_NULL){
                AppCleanAllPage();
            }
            g_lastSts = E_BG_NULL;
            osDelay(25);
            GsmSetHungUpSts(false);
        }
    } else if (showMessageSts) {
        if(g_lastSts != E_BG_SHOW_MESSAGE){
            printf("g_lastSts =%d\n", g_lastSts);

            if(g_lastSts != E_BG_NULL){
                AppCleanAllPage();
            }
            g_lastSts = E_BG_SHOW_MESSAGE;
            AppShowMessagePage();
        }
    } else {
        AppCleanAllPage();
        g_lastSts = E_BG_NULL; 
        osDelay(20);
    }
}
