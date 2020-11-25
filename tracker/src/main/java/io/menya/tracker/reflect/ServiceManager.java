package io.menya.tracker.reflect;

import android.os.IBinder;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;

import io.menya.tracker.utils.Log;
import io.menya.tracker.utils.ReflectHelper;

public class ServiceManager {
    private final static String TAG = "ServiceManager";
    private static Object managerBinderRef;

    // frameworks_base/core/java/android/os/ServiceManager.java:getServiceManager()
    public static void init() {
        try {
            Class<?> serviceManagerNativeClass = Class.forName("android.os.ServiceManagerNative", false, ClassLoader.getSystemClassLoader());
            Class<?> binderInternalClass = Class.forName("com.android.internal.os.BinderInternal", false, ClassLoader.getSystemClassLoader());
            Object context = ReflectHelper.callStaticMethod(binderInternalClass, "getContextObject", null);
            Class<?>[] asInterfaceArgsClazz = new Class[1];
            asInterfaceArgsClazz[0] = IBinder.class;
            managerBinderRef = ReflectHelper.callStaticMethod(serviceManagerNativeClass, "asInterface", asInterfaceArgsClazz, context);
        } catch (ClassNotFoundException | IllegalAccessException | InvocationTargetException e) {
            Log.fault(TAG, e,"InitServiceManagerIBinder");
        }
    }

    public static List<ServiceInfo> parseAllServiceInfo() {
        List<ServiceInfo> metadataList = new ArrayList<>();
        try {
            String[] serviceStrs = (String[]) ReflectHelper.callMethod(managerBinderRef, "listServices", null);
            for (String interfaceName : serviceStrs){
                metadataList.add(parseServiceInfo(interfaceName));
            }
        } catch (IllegalAccessException | InvocationTargetException e) {
            Log.error(TAG, e, "ServiceManager listService() error");
        }
        return metadataList;
    }

    private static ServiceInfo parseServiceInfo(String serviceName) {
        IBinder service = findServiceAtServiceManager(serviceName);
        if (service == null) {
            Log.error(TAG, null, "service '%s' not registered", serviceName);
            return null;
        }

        ServiceInfo serviceInfo = new ServiceInfo();
        serviceInfo.name = serviceName;
        Class<?> serviceInterface = null;
        try {
            serviceInfo.interfaceDescriptor = (String) ReflectHelper.callMethod(service, "getInterfaceDescriptor", null);
            serviceInterface = Class.forName(serviceInfo.interfaceDescriptor);
        } catch (IllegalAccessException | InvocationTargetException | ClassNotFoundException e) {
            Log.error(TAG, null, "service '%s(%s)' may impl in C++", serviceName, serviceInfo.interfaceDescriptor);
        }

        if (serviceInfo.interfaceDescriptor != null && serviceInterface != null) {
            Log.info(TAG,"service '%s(%s)' impl in Java", serviceName, serviceInfo.interfaceDescriptor);
            serviceInfo.type = ServiceImplType.Java;
            List<ServiceInfo.MethodInfo> methods = new ArrayList<>();
            for (Method m : serviceInterface.getDeclaredMethods()) {
                ServiceInfo.MethodInfo methodInfo = new ServiceInfo.MethodInfo();
                methodInfo.name = m.getName();
                List<String> paramList = new ArrayList<>();
                for (Class<?> p: m.getParameterTypes()){
                    paramList.add(p.getName());
                }
                methodInfo.paramTypes = paramList.toArray(new String[0]);
                methods.add(methodInfo);
            }
            serviceInfo.methods = methods.toArray(new ServiceInfo.MethodInfo[0]);
        } else {
            serviceInfo.type = ServiceImplType.Native;
        }
        return serviceInfo;
    }

    private static IBinder findServiceAtServiceManager(String serviceName) {
        Class<?>[] getServiceArgsClazz = new Class[1];
        getServiceArgsClazz[0] = String.class;
        IBinder iBinder = null;
        try {
            iBinder = (IBinder) ReflectHelper.callMethod(managerBinderRef, "getService", getServiceArgsClazz, serviceName);
        } catch (IllegalAccessException | InvocationTargetException ignored) {}
        return iBinder;
    }
}
