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

#include "iot_sntp.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "ohos_types.h"
#include "cmsis_os2.h"

#include "lwip/sockets.h"
#include "kv_store.h"

// this is the default SNTP server and port defines, you could define your own by add it in the config.gni
#ifndef NTP_SERVER_IP
#define NTP_SERVER_IP "120.24.166.46"
#endif
#ifndef NTP_SERVER_PORT
#define NTP_SERVER_PORT    123
#endif

/**
 * The ntp server returns the time from 1900-1-1-0-0-0, and the unix
 * timestamp compute from 1970-1-1-0-0-0, so we define the seconds between
 * 1900-1-1-0-0-0 and 1970-1-1-0-0-0; when we get timestamp from NTP server,
 * we must sub this time
 */
#define SECONDS_FROM_1900_TO_1970    (2208988800L)

/**
 * @brief NTP procto packet message
 * 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * | LI|  VN |Mode |    Stratum    |  Poll         | Precision    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * |                 Root delay (32Bits)                          |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * |                 Root dispersion (32Bits)                     |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * |                 Reference identifier (32Bits)                |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * |                                                              |
 * |                 Reference timestamp (64Bits)                 |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * |                                                              |
 * |                 Originate timestamp (64Bits)                 |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * |                                                              |
 * |                 Receive timestamp (64Bits)                   |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * |                                                              |
 * |                 Transmit timestamp (64Bits)                  |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 * |                                                              |
 * |                 Authenticator(optional 96Bits)               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 *
 * 主要字段的解释如下：
 *    LI：当前时间闰秒标志。字段长度为2位整数，只在服务器端有效。取值定义为：
 *    LI=0：无警告；
 *    LI=1：最后一分钟是61秒；
 *    LI=2：最后一分钟是59秒；
 *    LI=3：警告（时钟没有同步）
 *    服务器在开始时，LI设置为3，一旦与主钟取得同步后就设置成其它值。
 *    VN（Version Number）：长度为3比特，表示NTP的版本号，可以是3或者是4
 *    Mode：指示协议模式。字段长度为3位，取值定义为：
 *    Mode=0：保留
 *    Mode=1：对称主动；
 *    Mode=2：对称被动；
 *    Mode=3：客户；
 *    Mode=4：服务器；
 *    Mode=5：广播；
 *    Mode=6：保留为NTP控制信息；
 *    Mode=7：保留为用户定义；
 *    在单播和多播模式，客户在请求时把这个字段设置为3，服务器在响应时把这个字段设置为4。在广播模式下，服务器把这个字段设置为5。
 *    Poll Interval：指示数据包的最大时间间隔，以秒为单位，作为2的指数方的指数部分，该字段只在服务器端有效。字段长度为8位整数，取值范围从4-17，即16秒到131,072秒。
 *    Precision：指示系统时钟的精确性，以秒为单位，作为2的指数方的指数部分，该字段只在服务器端有效。字段长度为8位符号整数，取值范围从-6到-20。
 *    Root Delay：指示与主时钟参考源的总共往返延迟，以秒为单位，该字段只在服务器端有效。字段长度为32位浮点数，小数部分在16位以后，取值范围从负几毫秒到正几百毫秒。
 *    Root Dispersion：指示与主时钟参考源的误差，以秒为单位，该字段只在服务器端有效。字段长度为32位浮点数，小数部分在16位以后，取值范围从零毫秒到正几百毫秒。
 *    Reference Identifier：指示时钟参考源的标记，该字段只在服务器端有效。对于一级服务器，字段长度为4字节ASCII字符串，左对齐不足添零。对于二级服务器，在IPV4环境下，取值为一级服务器的IP地址，在IPV6环境下，是一级服务器的NSAP地址。
 *    Reference Timestamp：指示系统时钟最后一次校准的时间，该字段只在服务器端有效，以前面所述64位时间戳格式表示。
 *    Originate Timestamp：指示客户向服务器发起请求的时间，以前面所述64位时间戳格式表示。
 *    Receive Timestamp：指服务器收到客户请求的时间 ，以前面所述64位时间戳格式表示。
 *    Transmit Timestamp：指示服务器向客户发时间戳的时间，以前面所述64位时间戳格式表示。
 *    Authenticator（可选）：当需要进行SNTP认证时，该字段包含密钥和信息加密码
 *
 */

/**
 * @brief defines the NTP packet corresponding to the procto
 *
 */
#pragma pack(1)
typedef struct {
    uint32_t low;
    uint32_t high;
}TimeStamp; // for the 64Bits timestamp use

typedef struct {
    uint8_t liVnMode;
    uint8_t stratum;
    uint8_t poll;
    uint8_t precision;
    uint32_t rootDelay;
    uint32_t rootDispersion;
    uint8_t referenceID[4];
    TimeStamp refecenceTimestamp;
    TimeStamp originateTimestamp;
    TimeStamp receiveTimestamp;
    TimeStamp transmitTimestamp;
}SntpPacket;
#pragma pack()

#define CN_TIMEZONE_MAXIMUM  (11)
#define CN_TIMEZONE_MINIMUM  (-11)
#define CN_SNTP_RETRYTIMES  10

int SntpGetRtcTime(int localTimeZone, struct tm *rtcTime)
{
    int ret = -1;
    int sockfd = -1;
    struct sockaddr_in server;
    socklen_t addrLen;
    fd_set set;
    struct timeval timeout;
    struct timeval tv;
    int nRet;
    SntpPacket sntpPacket;
    struct tm *time = NULL;
    int tryTimes = CN_SNTP_RETRYTIMES;

    // check the params passed to this API
    if ((localTimeZone > CN_TIMEZONE_MAXIMUM) || (localTimeZone < CN_TIMEZONE_MINIMUM) ||\
        rtcTime == NULL) {
        return ret;
    }

    // prepare the server address
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(NTP_SERVER_PORT);
    addrLen = sizeof(server);
    if (inet_aton(NTP_SERVER_IP, &server.sin_addr) <= 0) {
        printf("inet_pton error\n");
        return ret;
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return ret;
    }

    // we do the loop here to get the time
    do{
        tryTimes--;
        // prepare the send packet
        bzero((char * )&sntpPacket, sizeof(sntpPacket));  // clean it
        sntpPacket.liVnMode = 0x1b; // set the mode to client
        if (sendto(sockfd, (char *) &sntpPacket, sizeof(sntpPacket), 0,
            (struct sockaddr*) &server, addrLen) < 0) {
            printf("sendto error!\n");
            continue;
        }
        // use select to get the data
        FD_ZERO(&set);
        FD_SET(sockfd, &set);
        timeout.tv_sec = 0;
        timeout.tv_usec = 500000;
        nRet = select(sockfd + 1, &set, NULL, NULL, &timeout);
        if (nRet < 0) {
            printf("select error!\n");
            continue;
        } else if (nRet == 0) {
            printf("time out!\n");
            continue;
        } else {
            if (FD_ISSET(sockfd, &set)) {
                if (recvfrom(sockfd, (char *) &sntpPacket, sizeof(sntpPacket),
                    0, (struct sockaddr*) &server, &addrLen) < 0) {
                    printf("recv error!\n");
                    continue;
                } else {
                    tv.tv_usec = 0;
                    tv.tv_sec = ntohl(sntpPacket.transmitTimestamp.low);
                    tv.tv_sec -= SECONDS_FROM_1900_TO_1970;
                    tv.tv_sec += (localTimeZone * 3600);
                    if((time = gmtime_r((const long int *) &tv.tv_sec, rtcTime)) != NULL) {
                        ret = 0;
                        break;
                    }
                }
            }
        }
    } while (tryTimes > 0);
    closesocket(sockfd);

    return ret;
}