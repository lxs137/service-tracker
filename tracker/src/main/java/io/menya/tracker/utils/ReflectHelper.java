package io.menya.tracker.utils;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.HashMap;

import org.apache.commons.lang3.ClassUtils;

public class ReflectHelper {

    private static final HashMap<String, Method> methodCache = new HashMap<>();
    /**
     * Calls an instance or static method of the given object.
     * The method is resolved using {@link #findMethodBestMatch(Class, String, Object...)}.
     *
     * @param obj The object instance. A class reference is not sufficient!
     * @param methodName The method name.
     * @param args The arguments for the method call.
     * @throws NoSuchMethodError In case no suitable method was found.
     */
    public static Object callMethod(Object obj, String methodName, Object... args) throws IllegalAccessException, InvocationTargetException {
        return findMethodBestMatch(obj.getClass(), methodName, args).invoke(obj, args);
    }

    /**
     * Calls an instance or static method of the given object.
     * See {@link #callMethod(Object, String, Object...)}.
     *
     * <p>This variant allows you to specify parameter types, which can help in case there are multiple
     * methods with the same name, especially if you call it with {@code null} parameters.
     */
    public static Object callMethod(Object obj, String methodName, Class<?>[] parameterTypes, Object... args) throws IllegalAccessException, InvocationTargetException {
        return findMethodBestMatch(obj.getClass(), methodName, parameterTypes, args).invoke(obj, args);
    }

    /**
     * Calls a static method of the given class.
     * The method is resolved using {@link #findMethodBestMatch(Class, String, Object...)}.
     *
     * @param clazz The class reference.
     * @param methodName The method name.
     * @param args The arguments for the method call.
     * @throws NoSuchMethodError In case no suitable method was found.
     */
    public static Object callStaticMethod(Class<?> clazz, String methodName, Object... args) throws IllegalAccessException, InvocationTargetException {
        return findMethodBestMatch(clazz, methodName, args).invoke(null, args);
    }

    /**
     * Calls a static method of the given class.
     * See {@link #callStaticMethod(Class, String, Object...)}.
     *
     * <p>This variant allows you to specify parameter types, which can help in case there are multiple
     * methods with the same name, especially if you call it with {@code null} parameters.
     */
    public static Object callStaticMethod(Class<?> clazz, String methodName, Class<?>[] parameterTypes, Object... args) throws IllegalAccessException, InvocationTargetException {
        return findMethodBestMatch(clazz, methodName, parameterTypes, args).invoke(null, args);
    }

    /**
     * Look up a method in a class and set it to accessible.
     *
     * <p>This does'nt only look for exact matches, but for the best match. All considered candidates
     * must be compatible with the given parameter types, i.e. the parameters must be assignable
     * to the method's formal parameters. Inherited methods are considered here.
     *
     * @param clazz The class which declares, inherits or overrides the method.
     * @param methodName The method name.
     * @param parameterTypes The types of the method's parameters.
     * @return A reference to the best-matching method.
     * @throws NoSuchMethodError In case no suitable method was found.
     */
    public static Method findMethodBestMatch(Class<?> clazz, String methodName, Class<?>... parameterTypes) {
        String fullMethodName = clazz.getName() + '#' + methodName + getParametersString(parameterTypes) + "#bestmatch";

        if (methodCache.containsKey(fullMethodName)) {
            Method method = methodCache.get(fullMethodName);
            if (method == null)
                throw new NoSuchMethodError(fullMethodName);
            return method;
        }

        try {
            Method method = findMethodExact(clazz, methodName, parameterTypes);
            methodCache.put(fullMethodName, method);
            return method;
        } catch (NoSuchMethodError ignored) {}

        Method bestMatch = null;
        Class<?> clz = clazz;
        boolean considerPrivateMethods = true;
        do {
            for (Method method : clz.getDeclaredMethods()) {
                // don't consider private methods of superclasses
                if (!considerPrivateMethods && Modifier.isPrivate(method.getModifiers()))
                    continue;

                // compare name and parameters
                if (method.getName().equals(methodName) && ClassUtils.isAssignable(parameterTypes, method.getParameterTypes(), true)) {
                    // get accessible version of method
                    if (bestMatch == null) {
                        bestMatch = method;
                    }
                }
            }
            considerPrivateMethods = false;
        } while ((clz = clz.getSuperclass()) != null);

        if (bestMatch != null) {
            bestMatch.setAccessible(true);
            methodCache.put(fullMethodName, bestMatch);
            return bestMatch;
        } else {
            NoSuchMethodError e = new NoSuchMethodError(fullMethodName);
            methodCache.put(fullMethodName, null);
            throw e;
        }
    }

    /**
     * Look up a method in a class and set it to accessible.
     *
     * <p>See {@link #findMethodBestMatch(Class, String, Class...)} for details. This variant
     * determines the parameter types from the classes of the given objects.
     */
    public static Method findMethodBestMatch(Class<?> clazz, String methodName, Object... args) {
        return findMethodBestMatch(clazz, methodName, getParameterTypes(args));
    }

    /**
     * Look up a method in a class and set it to accessible.
     *
     * <p>See {@link #findMethodBestMatch(Class, String, Class...)} for details. This variant
     * determines the parameter types from the classes of the given objects. For any item that is
     * {@code null}, the type is taken from {@code parameterTypes} instead.
     */
    public static Method findMethodBestMatch(Class<?> clazz, String methodName, Class<?>[] parameterTypes, Object[] args) {
        Class<?>[] argsClasses = null;
        for (int i = 0; i < parameterTypes.length; i++) {
            if (parameterTypes[i] != null)
                continue;
            if (argsClasses == null)
                argsClasses = getParameterTypes(args);
            parameterTypes[i] = argsClasses[i];
        }
        return findMethodBestMatch(clazz, methodName, parameterTypes);
    }

    public static Method findMethodExact(Class<?> clazz, String methodName, Class<?>... parameterTypes) {
        String fullMethodName = clazz.getName() + '#' + methodName + getParametersString(parameterTypes) + "#exact";

        if (methodCache.containsKey(fullMethodName)) {
            Method method = methodCache.get(fullMethodName);
            if (method == null)
                throw new NoSuchMethodError(fullMethodName);
            return method;
        }

        try {
            Method method = clazz.getDeclaredMethod(methodName, parameterTypes);
            method.setAccessible(true);
            methodCache.put(fullMethodName, method);
            return method;
        } catch (NoSuchMethodException e) {
            methodCache.put(fullMethodName, null);
            throw new NoSuchMethodError(fullMethodName);
        }
    }

    /**
     * Look up a class with the specified class loader.
     *
     * <p>There are various allowed syntaxes for the class name, but it's recommended to use one of
     * these:
     * <ul>
     *   <li>{@code java.lang.String}
     *   <li>{@code java.lang.String[]} (array)
     *   <li>{@code android.app.ActivityThread.ResourcesKey}
     *   <li>{@code android.app.ActivityThread$ResourcesKey}
     * </ul>
     *
     * @param className The class name in one of the formats mentioned above.
     * @param classLoader The class loader, or {@code null} for the boot class loader.
     * @return A reference to the class.
     * @throws ClassNotFoundException In case the class was not found.
     */
    public static Class<?> findClass(String className, ClassLoader classLoader) throws ClassNotFoundException {
        if (classLoader == null)
            classLoader = ClassLoader.getSystemClassLoader();
        return ClassUtils.getClass(classLoader, className, false);
    }

    /**
     * Returns an array with the classes of the given objects.
     */
    public static Class<?>[] getParameterTypes(Object... args) {
        if (args == null) {
            return new Class<?>[0];
        }
        Class<?>[] clazzes = new Class<?>[args.length];
        for (int i = 0; i < args.length; i++) {
            clazzes[i] = (args[i] != null) ? args[i].getClass() : null;
        }
        return clazzes;
    }

    /**
     * Retrieve classes from an array, where each element might either be a Class
     * already, or a String with the full class name.
     * @throws ClassNotFoundException In case the class was not found.
     */
    private static Class<?>[] getParameterClasses(ClassLoader classLoader, Object[] parameterTypesAndCallback) throws ClassNotFoundException {
        Class<?>[] parameterClasses = null;
        for (int i = parameterTypesAndCallback.length - 1; i >= 0; i--) {
            Object type = parameterTypesAndCallback[i];
            if (type == null)
                throw new ClassNotFoundException("parameter type must not be null", null);

            if (parameterClasses == null)
                parameterClasses = new Class<?>[i+1];

            if (type instanceof Class)
                parameterClasses[i] = (Class<?>) type;
            else if (type instanceof String)
                parameterClasses[i] = findClass((String) type, classLoader);
            else
                throw new ClassNotFoundException("parameter type must either be specified as Class or String", null);
        }

        // if there are no arguments for the method
        if (parameterClasses == null)
            parameterClasses = new Class<?>[0];

        return parameterClasses;
    }

    private static String getParametersString(Class<?>... clazzes) {
        StringBuilder sb = new StringBuilder("(");
        boolean first = true;
        for (Class<?> clazz : clazzes) {
            if (first)
                first = false;
            else
                sb.append(",");

            if (clazz != null)
                sb.append(clazz.getCanonicalName());
            else
                sb.append("null");
        }
        sb.append(")");
        return sb.toString();
    }

}
