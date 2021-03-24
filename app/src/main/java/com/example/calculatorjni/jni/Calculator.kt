package com.example.calculatorjni.jni

/**
 *@author zhangzhe
 *@date 2021/3/24
 *@description
 */

class Calculator {
    private var instanceId = 0
    private var _isDestroy = false
    val isDestroy get() = _isDestroy

    init {
        instanceId = CalculatorJNI.newInstance()
    }


    private fun checkDestroy() {
        if (isDestroy) {
            throw IllegalStateException("the Calculator instance has destroyed!")
        }
    }


    private fun formatExpression() {
        checkDestroy()
        CalculatorJNI.formatExpression(instanceId)
    }

    private fun eval() {
        checkDestroy()
        CalculatorJNI.eval(instanceId)
    }

    /**
     * you should call @{destroy()} after used this instance.
     */
    fun destroy() {
        CalculatorJNI.destroyInstance(instanceId)
        _isDestroy = true
    }


    fun setExpression(expression: String) {
        checkDestroy()
        expression.removeSuffix(" ")
        if (expression.length >= 1000)
            throw IllegalStateException("expression's length is too long! length: ${expression.length}")
        CalculatorJNI.setExpression(instanceId, expression)
        formatExpression()
        eval()
    }

    fun clearVariable() {
        checkDestroy()
        CalculatorJNI.clearVariable(instanceId)
    }

    fun getAns(): Double {
        checkDestroy()
        return CalculatorJNI.getAns(instanceId)
    }

    fun setVariable(variable: Char, value: Double) {
        checkDestroy()
        CalculatorJNI.setVariable(instanceId, variable, value)
    }

}