package com.example.distschedule.rx2;

import com.alibaba.fastjson.JSONObject;
import com.example.distschedule.bean.RespondBase;
import com.example.distschedule.scheduler.HmOSSchedulers;
import com.example.distschedule.toast.XToastUtils;
import com.socks.library.KLog;
import io.reactivex.ObservableSource;
import io.reactivex.ObservableTransformer;
import io.reactivex.Observer;
import io.reactivex.disposables.Disposable;
import io.reactivex.schedulers.Schedulers;
import io.reactivex.subjects.Subject;
import ohos.app.Context;
import okhttp3.*;
import ren.yale.ohos.retrofitcachelibrx2.CacheInterceptorListener;
import ren.yale.ohos.retrofitcachelibrx2.RetrofitCache;
import ren.yale.ohos.retrofitcachelibrx2.intercept.CacheForceInterceptorNoNet;
import ren.yale.ohos.retrofitcachelibrx2.intercept.CacheInterceptorOnNet;
import ren.yale.ohos.retrofitcachelibrx2.transformer.CacheTransformer;
import retrofit2.Retrofit;
import retrofit2.adapter.rxjava2.RxJava2CallAdapterFactory;
import retrofit2.converter.gson.GsonConverterFactory;

import java.io.File;
import java.io.IOException;
import java.util.concurrent.TimeUnit;

/**
 * Created by Yale on 2017/6/12.
 */
public enum OKHttpUtilsRx2 {
    INSTANCE;
    private static Context mContext;
    private static ApiRx2 apiRx2;

    private static void showLog(String str1) {
        String str = str1;
        str = str.trim();
        int index = 0;
        int maxLength = 2000;
        String finalString = "";

        while (index < str.length()) {
            if (str.length() <= index + maxLength) {
                finalString = str.substring(index);
            } else {
                finalString = str.substring(index, index + maxLength);
            }
            index += maxLength;
            KLog.d(finalString.trim());
        }
    }

    public void init(Context context) {
        final String url = "http://81.71.17.188:8080/distschedule-api/";
        mContext = context;
        if (apiRx2 == null) {
            apiRx2 = configRetrofit(ApiRx2.class, url);
        }
        RetrofitCache.getInstance().init(context);
        RetrofitCache.getInstance().setCacheInterceptorListener(
                new CacheInterceptorListener() {
                    @Override
                    public boolean canCache(Request request, Response response) {
                        return true;
                    }
                });

    }

    public OkHttpClient getOkHttpClient() {
        OkHttpClient.Builder clientBuilder = new OkHttpClient.Builder();
        clientBuilder.readTimeout(20, TimeUnit.SECONDS);
        clientBuilder.connectTimeout(20, TimeUnit.SECONDS);
        clientBuilder.writeTimeout(20, TimeUnit.SECONDS);
        clientBuilder.addInterceptor(new LogInterceptor());
        clientBuilder.addInterceptor(new CacheForceInterceptorNoNet());
        clientBuilder.addNetworkInterceptor(new CacheInterceptorOnNet());

        int cacheSize = 200 * 1024 * 1024;
        File cacheDirectory = new File(mContext.getCacheDir(), "httpcache");
        Cache cache = new Cache(cacheDirectory, cacheSize);
        return clientBuilder.cache(cache).build();
    }

    public <T> ObservableTransformer<T, T> ioMain() {
        return new ObservableTransformer<T, T>() {
            @Override
            public ObservableSource<T> apply(io.reactivex.Observable<T> upstream) {
                return upstream.compose(CacheTransformer.<T>emptyTransformer()).
                        subscribeOn(Schedulers.io())
                        .observeOn(HmOSSchedulers.mainThread());
            }
        };
    }

    public ApiRx2 getApi() {
        return apiRx2;
    }

    private <T> T configRetrofit(Class<T> service, String url) {
        Retrofit retrofit = new Retrofit.Builder()
                .baseUrl(url)
                .client(getOkHttpClient())
                .addConverterFactory(GsonConverterFactory.create())
                .addCallAdapterFactory(RxJava2CallAdapterFactory.create())
                .build();
        RetrofitCache.getInstance().addRetrofit(retrofit);
        return retrofit.create(service);
    }

    public static class SubjectImpl<Object> extends Subject<Object> {

        @Override
        public boolean hasObservers() {
            return false;
        }

        @Override
        public boolean hasThrowable() {
            return false;
        }

        @Override
        public boolean hasComplete() {
            return false;
        }

        @Override
        public Throwable getThrowable() {
            return null;
        }

        @Override
        public void onSubscribe(Disposable disposable) {

        }

        @Override
        public void onNext(Object object) {
            if (object instanceof RespondBase && !((RespondBase) object).isSuccess()) {
                RespondBase respond = (RespondBase) object;
                KLog.e(respond.getMessage());
                XToastUtils.warning(respond.getMessage());
            }
        }

        @Override
        public void onError(Throwable throwable) {
            KLog.e(throwable.getMessage());
            XToastUtils.warning("onError：" + throwable.getMessage());
        }

        @Override
        public void onComplete() {

        }


        @Override
        protected void subscribeActual(Observer<? super Object> observer) {

        }
    }

    private final class LogInterceptor implements Interceptor {
        private LogInterceptor() {

        }

        public Response intercept(Chain chain) throws IOException {
            Request request = chain.request();
            StringBuffer sb = new StringBuffer();


            Response response = chain.proceed(chain.request());
            okhttp3.MediaType mediaType = response.body().contentType();
            String content = response.body().string();

            sb.append("======== request: " + request.toString() + "\r\n " +
                    "======== request headers: " + request.headers().toString() + "\r\n" +
                    "======= response header:" + response.headers().toString() + "\r\n" +
                    "---------- response body:\r\n");
            KLog.d(sb.toString());
            KLog.d(JSONObject.toJSONString(response.body()));
            try {
                showLog(content);
            } catch (Exception e) {
                KLog.d(e.toString());
            }

            return response.newBuilder()
                    .body(okhttp3.ResponseBody.create(mediaType, content))
                    .build();
        }
    }
}
