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

package com.example.distschedule.database;

import com.j256.ormlite.dao.Dao;
import com.j256.ormlite.hmos.apptools.OrmLiteRdbOpenHelper;
import com.j256.ormlite.support.ConnectionSource;
import com.j256.ormlite.table.TableUtils;
import com.socks.library.KLog;
import ohos.app.Context;
import ohos.data.DatabaseHelper;
import ohos.data.orm.OrmContext;
import ohos.data.rdb.RdbStore;

import java.sql.SQLException;
import java.util.HashMap;
import java.util.Map;

/**
 * Function description
 * Dbhelper extend OrmLiteRdbOpenHelper
 */
public class Dbhelper extends OrmLiteRdbOpenHelper {
    /**
     * DB_VERSION
     */
    public static final int DB_VERSION = 1;

    /**
     * DB_NAME
     */
    public static final String DB_NAME = "DistSchedule.db";

    /**
     * DB_NAME_ALIAS
     */
    public static final String DB_NAME_ALIAS = "DistSchedule database";

    /**
     * TAG
     */
    private static final String TAG = "DistSchedule_log";

    /**
     * instance
     */
    private static Dbhelper instance;

    /**
     * dbHelper
     */
    private DatabaseHelper dbHelper = null;

    /**
     * rdb
     */
    private RdbStore rdb = null;

    /**
     * ormContext
     */
    private OrmContext ormContext = null;

    /**
     * daos
     */
    private Map<String, Dao> daos = new HashMap<>();

    /**
     * Dbhelper File method
     *
     * @param context Context
     */
    public Dbhelper(Context context) {
        super(context, DB_NAME, DB_NAME_ALIAS, DB_VERSION);
    }

    /**
     * Dbhelper File method
     *
     * @param context Context
     * @return dao
     */
    public static synchronized Dbhelper getInstance(Context context) {
        if (instance == null) {
            synchronized (Dbhelper.class) {
                if (instance == null) {
                    instance = new Dbhelper(context);
                }
            }
        }
        return instance;
    }

    /**
     * get File method
     *
     * @param clazz Class
     * @return dao
     * @throws SQLException Exception
     */
    public synchronized Dao getDao(Class clazz) throws SQLException {
        Dao dao = null;
        String className = clazz.getSimpleName();
        if (daos.containsKey(className)) {
            dao = daos.get(className);
        }
        if (dao == null) {
            dao = super.getDao(clazz);
            daos.put(className, dao);
        }
        return dao;
    }

    @Override
    public void onCreate(RdbStore database, ConnectionSource connectionSource) {
        try {
            TableUtils.createTableIfNotExists(connectionSource, AlarmBean.class);
        } catch (SQLException e) {
            KLog.i(e.getMessage());
        }
    }

    @Override
    public void onUpgrade(RdbStore database, ConnectionSource connectionSource, int oldVersion, int newVersion) {
    }
}