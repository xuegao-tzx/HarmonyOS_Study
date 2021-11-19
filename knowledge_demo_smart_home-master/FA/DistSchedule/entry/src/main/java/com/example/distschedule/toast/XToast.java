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
import ohos.agp.components.*;
import ohos.agp.components.element.Element;
import ohos.agp.components.element.ShapeElement;
import ohos.agp.components.element.VectorElement;
import ohos.agp.text.Font;
import ohos.agp.utils.Color;
import ohos.agp.window.dialog.ToastDialog;
import ohos.app.Context;

import static com.example.distschedule.toast.DistSchedule.getContext;

public class XToast {
    public static final int LENGTH_SHORT = 1500;
    public static final int LENGTH_LONG = 4000;
    private static ToastDialog lastToast = null;

    private XToast() {
        throw new UnsupportedOperationException("u can't instantiate me...");
    }

    public static ToastDialog normal(Context context, int message) {
        return normal(context, context.getString(message), LENGTH_SHORT, null, false);
    }

    public static ToastDialog normal(Context context, String message) {
        return normal(context, message, LENGTH_SHORT, null, false);
    }

    public static ToastDialog normal(Context context, int message, VectorElement icon) {
        return normal(context, context.getString(message), LENGTH_SHORT, icon, true);
    }

    public static ToastDialog normal(Context context, String message, VectorElement icon) {
        return normal(context, message, LENGTH_SHORT, icon, true);
    }

    public static ToastDialog normal(Context context, int message, int duration) {
        return normal(context, context.getString(message), duration, null, false);
    }

    public static ToastDialog normal(Context context, String message, int duration) {
        return normal(context, message, duration, null, false);
    }

    public static ToastDialog normal(Context context, int message, int duration,
                                     VectorElement icon) {
        return normal(context, context.getString(message), duration, icon, true);
    }

    public static ToastDialog normal(Context context, String message, int duration,
                                     VectorElement icon) {
        return normal(context, message, duration, icon, true);
    }

    public static ToastDialog normal(Context context, int message, int duration,
                                     VectorElement icon, boolean isWithIcon) {
        return custom(context, context.getString(message), icon,
                Utils.getColor(context, ResourceTable.Color_toast_normal_tint_color),
                Utils.getColor(context, ResourceTable.Color_toast_default_text_color),
                duration, isWithIcon, true);
    }

    public static ToastDialog normal(Context context, String message, int duration,
                                     VectorElement icon, boolean isWithIcon) {
        return custom(context, message,
                Utils.getVectorDrawable(context, ResourceTable.Graphic_xtoast_ic_info_outline_white),
                Utils.getColor(context, ResourceTable.Color_toast_normal_tint_color),
                Utils.getColor(context, ResourceTable.Color_toast_default_text_color), duration, isWithIcon, true);
    }

    public static ToastDialog warning(Context context, int message) {
        return warning(context, context.getString(message), LENGTH_SHORT, true);
    }

    public static ToastDialog warning(Context context, String message) {
        return warning(context, message, LENGTH_SHORT, true);
    }

    public static ToastDialog warning(Context context, int message, int duration) {
        return warning(context, context.getString(message), duration, true);
    }

    public static ToastDialog warning(Context context, String message, int duration) {
        return warning(context, message, duration, true);
    }

    public static ToastDialog warning(Context context, int message, int duration, boolean isWithIcon) {
        return custom(context, context.getString(message),
                Utils.getVectorDrawable(context, ResourceTable.Graphic_xtoast_ic_error_outline_white),
                Utils.getColor(context, ResourceTable.Color_toast_warning_color),
                Utils.getColor(context, ResourceTable.Color_toast_default_text_color),
                duration, isWithIcon, true);
    }

    public static ToastDialog warning(Context context, String message, int duration, boolean isWithIcon) {
        return custom(context, message,
                Utils.getVectorDrawable(context, ResourceTable.Graphic_xtoast_ic_warning_outline_red),
                Utils.getColor(context, ResourceTable.Color_toast_warning_color),
                Utils.getColor(context, ResourceTable.Color_toast_warning_text_color),
                duration, isWithIcon, true);
    }

    public static ToastDialog info(Context context, int message) {
        return info(context, context.getString(message), LENGTH_SHORT, true);
    }

    public static ToastDialog info(Context context, String message) {
        return info(context, message, LENGTH_SHORT, true);
    }

    public static ToastDialog info(Context context, int message, int duration) {
        return info(context, context.getString(message), duration, true);
    }

    public static ToastDialog info(Context context, String message, int duration) {
        return info(context, message, duration, true);
    }

    public static ToastDialog info(Context context, int message, int duration, boolean isWithIcon) {
        return custom(context, context.getString(message),
                Utils.getVectorDrawable(context, ResourceTable.Graphic_xtoast_ic_info_outline_white),
                Utils.getColor(context, ResourceTable.Color_toast_info_color),
                Utils.getColor(context, ResourceTable.Color_toast_default_text_color),
                duration, isWithIcon, true);
    }

    public static ToastDialog info(Context context, String message, int duration, boolean isWithIcon) {
        return custom(context, message,
                Utils.getVectorDrawable(context, ResourceTable.Graphic_xtoast_ic_info_outline_white),
                Utils.getColor(context, ResourceTable.Color_toast_info_color),
                Utils.getColor(context, ResourceTable.Color_toast_default_text_color),
                duration, isWithIcon, true);
    }

    public static ToastDialog success(Context context, int message) {
        return success(context, context.getString(message), LENGTH_SHORT, true);
    }

    public static ToastDialog success(Context context, String message) {
        return success(context, message, LENGTH_SHORT, true);
    }

    public static ToastDialog success(Context context, int message, int duration) {
        return success(context, context.getString(message), duration, true);
    }

    public static ToastDialog success(Context context, String message, int duration) {
        return success(context, message, duration, true);
    }

    public static ToastDialog success(Context context, int message, int duration, boolean withIcon) {
        return custom(context, context.getString(message),
                Utils.getVectorDrawable(context, ResourceTable.Graphic_xtoast_ic_check_white),
                Utils.getColor(context, ResourceTable.Color_toast_success_color),
                Utils.getColor(context, ResourceTable.Color_toast_default_text_color),
                duration, withIcon, true);
    }

    public static ToastDialog success(Context context, String message, int duration, boolean withIcon) {
        return custom(context, message,
                Utils.getVectorDrawable(context, ResourceTable.Graphic_xtoast_ic_check_white),
                Utils.getColor(context, ResourceTable.Color_toast_success_color),
                Utils.getColor(context, ResourceTable.Color_toast_default_text_color),
                duration, withIcon, true);
    }

    public static ToastDialog error(Context context, int message) {
        return error(context, context.getString(message), LENGTH_SHORT, true);
    }

    public static ToastDialog error(Context context, String message) {
        return error(context, message, LENGTH_SHORT, true);
    }

    public static ToastDialog error(Context context, int message, int duration) {
        return error(context, context.getString(message), duration, true);
    }

    public static ToastDialog error(Context context, String message, int duration) {
        return error(context, message, duration, true);
    }

    public static ToastDialog error(Context context, int message, int duration, boolean withIcon) {
        return custom(context, context.getString(message), Utils.getVectorDrawable(context, ResourceTable.Graphic_xtoast_ic_clear_white),
                Utils.getColor(context, ResourceTable.Color_toast_error_color), Utils.getColor(context, ResourceTable.Color_toast_default_text_color),
                duration, withIcon, true);
    }

    public static ToastDialog error(Context context, String message, int duration, boolean withIcon) {
        return custom(context, message, Utils.getVectorDrawable(context, ResourceTable.Graphic_xtoast_ic_clear_white),
                Utils.getColor(context, ResourceTable.Color_toast_error_color), Utils.getColor(context, ResourceTable.Color_toast_default_text_color),
                duration, withIcon, true);
    }

    public static ToastDialog custom(Context context, int message, VectorElement icon,
                                     int duration, boolean withIcon) {
        return custom(context, context.getString(message), icon, -1, Utils.getColor(context, ResourceTable.Color_toast_default_text_color),
                duration, withIcon, false);
    }

    public static ToastDialog custom(Context context, String message, VectorElement icon,
                                     int duration, boolean withIcon) {
        return custom(context, message, icon, -1, Utils.getColor(context, ResourceTable.Color_toast_default_text_color),
                duration, withIcon, false);
    }

    public static ToastDialog custom(Context context, int message, int iconRes,
                                     int tintColorRes, int duration,
                                     boolean withIcon, boolean shouldTint) {
        return custom(context, context.getString(message), Utils.getVectorDrawable(context, iconRes),
                Utils.getColor(context, tintColorRes), Utils.getColor(context, ResourceTable.Color_toast_default_text_color),
                duration, withIcon, shouldTint);
    }

    public static ToastDialog custom(Context context, String message, int iconRes,
                                     int tintColorRes, int duration,
                                     boolean withIcon, boolean shouldTint) {
        return custom(context, message, Utils.getVectorDrawable(context, iconRes),
                Utils.getColor(context, tintColorRes), Utils.getColor(context, ResourceTable.Color_toast_default_text_color),
                duration, withIcon, shouldTint);
    }

    public static ToastDialog custom(Context context, int message, VectorElement icon,
                                     int tintColorRes, int duration,
                                     boolean withIcon, boolean shouldTint) {
        return custom(context, context.getString(message), icon, Utils.getColor(context, tintColorRes),
                Utils.getColor(context, ResourceTable.Color_toast_default_text_color), duration, withIcon, shouldTint);
    }

    public static ToastDialog custom(Context context, int message, VectorElement icon,
                                     int tintColorRes, int textColorRes, int duration,
                                     boolean withIcon, boolean shouldTint) {
        return custom(context, context.getString(message), icon, Utils.getColor(context, tintColorRes),
                Utils.getColor(context, textColorRes), duration, withIcon, shouldTint);
    }


    public static ToastDialog custom(Context context, String message, VectorElement icon,
                                     int tintColor, int textColor, int duration,
                                     boolean withIcon, boolean shouldTint) {
        final ToastDialog currentToast = new ToastDialog(context);
        final Component toastLayout = LayoutScatter.getInstance(context).parse(ResourceTable.Layout_layout_xtoast, null, false);
        final Component rootLayout = toastLayout.findComponentById(ResourceTable.Id_toast_layout);
        final Image toastIcon = (Image) toastLayout.findComponentById(ResourceTable.Id_toast_icon);
        final Text toastTextView = (Text) toastLayout.findComponentById(ResourceTable.Id_toast_text);
        Element drawableFrame = null;
        if (shouldTint) {
            drawableFrame = Utils.tint9PatchDrawableFrame(context, tintColor);
        } else {
            drawableFrame = new ShapeElement(context, ResourceTable.Graphic_xtoast_frame);
        }
        Utils.setBackground(rootLayout, drawableFrame);

        if (withIcon) {
            if (icon == null) {
                throw new IllegalArgumentException("Avoid passing 'icon' as null if 'withIcon' is set to true");
            }
            Utils.setImageBackground(toastIcon,
                    Config.get().tintIcon ? Utils.tintImageIcon(icon, textColor) : icon);
        } else {
            toastIcon.setVisibility(Component.HIDE);
        }

        toastTextView.setText(message);
//        toastTextView.setTextColor(new Color(context.getColor(textColor)));
        toastTextView.setTextColor(new Color(textColor));
        toastTextView.setFont(Font.DEFAULT);
        if (Config.get().textSize != -1) {
            toastTextView.setTextSize(Config.get().textSize);
        }
        if (Config.get().alpha != -1) {
            rootLayout.setAlpha(Config.get().alpha);
        }
        currentToast.setComponent(toastLayout);

        if (!Config.get().allowQueue) {
            if (lastToast != null) {
                lastToast.cancel();
            }
            lastToast = currentToast;
        }
        if (Config.get().gravity != -1) {
            currentToast
                    .setAlignment(Config.get().gravity)
                    .setOffset(Config.get().xOffset, Config.get().yOffset);
        }
        currentToast.setTransparent(true);
        currentToast.setSize(DirectionalLayout.LayoutConfig.MATCH_CONTENT,
                DirectionalLayout.LayoutConfig.MATCH_CONTENT);
        currentToast.setOffset(0, AttrHelper.fp2px(Constents.TOAST_OFFSET_X, getContext()));
        return currentToast;
    }

    public static class Config {
        private static final Font LOADED_TOAST_TYPEFACE = new Font.Builder("sans-serif-condensed").build();
        private static volatile Config sInstance = null;
        private Font typeface = LOADED_TOAST_TYPEFACE;
        private int textSize = -1; //sp
        private boolean tintIcon = true;
        private boolean allowQueue = true;
        private int alpha = -1;
        private int gravity = -1;
        private int xOffset = 0;
        private int yOffset = 0;

        private Config() {

        }

        /**
         * 获取单例
         *
         * @return return
         */
        public static Config get() {
            if (sInstance == null) {
                synchronized (Config.class) {
                    if (sInstance == null) {
                        sInstance = new Config();
                    }
                }
            }
            return sInstance;
        }

        public void reset() {
            typeface = LOADED_TOAST_TYPEFACE;
            textSize = -1;
            tintIcon = true;
            allowQueue = true;
            alpha = -1;
            gravity = -1;
            xOffset = 0;
            yOffset = 0;
        }

        public Config setToastTypeface(Font typeface) {
            if (typeface != null) {
                this.typeface = typeface;
            }
            return this;
        }

        public Config setTextSize(int sizeInSp) {
            this.textSize = sizeInSp;
            return this;
        }

        public Config tintIcon(boolean tintIcon) {
            this.tintIcon = tintIcon;
            return this;
        }

        public Config setAlpha(int alpha) {
            this.alpha = alpha;
            return this;
        }

        public Config allowQueue(boolean allowQueue) {
            this.allowQueue = allowQueue;
            return this;
        }

        public Config setGravity(int gravity) {
            this.gravity = gravity;
            return this;
        }

        public Config setGravity(int gravity, int xOffset, int yOffset) {
            this.gravity = gravity;
            this.xOffset = xOffset;
            this.yOffset = yOffset;
            return this;
        }

        public Config resetGravity() {
            gravity = -1;
            xOffset = 0;
            yOffset = 0;
            return this;
        }

        public Config setXOffset(int xOffset) {
            this.xOffset = xOffset;
            return this;
        }

        public Config setYOffset(int yOffSet) {
            this.yOffset = yOffSet;
            return this;
        }
    }
}
