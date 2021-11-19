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
    CLOUD_COMMAND_SETBEEP = 0,
    CLOUD_COMMAND_SETTHRESHOLD,
    CLOUD_COMMAND_LAST,
}CLOUD_CommandType;


/**
 * @brief defines the parameter for set beep
 *
 */
typedef struct {
	int status;
}CommandParamSetBeep;

/**
 * @brief defines the parameter for set the MQ2 threshold
 *
 */
typedef struct {
	int threshold;
}CommandParamSetThreshold;

/**
 * @brief callback functions used to deal the command delivered by the Cloud Platform
 * @param command Indicates the command delivered by the Cloud Platform
 * @param cmdPara Indicates the parameter corresponding to the command
 * @return Returns 0 success while -1 failed
 */
int IotProfile_CommandCallback(int command, void *cmdParam);

/**
 * @brief use this function to encode the data to jsonString and send it to iot_cloud
 * @param beepStatus indicates the beep on/off
 * @param mq2 indicated the value detected by the MQ2
 * @param threshold indicate the warning threshhold
 * @return Returns 0 success while -1 failed
 *
 */
int IotProfile_Report(int beepStatus,float mq2, float threshold);



#endif /* IOT_PROFILE_H_ */
