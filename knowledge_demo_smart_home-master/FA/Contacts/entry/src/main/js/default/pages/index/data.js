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
const HEAD_IMAGE = ["/common/head0.png", "/common/head1.png", "/common/head2.png", "/common/head3.png", "/common/head4.png"];

export default {
//数据字段定义
    contactList: [
            {
                image: HEAD_IMAGE[0],
                name: "Alan",
                phone: "020-00000000"
            },
            {
                image: HEAD_IMAGE[2],
                name: "Bob",
                phone: "020-11111111"
            },
            {
                image: HEAD_IMAGE[3],
                name: "Carl",
                phone: "0755-33333333"
            },
            {
                image: HEAD_IMAGE[4],
                name: "David",
                phone: "19876543210"
            },
            {
                image: HEAD_IMAGE[0],
                name: "Alan",
                phone: "020-00000000"
            },
            {
                image: HEAD_IMAGE[1],
                name: "Bob",
                phone: "020-11111111"
            },
            {
                image: HEAD_IMAGE[2],
                name: "Carl",
                phone: "12345678910"
            },
            {
                image: HEAD_IMAGE[3],
                name: "David",
                phone: "19876543210"
            },
            {
                image: HEAD_IMAGE[4],
                name: "Alan",
                phone: "020-00000000"
            },
            {
                image: HEAD_IMAGE[0],
                name: "Bob",
                phone: "020-11111111"
            },
            {
                image: HEAD_IMAGE[1],
                name: "Carl",
                phone: "12345678910"
            },
            {
                image: HEAD_IMAGE[2],
                name: "David",
                phone: "19876543210"
            },
            {
                image: HEAD_IMAGE[3],
                name: "Alan",
                phone: "020-00000000"
            },
            {
                image: HEAD_IMAGE[4],
                name: "Bob",
                phone: "020-11111111"
            },
            {
                image: HEAD_IMAGE[0],
                name: "Carl",
                phone: "12345678910"
            },
            {
                image: HEAD_IMAGE[1],
                name: "David",
                phone: "19876543210"
            }
    ],
//只做数据字段定义
    isShowDialog: true,
    name: "",
    phone: "",
    detailObj: {
        image: "",
        name: "",
        phone: ""
    },
}