package com.example.calculatorjni.jni;

import kotlin.jvm.Synchronized;

/**
 * @author zhangzhe
 * @date 2021/3/22
 * @description
 */

public class CalculatorJNI {

    static {
        System.loadLibrary("eval");
    }

    @Synchronized
    protected static native int newInstance();

    @Synchronized
    protected static native void destroyInstance(int instanceId);

    @Synchronized
    protected static native void formatExpression(int instanceId);

    @Synchronized
    protected static native void eval(int instanceId);

    @Synchronized
    protected static native void clearVariable(int instanceId);

    @Synchronized
    protected static native double getAns(int instanceId);

    @Synchronized
    protected static native void setExpression(int instanceId, String expression);

    @Synchronized
    protected static native void setVariable(int instanceId, char c, double value);
}
