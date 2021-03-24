package com.example.calculatorjni

import android.annotation.SuppressLint
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.example.calculatorjni.jni.Calculator
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)



        button.setOnClickListener {
            val c = Calculator()
            Thread {
                et_exp.text.toString().let {
                    it.removeSuffix(" ")
                    if (it.isBlank()) {
                        c.setExpression("0")
                    } else {
                        c.setExpression(it)
                    }
                }

                var ans = 0.0

                try {

                    for (i in et_x.text.toString().toInt()..et_y.text.toString().toInt()) {
                        c.clearVariable()
                        c.setVariable('x', i.toDouble())
                        ans += c.getAns()
                    }


                } catch (e: Exception) {
                    e.printStackTrace()
                } finally {
                    c.destroy()
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