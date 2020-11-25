package io.menya.tracker.reflect;

import java.util.Arrays;

public class ServiceInfo {
    public final static class MethodInfo {
        public String name;
        public String[] paramTypes;

        @Override
        public String toString() {
            return "{methodName='" + name + '\'' +
                    ", paramTypes=" + Arrays.toString(paramTypes) +
                    '}';
        }
    }

    public String name;
    public String interfaceDescriptor;
    public ServiceImplType type;
    public MethodInfo[] methods;

    @Override
    public String toString() {
        return "  ServiceMetadata{" +
                "name='" + name + '\'' +
                ", interfaceDescriptor='" + interfaceDescriptor + '\'' +
                ", type='" + type.toString() + '\'' +
                ", methods=" + Arrays.toString(methods) +
                '}';
    }
}
