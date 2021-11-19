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

import ohos.agp.components.AttrSet;
import ohos.agp.components.Component;
import ohos.agp.components.Image;
import ohos.agp.components.element.Element;
import ohos.agp.components.element.PixelMapElement;
import ohos.agp.render.*;
import ohos.agp.utils.Color;
import ohos.agp.utils.Matrix;
import ohos.app.Context;
import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.media.image.PixelMap;
import ohos.media.image.common.AlphaType;
import ohos.media.image.common.PixelFormat;
import ohos.media.image.common.Size;

import java.util.Optional;

import static ohos.agp.render.Shader.TileMode.CLAMP_TILEMODE;

/**
 * 圆形图片
 *
 * @since 2021-07-27
 */
public class CircleImageView extends Image implements Component.DrawTask {
    private static final ScaleMode SCALE_TYPE = ScaleMode.CENTER;

    private static final PixelFormat PIXELMAP_CONFIG = PixelFormat.ARGB_8888;

    private static final int DEFAULT_BORDER_WIDTH = 2;

    private static final Color DEFAULT_BORDER_COLOR = Color.BLACK;

    private static final Color DEFAULT_CIRCLE_BACKGROUND_COLOR = Color.TRANSPARENT;

    private static final float DEFAULT_IMAGE_ALPHA = 255;

    private static final float DEFAULT_VALUE_TWO_FLOAT = 2.0f;

    private static final float DEFAULT_VALUE_TWO = 2;

    private static final float DEFAULT_VALUE_POINT_FIVE = 0.5f;

    private static final String ATTRIBUTE_BORDER_COLOR = "civ_color";

    private static final String ATTRIBUTE_BORDER_WIDTH = "civ_border_width";

    private static final String ATTRIBUTE_BG_COLOR = "civ_bg_color";

    private static final int HILOG_TYPE = 3;

    private static final int HILOG_DOMAIN = 0xD000F00;

    private static final HiLogLabel LABEL = new HiLogLabel(HILOG_TYPE, HILOG_DOMAIN, "[CircleImageView] ");

    private final RectF mDrawableRect = new RectF();

    private final RectF mBorderRect = new RectF();

    private final RectF pixelMapRect = new RectF();
    private final Paint mPixelMapPaint = new Paint();
    private final Paint mBorderPaint = new Paint();
    private final Paint mCircleBackgroundPaint = new Paint();
    private Matrix mShaderMatrix = new Matrix();
    private Color mBorderColor = DEFAULT_BORDER_COLOR;

    private int mBorderWidth = DEFAULT_BORDER_WIDTH;

    private boolean mIsBorderOverlay = false;

    private Color mCircleBackgroundColor = DEFAULT_CIRCLE_BACKGROUND_COLOR;

    private float mImageAlpha = DEFAULT_IMAGE_ALPHA;

    private PixelMap mPixelMap;

    private Canvas mPixelMapCanvas;

    private float mDrawableRadius;

    private float mBorderRadius;

    private ColorMatrix mColorFilter = new ColorMatrix();

    private boolean mIsInitialized;

    private boolean mIsRebuildShader;

    private boolean mIsDrawableDirty;

    private boolean mIsDisableCircularTransformation;

    /**
     * CircleImageView Constructor
     *
     * @param context context
     */
    public CircleImageView(Context context) {
        super(context);
        init();
    }

    /**
     * CircleImageView Constructor
     *
     * @param context context
     * @param attrs   attributeset
     */
    public CircleImageView(Context context, AttrSet attrs) {
        this(context, attrs, null);
    }

    /**
     * CircleImageView Constructor
     *
     * @param context  context
     * @param attrs    attributeset
     * @param defStyle defStyle
     */
    public CircleImageView(Context context, AttrSet attrs, String defStyle) {
        super(context, attrs, defStyle);
        boolean isPresentBorderWidth = attrs.getAttr(ATTRIBUTE_BORDER_WIDTH).isPresent();
        if (isPresentBorderWidth) {
            mBorderWidth = attrs.getAttr(ATTRIBUTE_BORDER_WIDTH).get().getDimensionValue();
        } else {
            mBorderWidth = DEFAULT_BORDER_WIDTH;
        }
        boolean isPresentBorderColor = attrs.getAttr(ATTRIBUTE_BORDER_COLOR).isPresent();
        if (isPresentBorderColor) {
            mBorderColor = attrs.getAttr(ATTRIBUTE_BORDER_COLOR).get().getColorValue();
        } else {
            mBorderColor = DEFAULT_BORDER_COLOR;
        }
        boolean isPresentBgColor = attrs.getAttr(ATTRIBUTE_BG_COLOR).isPresent();
        if (isPresentBgColor) {
            mCircleBackgroundColor = attrs.getAttr(ATTRIBUTE_BG_COLOR).get().getColorValue();
        } else {
            mCircleBackgroundColor = DEFAULT_CIRCLE_BACKGROUND_COLOR;
        }
        init();
    }

    private void init() {
        mIsInitialized = true;
        super.setScaleMode(SCALE_TYPE);
        mPixelMapPaint.setAntiAlias(true);
        mPixelMapPaint.setDither(true);
        mPixelMapPaint.setFilterBitmap(true);
        mPixelMapPaint.setAlpha(mImageAlpha);
        mPixelMapPaint.setColorMatrix(mColorFilter);
        mBorderPaint.setStyle(Paint.Style.STROKE_STYLE);
        mBorderPaint.setAntiAlias(true);
        mBorderPaint.setColor(mBorderColor);
        mBorderPaint.setStrokeWidth(mBorderWidth);
        mCircleBackgroundPaint.setStyle(Paint.Style.FILL_STYLE);
        mCircleBackgroundPaint.setAntiAlias(true);
        mCircleBackgroundPaint.setColor(mCircleBackgroundColor);
        setLayoutRefreshedListener(new RefreshListener());
        addDrawTask(this);
    }

    @Override
    public void onDraw(Component view, Canvas canvas) {
        if (mIsDisableCircularTransformation) {
            super.setPixelMap(mPixelMap);
            return;
        }

        if (mCircleBackgroundColor != Color.TRANSPARENT) {
            canvas.drawCircle(mDrawableRect.centerX(), mDrawableRect.centerY(), mDrawableRadius,
                    mCircleBackgroundPaint);
        }

        if (mPixelMap != null) {
            if (mIsDrawableDirty && mPixelMapCanvas != null) {
                mIsDrawableDirty = false;
                Element drawable = getImageElement();
                drawable.drawToCanvas(mPixelMapCanvas);
            }

            if (mIsRebuildShader) {
                mIsRebuildShader = false;
                PixelMapHolder pixelMapholder = new PixelMapHolder(mPixelMap);
                Shader pixelMapShader = new PixelMapShader(pixelMapholder, CLAMP_TILEMODE, CLAMP_TILEMODE);
                pixelMapShader.setShaderMatrix(mShaderMatrix);
                mPixelMapPaint.setShader(pixelMapShader, Paint.ShaderType.PIXELMAP_SHADER);
            }
            canvas.drawCircle(mDrawableRect.centerX(), mDrawableRect.centerY(), mDrawableRadius, mPixelMapPaint);
        }
        if (mBorderWidth > 0) {
            canvas.drawCircle(mBorderRect.centerX(), mBorderRect.centerY(), mBorderRadius, mBorderPaint);
        }
    }

    @Override
    public void setPadding(int left, int top, int right, int bottom) {
        super.setPadding(left, top, right, bottom);
        updateDimensions();
        invalidate();
    }

    @Override
    public void setPaddingRelative(int start, int top, int end, int bottom) {
        super.setPaddingRelative(start, top, end, bottom);
        updateDimensions();
        invalidate();
    }

    /**
     * Obtains circle border color
     *
     * @return Color of the border
     */
    public Color getBorderColor() {
        return mBorderColor;
    }

    /**
     * set circle border color
     *
     * @param borderColor borderColor
     */
    public void setBorderColor(Color borderColor) {
        if (borderColor.equals(mBorderColor)) {
            return;
        }

        mBorderColor = borderColor;
        mBorderPaint.setColor(borderColor);
        invalidate();
    }

    /**
     * Gets circle Background color
     * *
     *
     * @return Color color
     */
    public Color getCircleBackgroundColor() {
        return mCircleBackgroundColor;
    }

    /**
     * * Sets Circle background color
     *
     * @param circleBackgroundColor circleBackgroundColor
     */
    public void setCircleBackgroundColor(Color circleBackgroundColor) {
        if (circleBackgroundColor.equals(mCircleBackgroundColor)) {
            return;
        }
        mCircleBackgroundColor = circleBackgroundColor;
        mCircleBackgroundPaint.setColor(circleBackgroundColor);
        invalidate();
    }

    /**
     * Obtains the circle border width
     *
     * @return Width of the border
     */
    public int getBorderWidth() {
        return mBorderWidth;
    }

    /**
     * Sets circle border width
     * *
     *
     * @param borderWidth borderWidth
     */
    public void setBorderWidth(int borderWidth) {
        if (borderWidth != mBorderWidth) {
            mBorderWidth = borderWidth;
            mBorderPaint.setStrokeWidth(borderWidth);
            updateDimensions();
            invalidate();
        }
    }

    /**
     * Checks whether the border overlay is set
     *
     * @return true if border overlay is set
     */
    public boolean isBorderOverlay() {
        return mIsBorderOverlay;
    }

    /**
     * * Sets border overlay
     *
     * @param isBorderOverlay borderOverlay
     */
    public void setBorderOverlay(boolean isBorderOverlay) {
        if (isBorderOverlay == mIsBorderOverlay) {
            return;
        }
        mIsBorderOverlay = isBorderOverlay;
        updateDimensions();
        invalidate();
    }

    /**
     * Checks whether the Circular Transformation is disabled
     *
     * @return true if Circular Transformation is disabled
     */
    public boolean isDisableCircularTransformation() {
        return mIsDisableCircularTransformation;
    }

    /**
     * * Set setDisableCircularTransformation
     *
     * @param isDisableCircularTransformation disableCircularTransformation
     */
    public void setDisableCircularTransformation(boolean isDisableCircularTransformation) {
        if (isDisableCircularTransformation == mIsDisableCircularTransformation) {
            return;
        }

        mIsDisableCircularTransformation = isDisableCircularTransformation;

        if (isDisableCircularTransformation) {
            mPixelMap = null;
            mPixelMapCanvas = null;
            mPixelMapPaint.setShader(null, Paint.ShaderType.PIXELMAP_SHADER);
        } else {
            initializePixelMap();
        }
        invalidate();
    }

    @Override
    public void setPixelMap(PixelMap pixelMap) {
        PixelMap emptyPixelMap = createEmptyPixelMap(pixelMap);
        super.setPixelMap(emptyPixelMap);
        mPixelMap = pixelMap;
        initializePixelMap();
        invalidate();
    }

    @Override
    public void setPixelMap(int resId) {
        super.setPixelMap(resId);
        initializePixelMap();
        invalidate();
    }

    /**
     * Creates an empty pixel map
     *
     * @param pixelMap pixelMap
     * @return pixelMap
     */
    private PixelMap createEmptyPixelMap(PixelMap pixelMap) {
        PixelMap.InitializationOptions initializationOptions = new PixelMap.InitializationOptions();
        initializationOptions.size = new Size(pixelMap.getImageInfo().size.width, pixelMap.getImageInfo().size.height);
        initializationOptions.alphaType = AlphaType.UNKNOWN;
        initializationOptions.pixelFormat = PixelFormat.ARGB_8888;
        return PixelMap.create(initializationOptions);
    }

    /**
     * set image drawable
     *
     * @param drawable drawable
     */
    public void setImageDrawable(Element drawable) {
        Optional<PixelMap> pixelMap = getPixelMapFromDrawable(drawable);
        if (pixelMap.isPresent()) {
            setPixelMap(pixelMap.get());
        } else {
            HiLog.error(LABEL, "set imageview pixelMap failed, pixelMap is empty");
        }
    }

    /**
     * set image Uri
     *
     * @param uri uri
     */
    public void setImageUri(String uri) {
        Optional<PixelMap> pixelMap = CircleImgUtils.getPixelMapByUri(uri);
        if (pixelMap.isPresent()) {
            setPixelMap(pixelMap.get());
        }
        initializePixelMap();
        invalidate();
    }

    @Override
    public float getAlpha() {
        return mImageAlpha;
    }

    @Override
    public void setAlpha(float alpha) {
        if (alpha == mImageAlpha) {
            return;
        }
        mImageAlpha = alpha;

        // This might be called during ImageView construction before
        // member initialization has finished on API level >= 16.
        if (mIsInitialized) {
            mPixelMapPaint.setAlpha(alpha);
            invalidate();
        }
    }

    /**
     * Obtains the ColorMatrix set to CircleImageView
     *
     * @return ColorMatrix set to CircleImageView
     */
    public ColorMatrix getColorFilter() {
        return mColorFilter;
    }

    /**
     * set color filter
     *
     * @param cf colormatrix
     */
    public void setColorFilter(ColorMatrix cf) {
        if (cf == mColorFilter) {
            return;
        }

        mColorFilter = cf;

        // This might be called during ImageView construction before
        // member initialization has finished on API level <= 19.
        if (mIsInitialized) {
            mPixelMapPaint.setColorMatrix(cf);
            invalidate();
        }
    }

    private Optional<PixelMap> getPixelMapFromDrawable(Element drawable) {
        if (drawable == null) {
            return Optional.empty();
        }

        if (drawable instanceof PixelMapElement) {
            return Optional.of(((PixelMapElement) drawable).getPixelMap());
        }

        try {
            PixelMap pixelMap;
            PixelMap.InitializationOptions initializationOptions = new PixelMap.InitializationOptions();

            initializationOptions.size = new Size(drawable.getBounds().getHeight(), drawable.getBounds().getWidth());
            initializationOptions.pixelFormat = PIXELMAP_CONFIG;
            pixelMap = PixelMap.create(initializationOptions);
            Canvas canvas = new Canvas();
            float radius = (pixelMap.getImageInfo().size.width) / DEFAULT_VALUE_TWO;
            canvas.drawPixelMapHolderCircleShape(new PixelMapHolder(pixelMap), pixelMapRect, 0, 0, radius);
            return Optional.of(pixelMap);
        } catch (IllegalArgumentException e) {
            return Optional.empty();
        }
    }

    private void initializePixelMap() {
        if (mPixelMap != null && mPixelMap.isEditable()) {
            mPixelMapCanvas = new Canvas();
            float radius = (mPixelMap.getImageInfo().size.width) / DEFAULT_VALUE_TWO;
            mPixelMapCanvas.drawPixelMapHolderCircleShape(new PixelMapHolder(mPixelMap), pixelMapRect, 0, 0, radius);
        } else {
            mPixelMapCanvas = null;
        }

        if (!mIsInitialized) {
            return;
        }

        if (mPixelMap != null) {
            updateShaderMatrix(); /* if commented, img does not come */
        } else {
            mPixelMapPaint.setShader(null, Paint.ShaderType.PIXELMAP_SHADER);
        }
    }

    private void updateDimensions() {
        mBorderRect.set(calculateBounds());
        mBorderRadius = Math.min((mBorderRect.height() - mBorderWidth) / DEFAULT_VALUE_TWO_FLOAT,
                (mBorderRect.width() - mBorderWidth) / DEFAULT_VALUE_TWO_FLOAT);

        mDrawableRect.set(mBorderRect);
        if (!mIsBorderOverlay && mBorderWidth > 0) {
            mDrawableRect.inset(mBorderWidth - 1, mBorderWidth - 1);
        }
        mDrawableRadius = Math.min(mDrawableRect.height() / DEFAULT_VALUE_TWO_FLOAT,
                mDrawableRect.width() / DEFAULT_VALUE_TWO_FLOAT);
        updateShaderMatrix();
    }

    private RectF calculateBounds() {
        int availableWidth = getWidth() - getPaddingLeft() - getPaddingRight();
        int availableHeight = getHeight() - getPaddingTop() - getPaddingBottom();
        int sideLength = Math.min(availableWidth, availableHeight);
        float left = getPaddingLeft() + (availableWidth - sideLength) / DEFAULT_VALUE_TWO_FLOAT;
        float top = getPaddingTop() + (availableHeight - sideLength) / DEFAULT_VALUE_TWO_FLOAT;
        return new RectF(left, top, left + sideLength, top + sideLength);
    }

    private void updateShaderMatrix() {
        if (mPixelMap == null) {
            return;
        }

        float scale;
        float dx = 0;
        float dy = 0;

        mShaderMatrix.setIdentity();
        int pixelMapHeight = mPixelMap.getImageInfo().size.height;
        int pixelMapWidth = mPixelMap.getImageInfo().size.width;

        if (pixelMapWidth * mDrawableRect.height() > mDrawableRect.width() * pixelMapHeight) {
            scale = mDrawableRect.height() / (float) pixelMapHeight;
            dx = (mDrawableRect.width() - pixelMapWidth * scale) * DEFAULT_VALUE_POINT_FIVE;
        } else {
            scale = mDrawableRect.width() / (float) pixelMapWidth;
            dy = (mDrawableRect.height() - pixelMapHeight * scale) * DEFAULT_VALUE_POINT_FIVE;
        }
        mShaderMatrix.setScale(scale, scale);
        mShaderMatrix.postTranslate((int) (dx + DEFAULT_VALUE_POINT_FIVE) + mDrawableRect.left,
                (int) (dy + DEFAULT_VALUE_POINT_FIVE) + mDrawableRect.top);
        mIsRebuildShader = true;
    }

    /**
     * RefreshListener updates when view is relayout
     */
    class RefreshListener implements LayoutRefreshedListener {
        @Override
        public void onRefreshed(Component component) {
            updateDimensions();
            invalidate();
        }
    }
}
