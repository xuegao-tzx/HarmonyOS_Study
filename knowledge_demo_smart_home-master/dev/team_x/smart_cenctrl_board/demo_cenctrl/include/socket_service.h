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

#ifndef _SOCKET_SERVICE_H_
#define _SOCKET_SERVICE_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif  /* __cplusplus */

#define MAX_CONNECT 10 // how many pending connections queue will hold
#define SOCKET_BUF_SIZE 1024
#define TICK_COUNT_MAX 3
#define TMP_BUF_MAX 64

#define EXIT_ERR    (-1)
#define SOCKET_OK   (0)
#define SOCKET_ERR  (-1)

typedef struct _stContrlParam {
    unsigned int service_id;   // 0:not connected
    unsigned int state;        // Device switch status
    unsigned int enable;       // socket connect status
    char devName[TMP_BUF_MAX]; // Name of the connected device
    unsigned int countTick;    // Number of times that no heartbeat response is received
}stContrlParam;

enum SOCKET_CONCTRL {
    SOCKET_DISABLE = 0,
    SOCKET_ENABLE
};

enum LIGHT_CONCTRL {
    LIGHT_OFF = 0,
    LIGHT_ON,
};

/*
* @brief: display callback func.
* @params: data -- the value to display, devStatus -- the device status to display (enable/disable)
* @return: 0 -- success, -1 --failed
*
* @since 1.0
* @version 1.0
*/
typedef int (*funCallback) (int data, int devStatus);

/*
* @brief: operation callback func.
* @params: devName -- device name,status -- device status(on/off)
* @return: 0 -- success, -1 --failed
*
* @since 1.0
* @version 1.0
*/
typedef void (*funLightCallback)(char *devName, int status);

// the callback params
typedef struct _stCallBackParam {
    funLightCallback On;            // light on
    funLightCallback Off;           // light off
    funCallback OnDisplayTP;        // display temperature
    funCallback OnDisplayTH;        // display humidity
    funLightCallback OnConnect;     // socket connect
    funLightCallback OnDisconnect;  // socket disconnect
}stCallBackParam;

/*
* @brief: create the socket service
*
* @since 1.0
* @version 1.0
*/
void SocketServiceCreate();

/*
* @brief: register callback
* @params: callParam -- reference stCallBackParam
*
* @since 1.0
* @version 1.0
*/
void RegisterSocketCallback(stCallBackParam* callParam);

/*
* @brief: send message to the device.
* @params: devName -- the device name witch the message to send
*         buf -- the message to send
*         size -- the message length
*
* @since 1.0
* @version 1.0
*/
int WriteSocket(const char* devName, const char* buf, int size);

/*
* @brief: unregister callback
*
* @since 1.0
* @version 1.0
*/
void UnRegisterSocketCallback();

/*
* @brief: close the socket service and release the resource
*
* @since 1.0
* @version 1.0
*/
void SocketServiceDelete();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif  /* __cplusplus */

#endif  /* __SOCKET_SERVICE_H__ */

