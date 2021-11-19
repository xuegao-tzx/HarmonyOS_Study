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

#ifndef IOT_PROFILE_H_
#define IOT_PROFILE_H_


/**
 * @brief Enumeration all the command issued by the Cloud Platform
 */
typedef enum {
    CLOUD_COMMAND_SETSHEDULE = 0,
    CLOUD_COMMAND_SET_PHONE_NUMBER,
    CLOUD_COMMAND_DAIL_NUMBER,
    CLOUD_COMMAND_GET_LOCATION,
}CLOUD_CommandType;

/**
 * @brief declare the command for set the shedule for the led
 *
 */
typedef enum {
	EN_MONDAY = 0,
	EN_TUESDAY,
	EN_WEDNESDAY,
	EN_THURSDAY,
	EN_FRIDAY,
	EN_SATURDAY,
	EN_SUNDAY,
	EN_DAYALL,
}DayInWeek;

#define	CN_MAX_SCHID		40
#define	CN_MAX_WEEKDAY		7
#define CN_MAX_HOUR        24
#define CN_MAX_MINUTE      60
#define CN_MAX_LASTTIME    (CN_MAX_HOUR * CN_MAX_MINUTE)


/**
 * @brief defines the parameter for set main led command
 *
 */
typedef struct {
	int msgType;
}CommandParamShowMessage;


typedef struct {
	int day[CN_MAX_WEEKDAY];         // defines as DayInWeek
	int num;		// the day array size
	int startHour;   // less than 24
	int startMinute; // less than 60
	int duration;    // less than 1440

	char scheduleID[CN_MAX_SCHID];		// the schedule id
	char option;						// the schedule optio, 'A' add, 'U'update, 'D'delete
    CommandParamShowMessage showMsgInfo;
}CommandParamSetShedule;

/**
 * @brief defines the parameter for set the auxiliary led duty cycle
 *
 */
typedef struct {
	int dutyCycle;
}CommandParamSetDutyCycle;

/**
 * @brief callback functions used to deal the command delivered by the Cloud Platform
 * @param command Indicates the command delivered by the Cloud Platform
 * @param cmdPara Indicates the parameter corresponding to the command
 * @return Returns 0 success while -1 failed
 */
int IotProfile_CommandCallback(int command, void *cmdParam);

/**
 * @brief use this function to encode the data to jsonString and send it to iot_cloud
 * @param mainLedStatus indicates the main led on/off
 * @param pwmLedDutyCycle indicates the duty cycle of the pwd led on the sc1 board
 * @param lightIntensity indicates the intensity of sc1 board
 * @return Returns 0 success while -1 failed
 *
 */
int IotProfile_Report(const char *utcDateTime, const char *logitude , const char  *latitude, const char *satellitesinview);

void IotProfile_init(void);
int IotPhoneNumber_get(char *name, char *phoneNumber, int size);
int Iot_getStartTime(void);
char *IotProfile_GetFatherPhoneNumber(void);
char *IotProfile_GetMohterPhoneNumber(void);
int Iot_getTotalPhoneNumber(void);

#endif /* IOT_PROFILE_H_ */
