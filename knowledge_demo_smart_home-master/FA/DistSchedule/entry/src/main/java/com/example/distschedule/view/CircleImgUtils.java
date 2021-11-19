/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

package com.example.distschedule.view;

import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.media.image.ImageSource;
import ohos.media.image.PixelMap;
import ohos.media.image.common.PixelFormat;
import ohos.media.image.common.Rect;
import ohos.media.image.common.Size;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Optional;

/**
 * 圆形图片工具类
 *
 * @since 2021-07-27
 */
public class CircleImgUtils {
    private static final String IMAGE_TYPE = "image/png";

    private static final int HILOG_TYPE = 3;

    private static final int HILOG_DOMAIN = 0xD000F00;

    private static final HiLogLabel LABEL = new HiLogLabel(HILOG_TYPE, HILOG_DOMAIN, "[CircleImgUtils] ");

    private static final int IO_END_LEN = -1;

    private static final int CACHE_SIZE = 256 * 1024;

    private CircleImgUtils() {
    }

    /**
     * * getPixelMapByUri
     *
     * @param uri uri
     * @return PixelMap pixelMap
     * @throws NullPointerException Exception
     */
    public static Optional<PixelMap> getPixelMapByUri(String uri) throws NullPointerException {
        byte[] bytes = null;
        if (uri != null) {
            try {
                bytes = readByteFromFile(uri);
            } catch (IOException e) {
                HiLog.error(LABEL, "read data from file failed");
            }
        }
        if (bytes == null || bytes.length == 0) {
            return Optional.empty();
        }
        ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
        srcOpts.formatHint = IMAGE_TYPE;
        ImageSource imageSource = ImageSource.create(bytes, srcOpts);
        if (imageSource == null) {
            return Optional.empty();
        }

        ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
        decodingOpts.desiredSize = new Size(0, 0);
        decodingOpts.desiredRegion = new Rect(0, 0, 0, 0);
        decodingOpts.desiredPixelFormat = PixelFormat.ARGB_8888;
        PixelMap decodePixelMap = imageSource.createPixelmap(decodingOpts);
        return Optional.of(decodePixelMap);
    }

    /**
     * * Read File
     *
     * @param filePath filePath
     * @return byte array
     * @throws IOException for reading data from file
     */
    public static byte[] readByteFromFile(String filePath) throws IOException {
        FileInputStream fileInputStream = null;
        byte[] cacheBytes = new byte[CACHE_SIZE];
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        byte[] bytes = new byte[0];
        int len = IO_END_LEN;
        try {
            fileInputStream = new FileInputStream(new File(filePath));
            len = fileInputStream.read(cacheBytes);
            while (len != IO_END_LEN) {
                baos.write(cacheBytes, 0, len);
                len = fileInputStream.read(cacheBytes);
            }
            bytes = baos.toByteArray();
        } catch (IOException e) {
            HiLog.error(LABEL, "obtain data file stream failed");
        } finally {
            if (fileInputStream != null) {
                fileInputStream.close();
            }
            cacheBytes = null;
            try {
                baos.close();
            } catch (IOException e) {
                HiLog.error(LABEL, "close stream failed");
            }
        }
        return bytes;
    }
}
