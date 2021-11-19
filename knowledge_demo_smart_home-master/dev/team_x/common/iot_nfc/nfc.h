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

#ifndef _NFC_H_
#define _NFC_H_

#include "NT3H.h"

/*
 * The function write in the NT3H a new URI Rtd on the required position
 *
 * param:
 *      position: where add the record
 *      http:     the address to write
 *
 */
bool storeUrihttp(RecordPosEnu position, uint8_t *http);


/*
 * The function write in the NT3H a new Text Rtd on the required position
 *
 * param:
 *      position: where add the record
 *      text:     the text to write
 *
 */
bool storeText(RecordPosEnu position, uint8_t *text);

#endif /* NFC_H_ */
