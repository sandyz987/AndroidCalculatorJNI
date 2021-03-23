package com.example.CalculatorJNI.jni;

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
    public static native void formatExpression();
    @Synchronized
    public static native void eval();
    @Synchronized
    public static native void clearVariable();
    @Synchronized
    public static native double getAns();
    @Synchronized
    public static native void setExpression(String expression);
    @Synchronized
    public static native void setVariable(char c, double value);
}
