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

package com.example.distschedule.toast;

import com.example.distschedule.ResourceTable;
import ohos.agp.colors.RgbColor;
import ohos.agp.components.Component;
import ohos.agp.components.Image;
import ohos.agp.components.element.Element;
import ohos.agp.components.element.ShapeElement;
import ohos.agp.components.element.VectorElement;
import ohos.app.Context;

final class Utils {
    private Utils() {
    }

    static Element tintIcon(ShapeElement element, int tintColor) {
        RgbColor rgbColor = RgbColor.fromArgbInt(tintColor);
        element.setRgbColor(rgbColor);
        return element;
    }

    static Element tint9PatchDrawableFrame(Context context, int tintColor) {
        ShapeElement toastElement = new ShapeElement(context, ResourceTable.Graphic_xtoast_frame);
        return tintIcon(toastElement, tintColor);
    }

    static void setBackground(Component view, Element element) {
        view.setBackground(element);
    }

    static void setImageBackground(Image view, VectorElement element) {
        view.setBackground(element);
    }

    static VectorElement tintImageIcon(VectorElement element, int tintColor) {
        return element;
    }

    static VectorElement getVectorDrawable(Context context, int id) {
        return new VectorElement(context, id);
    }

    static int getColor(Context context, int color) {
        return context.getColor(color);
    }
}
