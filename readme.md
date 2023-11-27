# Android 字符串求值工具类（科学计算）
###### 在Android数学计算APP，经常有场景让用户输入一串表达式，并且计算其值，如果手动用 Integer.parseInt 不太好用而且还需要 try。于是写了这个库。



#### 功能：接受一个字符串String表达式，返回表达式计算结果：double类型数字；



## 1、依赖 

build.gradle 加入：

```css
	allprojects {
		repositories {
			...
			maven { url 'https://jitpack.io' }
		}
	}
```

```css
	dependencies {
	        implementation 'com.github.sandyz987:CalculatorJNI:1.1.0'
	}
```





## 2、使用

**如果表达式错误，不会报错而是输出一个错误的值！请知晓。后续版本会更新。表达式中仅支持小写字符。**

![image-20210329174146009](C:\Users\lenovo\AppData\Roaming\Typora\typora-user-images\image-20210329174146009.png)



#### 直接计算表达式的值：

```kotlin
var calculator: Calculator? = Calculator() // 新建计算器实例

// calculator?.setExpression("1+2*3") // 设置常规表达式

calculator?.setExpression("if(-1,1==2|1!=2,sin(-1.57))") // 设置带函数的表达式

println(calculator?.getAns()) // 执行计算，获得计算结果

calculator?.destroy() // 因为底层是c++实现，所以防止内存泄露要执行destroy()

calculator = null // 已经destroy的calculator不能再使用，置为null以便jvm回收
```

上面的表达式中，if( 三个参数 )，第一个参数是条件，如果非零，整个if语句的值就是第二个参数的计算结果，否则为第三个表达式计算的结果。

可见-1!=0最终表达式计算的结果为：第二个参数的结果。为1（条件表达式的结果为真则返回1）。



#### 在表达式中使用变量：

```kotlin
var sum = 0.0

var calculator: Calculator? = Calculator() // 新建计算器实例

calculator?.setExpression("if(i<=500,sin(i),max(750,i)") // 设置要计算的表达式，包含可变变量‘i’，默认值为0，计算时请设置变量值

for (i in 1..1000) {
      calculator?.setVariable('i', i.toDouble()) // 设置表达式中i的值为 for循环i的值
      val ans = calculator?.getAns() // 执行计算
      sum += ans?: 0.0
}

println(sum)

calculator?.destroy()
```

变量仅支持a-z的字符。

setExpression方法会进行对字符串的预处理操作，此方法效率低下，避免在循环中使用。getAns方法效率较高。



## 3、支持的函数
#### 逻辑表达式(运算符)：

> 当一个逻辑表达式成立时，表达式的值为1，如“2==2|1=-1”的值是1，否则为零。
>
> 当一个表达式的结果为零，认为表达式为假，否则为真

​	>,  <,  == (注意是双等于号),  <=  ,>=  ,& (and)  ,| (or)  ,~ (not)  
#### 数学运算符：
​	+,  -,  *,  /,  % (求模),  ^ (乘方),  ! (阶乘),  # (负号，一般会自动区分负号和减号，无需用户输入井号)

#### 符号：
​	括号（改变计算顺序），逗号 （用于分割函数参数）
#### 函数：
​	**sin(x)**

​		三角函数 sin(x) cos(x) tan(x) asin(x) acos(x) atan(x)
​		如果不在定义域则返回0

​	**rand(x,y)**

​		随机数 rand(x,y)(返回[x,y]间的整数，需要srand)

​	**deg(x)**

​		弧度转角度 deg(x) 角度转弧度 rad(x)

​	**if(条件,x,y)**

​		条件表达式 if(条件,x,y),其中当x非0，则返回表达式x的值，否则返回表达式y的值

​	**log(x,y)**

​		对数 log(x,y)返回以x为底y的对数  ln(x)返回以e为底的自然对数

​	**exp(x)**

​		自然数的n次方 exp(x),返回e^x

​	**max(x,y)**

​		最大最小值 max(x,y)返回表达式x和表达式y的较大的一个，min相反

​	**sign(x)**

​		取符号 sign(x) 如果x大于等于0，返回1，否则返回-1

​	**round(x)**

​		四舍五入 round(x)   向下取整 floor(x)

​	**abs(x)**

​		绝对值 abs(x) 返回x的绝对值

​	**sqrt(x)**

​		开平方 sqrt(x) 返回x开平方的值
​		开n次方可以用运算符“ ^ ”代替



## 4、源码思路
​	这里我用例子解释。
​	假如用户输入“-sin(12.57)-cos(-6)”
​	规定：如果减号“-”出现在符号后面（不包括右括号）或者出现在字符串开头，就替换为负号“#”，负号是一元运算符，减号是二元运算符，所以有必要区分。
​	故原字符串被替换为“#sin(12.57)-cos(#6)”
​	由于sin，cos是多字符不好处理，则替换为单字符（这里我用A-Z）,故替换为“#J(12.57)-K(#6)”
​	接下来只需要区分是符号还是数字即可。

>读入#是符号
>读入J是符号
>读入(是符号（以上都是转后缀表达式的标准操作，不再赘述）
>读入1是数字，不确定是不是一个完整的数字，则存在number里，
>读入2是数字，将原来的number\*10+2。
>读入.表明接下来的数字是小数部分了。
>读入5，number+=pow(10,-1)\*5
>读入7，number+=pow(10,-2)\*7
>当读入符号或者到字符串末尾时，讲存的数字保存下来，并且清空。
>以此类推

​	这样子符号和数字就被区分开了，并且转为后缀表达式了。
​	接下来就只需要运行后缀表达式的求值和控制好优先级了。
​	因为没检测表达式是否符合规范，以下的写法也能计算出正确结果：

>sin7
>7sin
>(7)sin

#### 暂时未写语法分析，如果用户输入错误的表达式，则计算结果可能会有错误，表达式正确则结果正确。



## 5、赞助

​	它花费了我许多的工作之余的时间！如果它帮助到您了的话，欢迎请我喝杯咖啡：）

​	[二维码]

