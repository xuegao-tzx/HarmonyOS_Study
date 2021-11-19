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

#include "nfcForum.h"
#include <string.h>

static void rtdHeader(uint8_t type, NDEFRecordStr *ndefRecord, uint8_t *I2CMsg) {
    ndefRecord->header |= 1;
    ndefRecord->header |= BIT_SR;
    I2CMsg[0] = ndefRecord->header;

    ndefRecord->typeLength = 1;
    I2CMsg[1] = ndefRecord->typeLength;


    ndefRecord->type.typeCode=type;
    I2CMsg[3] = ndefRecord->type.typeCode;
}


uint8_t composeRtdText(const NDEFDataStr *ndef, NDEFRecordStr *ndefRecord, uint8_t *I2CMsg) {
    uint8_t retLen;
    
    uint16_t payLoadLen = addRtdText(&ndefRecord->type.typePayload.text);

    ndefRecord->payloadLength = ndef->rtdPayloadlength+payLoadLen; // added the typePayload
    if(ndefRecord->payloadLength>255){    
        I2CMsg[0] = 0xc1;
        I2CMsg[1] = 0x01;
        I2CMsg[2] = 0x00;
        I2CMsg[3] = 0x00;
        I2CMsg[4]=ndefRecord->payloadLength/256;
        I2CMsg[5]=ndefRecord->payloadLength%256;
        ndefRecord->type.typeCode=RTD_TEXT;
        I2CMsg[6] = ndefRecord->type.typeCode;
        memcpy(&I2CMsg[7], &ndefRecord->type.typePayload.text, payLoadLen);
        retLen = 6+ /*sizeof(ndefRecord->header) +
                    sizeof(ndefRecord->typeLength) +
                    sizeof(ndefRecord->payloadLength) +*/
                3 + //sizeof(RTDTextTypeStr)-sizeof(TextExtraDataStr)
                1   /*sizeof(ndefRecord->type.typeCode)*/;

        return retLen;
    }
        ndefRecord->header |= 1;
        ndefRecord->header |= BIT_SR;
        I2CMsg[0] = ndefRecord->header;

        ndefRecord->typeLength = 1;
        I2CMsg[1] = ndefRecord->typeLength;

        I2CMsg[2]=ndefRecord->payloadLength;

        ndefRecord->type.typeCode=RTD_TEXT;
        I2CMsg[3] = ndefRecord->type.typeCode;
        memcpy(&I2CMsg[4], &ndefRecord->type.typePayload.text, payLoadLen);
        retLen = 3 + /*sizeof(ndefRecord->header) +
                    sizeof(ndefRecord->typeLength) +
                    sizeof(ndefRecord->payloadLength) +*/
                3 + //sizeof(RTDTextTypeStr)-sizeof(TextExtraDataStr)
                1   /*sizeof(ndefRecord->type.typeCode)*/;

        return retLen;
}


uint8_t composeRtdUri(const NDEFDataStr *ndef, NDEFRecordStr *ndefRecord, uint8_t *I2CMsg) {

    rtdHeader(RTD_URI, ndefRecord, I2CMsg);

    uint8_t payLoadLen = addRtdUriRecord(ndef, &ndefRecord->type.typePayload.uri);
    memcpy(&I2CMsg[4], &ndefRecord->type.typePayload.uri, payLoadLen);

    ndefRecord->payloadLength = ndef->rtdPayloadlength+payLoadLen; // added the typePayload
    I2CMsg[2]=ndefRecord->payloadLength;

    return 5;
    /* retLen = sizeof(ndefRecord->header) +
                sizeof(ndefRecord->typeLength) +
                sizeof(ndefRecord->payloadLength) +
                sizeof(1) + //ndefRecord->type.typePayload.uri.type
                sizeof(ndefRecord->type.typeCode);
     */

}

void composeNDEFMBME(bool isFirstRecord, bool isLastRecord, NDEFRecordStr *ndefRecord) {
    if (isFirstRecord)
        ndefRecord->header |= BIT_MB;
    else
        ndefRecord->header &= ~MASK_MB;

    if (isLastRecord)
        ndefRecord->header |= BIT_ME;
    else
        ndefRecord->header &= ~MASK_ME;
}
