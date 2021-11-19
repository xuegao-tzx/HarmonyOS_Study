/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
package com.example.distschedule.config;


import org.apache.http.HttpEntityEnclosingRequest;
import org.apache.http.HttpRequest;
import org.apache.http.NoHttpResponseException;
import org.apache.http.client.HttpRequestRetryHandler;
import org.apache.http.client.config.RequestConfig;
import org.apache.http.client.protocol.HttpClientContext;
import org.apache.http.config.Registry;
import org.apache.http.config.RegistryBuilder;
import org.apache.http.conn.socket.ConnectionSocketFactory;
import org.apache.http.conn.socket.PlainConnectionSocketFactory;
import org.apache.http.conn.ssl.NoopHostnameVerifier;
import org.apache.http.conn.ssl.SSLConnectionSocketFactory;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.impl.client.HttpClients;
import org.apache.http.impl.client.StandardHttpRequestRetryHandler;
import org.apache.http.impl.conn.PoolingHttpClientConnectionManager;
import org.apache.http.ssl.SSLContextBuilder;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.http.MediaType;
import org.springframework.http.client.ClientHttpRequestFactory;
import org.springframework.http.client.HttpComponentsClientHttpRequestFactory;
import org.springframework.http.converter.json.MappingJackson2HttpMessageConverter;
import org.springframework.web.client.RestTemplate;

import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLException;
import javax.net.ssl.SSLHandshakeException;
import java.io.InterruptedIOException;
import java.net.UnknownHostException;
import java.security.GeneralSecurityException;
import java.util.ArrayList;
import java.util.List;

@Configuration
public class RestClient {

    @Value("${service.maxTotal:2000}")
    private int maxTotal = 2000;

    @Value("${service.defaultMaxPerRoute:400}")
    private int defaultMaxPerRoute = 400;

    private static final Logger logger = LoggerFactory.getLogger(RestClient.class);

    // 默认连接超时，10秒
    @Value("${service.connectionTimeout:300000}")
    private int CONNECT_TIMEOUT = 300000;

    // 默认读取数据超时，修改为120秒
    @Value("${service.readTimeout:12000000}")
    private int READ_TIMEOUT = 120000;

    @Bean(name = "httpsRestTemplate")
    public RestTemplate serviceHttpsRestTemplate() {
        RestTemplate restTemplate = new RestTemplate(clientHttpRequestFactory(true));
        restTemplate.getMessageConverters().add(new WxMappingJackson2HttpMessageConverter());
        return restTemplate;
    }

    @Bean(name = "httpRestTemplate")
    public RestTemplate serviceHttpRestTemplate() {
        return new RestTemplate(clientHttpRequestFactory(false));
    }

    private class WxMappingJackson2HttpMessageConverter extends MappingJackson2HttpMessageConverter {
        WxMappingJackson2HttpMessageConverter() {
            List<MediaType> mediaTypes = new ArrayList<>();
            mediaTypes.add(MediaType.APPLICATION_JSON_UTF8);
            setSupportedMediaTypes(mediaTypes);
        }
    }

    private ClientHttpRequestFactory clientHttpRequestFactory(boolean useHttps) {
        HttpComponentsClientHttpRequestFactory factory = new HttpComponentsClientHttpRequestFactory();
        factory.setReadTimeout(READ_TIMEOUT);
        factory.setConnectTimeout(CONNECT_TIMEOUT);

        if (useHttps) {
            factory.setHttpClient(createHttpsClient());
        }
        return factory;
    }

    private CloseableHttpClient createHttpsClient() {
        try {
            SSLContext sslContext = new SSLContextBuilder().loadTrustMaterial(null, (chain, authType) -> true).build();

            SSLConnectionSocketFactory sslsf =
                    new SSLConnectionSocketFactory(sslContext, new String[]{"TLSv1.2", "TLSv1.1", "TLSv1", "SSLv3"}, null, NoopHostnameVerifier.INSTANCE);

            Registry<ConnectionSocketFactory> socketFactoryRegistry = RegistryBuilder.<ConnectionSocketFactory>create()
                    .register("http", PlainConnectionSocketFactory.getSocketFactory()).register("https", sslsf).build();

            PoolingHttpClientConnectionManager pccm = new PoolingHttpClientConnectionManager(socketFactoryRegistry);
            logger.info("set maxTotal:" + this.maxTotal);
            logger.info("set defaultMaxPerRoute:" + this.defaultMaxPerRoute);
            // 连接池最大并发连接数
            pccm.setMaxTotal(this.maxTotal);
            // 单路由最大并发数
            pccm.setDefaultMaxPerRoute(this.defaultMaxPerRoute);
            pccm.setValidateAfterInactivity(2000);
            RequestConfig params = RequestConfig.custom().setConnectTimeout(CONNECT_TIMEOUT)
                    .setConnectionRequestTimeout(5000).setSocketTimeout(READ_TIMEOUT).build();

            HttpRequestRetryHandler httpRequestRetryHandler = (exception, executionCount, context) -> {

                // 如果已经重试了3次，就放弃
                if (executionCount >= 1) {
                    return false;
                }
                // 如果服务器丢掉了连接，那么就重试
                if (exception instanceof NoHttpResponseException) {
                    return true;
                }
                // 不要重试SSL握手异常
                if (exception instanceof SSLHandshakeException) {
                    return false;
                }
                // 超时
                if (exception instanceof InterruptedIOException) {
                    return false;
                }
                // 目标服务器不可达
                if (exception instanceof UnknownHostException) {
                    return false;
                }
                // ssl握手异常
                if (exception instanceof SSLException) {
                    return false;
                }

                HttpClientContext clientContext = HttpClientContext.adapt(context);
                HttpRequest request = clientContext.getRequest();
                // 如果请求是幂等的，就再次尝试
                return !(request instanceof HttpEntityEnclosingRequest);
            };
            StandardHttpRequestRetryHandler standardHttpRequestRetryHandler = new StandardHttpRequestRetryHandler();
            return HttpClients.custom().setConnectionManager(pccm).setDefaultRequestConfig(params)
                    .setRetryHandler(standardHttpRequestRetryHandler).setConnectionManagerShared(true).build();
        } catch (GeneralSecurityException e) {
            throw new RuntimeException("create https client error.", e);
        }
    }
}
