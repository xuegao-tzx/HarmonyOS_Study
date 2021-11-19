package com.example.distschedule.scheduler;

import io.reactivex.Scheduler;
import ohos.eventhandler.EventHandler;
import ohos.eventhandler.EventRunner;

/**
 * HmOs-specific Schedulers.
 */
public final class HmOSSchedulers {


    private static final Scheduler MAIN_THREAD = RxHmOSPlugins.initMainThreadScheduler(
            () -> MainHolder.DEFAULT);

    private HmOSSchedulers() {
        throw new AssertionError("No instances.");
    }

    public static Scheduler mainThread() {
        return RxHmOSPlugins.onMainThreadScheduler(MAIN_THREAD);
    }

    public static Scheduler from(EventRunner eventRunner) {
        if (eventRunner == null) {
            return null;
        }
        return new HandlerScheduler(new EventHandler(eventRunner));
    }

    private static final class MainHolder {

        static final Scheduler DEFAULT = new HandlerScheduler(new EventHandler(EventRunner.getMainEventRunner()));
    }
}