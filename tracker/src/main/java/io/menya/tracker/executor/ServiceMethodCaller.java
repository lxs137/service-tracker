package io.menya.tracker.executor;

import android.app.Activity;
import android.app.ActivityManager;

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import io.menya.tracker.reflect.ServiceManagerHelper;
import io.menya.tracker.utils.Log;

public class ServiceMethodCaller {
    private final static String TAG = "MethodCaller";
    private final ExecutorService executor;
    private final Object serviceInstance;
    private final Method method;

    public static ServiceMethodCaller buildFromName(String serviceName, String methodName) {
        Object serviceInstance;
        try {
            serviceInstance = ServiceManagerHelper.getServiceStub(serviceName);
        } catch (Exception e) {
            Log.error(TAG, e, "service %s not found in ServiceManager or not impl in Java", serviceName);
            return null;
        }

        for (Method m : serviceInstance.getClass().getDeclaredMethods()) {
            if (m.getName().equals(methodName)) {
                return new ServiceMethodCaller(serviceInstance, m);
            }
        }
        Log.info(TAG, "service %s does not have method %s", serviceName, methodName);
        return null;
    }

    ServiceMethodCaller(Object serviceInstance, Method method) {
        this.serviceInstance = serviceInstance;
        this.method = method;
        this.executor = Executors.newFixedThreadPool(1);
    }

    public Class[] getParameterTypes(){
        return this.method.getParameterTypes();
    }

    public Object callWithTimeout(long timeoutSec, Object...effectiveArgs) throws Exception {
        if (effectiveArgs == null && method.getParameterTypes().length != 0){
            effectiveArgs = new Object[]{null};
        }

        Future<Object> future = executor.submit(new MethodCall(method, serviceInstance, effectiveArgs));

        try {
            Log.info(TAG, "invoking method: " + method.getName());
            Object result = future.get(timeoutSec, TimeUnit.SECONDS);
            Log.info(TAG, "returned from method: " + method.getName());
            return result;
        } catch (TimeoutException | InterruptedException | ExecutionException e) {
            future.cancel(true);
            Log.error(TAG, e, "cancelling method: " + method.getName());
            throw e;
        }
    }

    public Object callWithoutArgs(long timeoutSec) throws Exception {
        Future<Object> future = executor.submit(new MethodCall(method, serviceInstance, new Object[]{}));
        try {
            Log.info(TAG, "invoking method: " + method.getName());
            Object result = future.get(timeoutSec, TimeUnit.SECONDS);
            Log.info(TAG, "returned from method: " + method.getName());
            return result;
        } catch (TimeoutException | InterruptedException | ExecutionException e) {
            future.cancel(true);
            Log.error(TAG, e, "cancelling method: " + method.getName());
            throw e;
        }
    }

    public static class MethodCall implements Callable<Object> {
        private final Method method;
        private final Object calledObject;
        private final Object[] effectiveArguments;

        public MethodCall(Method method, Object calledObject, Object[] effectiveArguments) {
            this.method = method;
            this.calledObject = calledObject;
            this.effectiveArguments = effectiveArguments;
        }

        @Override
        public Object call() throws Exception {
            return method.invoke(calledObject, effectiveArguments);
        }
    }
}
