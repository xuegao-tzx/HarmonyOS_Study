package com.example.starbucks.widget.widget;

import com.example.starbucks.widget.controller.FormController;
import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.ability.ProviderFormInfo;
import ohos.aafwk.content.Intent;
import ohos.app.Context;

public class WidgetImpl extends FormController {

    public WidgetImpl(Context context, String formName, Integer dimension) {
        super(context, formName, dimension);
    }

    @Override
    public ProviderFormInfo bindFormData(long formId) {
        return null;
    }

    @Override
    public void updateFormData(long formId, Object... vars) {
    }

    @Override
    public void onTriggerFormEvent(long formId, String message) {
    }

    @Override
    public Class<? extends AbilitySlice> getRoutePageSlice(Intent intent) {
        return null;
    }
}
