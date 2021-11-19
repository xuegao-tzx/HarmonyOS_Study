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

#include <stdbool.h>
#include "rtdText.h"
#include "rtdUri.h"
#include "ndef.h"
#include "nfc.h"

bool storeUrihttp(RecordPosEnu position, uint8_t *http){

    NDEFDataStr data;


    prepareUrihttp(&data, position, http);
    return   NT3HwriteRecord( &data );
}

bool storeText(RecordPosEnu position, uint8_t *text){
    NDEFDataStr data;
    prepareText(&data, position, text);
    return NT3HwriteRecord( &data );
}
