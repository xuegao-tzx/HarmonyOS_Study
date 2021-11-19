package com.example.distschedule.scheduler;

import com.socks.library.KLog;
import io.reactivex.Scheduler;
import io.reactivex.disposables.Disposable;
import io.reactivex.disposables.Disposables;
import io.reactivex.plugins.RxJavaPlugins;
import ohos.eventhandler.EventHandler;
import ohos.eventhandler.InnerEvent;

import java.util.concurrent.TimeUnit;

final class HandlerScheduler extends Scheduler {
    private static int innerEventID = 9819;
    private final EventHandler mHandler;

    HandlerScheduler(EventHandler handler) {
        this.mHandler = handler;
    }

    @Override
    public Disposable scheduleDirect(Runnable run, long delay, TimeUnit unit) {
        Runnable run1 = run;
        if (run1 == null) {
            KLog.d("run == null");
            return null;
        }
        if (unit == null) {
            KLog.d("unit == null");
            return null;
        }

        run1 = RxJavaPlugins.onSchedule(run1);
        ScheduledRunnable scheduled = new ScheduledRunnable(mHandler, run1);
        KLog.d("Scheduler=scheduleDirect");
        mHandler.postTask(scheduled, unit.toMillis(delay));
        return scheduled;
    }

    @Override
    public Worker createWorker() {
        return new HandlerWorker(mHandler);
    }

    private static final class HandlerWorker extends Worker {
        private final EventHandler mHandler1;

        private volatile boolean disposed;

        HandlerWorker(EventHandler handler) {
            this.mHandler1 = handler;
        }

        @Override
        public Disposable schedule(Runnable run, long delay, TimeUnit unit) {
            Runnable run1 = run;
            if (run1 == null) {
                KLog.d("run == null");
                return null;
            }
            if (unit == null) {
                KLog.d("unit == null");
                return null;
            }

            KLog.d("Scheduler=1");
            if (disposed) {
                return Disposables.disposed();
            }

            run1 = RxJavaPlugins.onSchedule(run1);

            ScheduledRunnable scheduled = new ScheduledRunnable(mHandler1, run1);

            InnerEvent.get(innerEventID, 0, null);


            KLog.d("Scheduler=2=" + unit.toMillis(delay));
            mHandler1.postTask(scheduled, unit.toMillis(delay));

            // Re-check disposed state for removing in case we were racing a call to dispose().
            if (disposed) {
                KLog.d("Scheduler=3");
                //不能remove Runnable
                mHandler1.removeAllEvent();
                return Disposables.disposed();
            }

            return scheduled;
        }

        @Override
        public void dispose() {
            disposed = true;
            KLog.d("Scheduler=dispose");

            //不能remove token
            mHandler1.removeAllEvent();
        }

        @Override
        public boolean isDisposed() {
            return disposed;
        }
    }

    private static final class ScheduledRunnable implements Runnable, Disposable {
        private final EventHandler mHandler2;
        private final Runnable mDelegate;

        private volatile boolean disposed;

        ScheduledRunnable(EventHandler handler, Runnable delegate) {
            this.mHandler2 = handler;
            this.mDelegate = delegate;
        }

        @Override
        public void run() {
            try {
                KLog.d("Scheduler=delegate.run");
                mDelegate.run();
            } catch (Exception e) {
                RxJavaPlugins.onError(e);
            }
        }

        @Override
        public void dispose() {
            disposed = true;
            KLog.d("Scheduler=delegate.dispose");
            //不能remove Runnable
            mHandler2.removeAllEvent();
        }

        @Override
        public boolean isDisposed() {
            return disposed;
        }
    }
}