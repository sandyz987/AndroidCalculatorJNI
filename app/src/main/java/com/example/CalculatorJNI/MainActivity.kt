package com.example.CalculatorJNI

import android.annotation.SuppressLint
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.example.CalculatorJNI.jni.CalculatorJNI
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)



        button.setOnClickListener {
            Thread {
                et_exp.text.toString().let {
                    it.removeSuffix(" ")
                    if (it.isBlank()) {
                        CalculatorJNI.setExpression("0")
                    } else {
                        CalculatorJNI.setExpression(it)
                    }
                }

                CalculatorJNI.formatExpression()
                CalculatorJNI.eval()
                var ans = 0.0
                try {
                    CalculatorJNI.setVariable('x', et_x.text.toString().toDouble())
                    CalculatorJNI.setVariable('y', et_y.text.toString().toDouble())
                    CalculatorJNI.clearVariable()
                    ans = CalculatorJNI.getAns()
                } catch (e: Exception) {
                    e.printStackTrace()
                }


                runOnUiThread {
                    done(ans)
                }
            }.start()
        }

    }

    @SuppressLint("SetTextI18n")
    fun done(ans: Double) {
        tv.text = "$ans"
    }

}