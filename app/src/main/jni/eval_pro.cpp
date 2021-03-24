#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <jni.h>
#include <map>
#include "utils.h"

extern "C" char *jstringToChar(JNIEnv *env, jstring jstr);
/**
 * Developed by sandyz987
 * Function 'eval'
 * @param Expression string (chars' length <= MAX_SIZE)
 * @return Answer : double
 * @isError 0:no error  1:wrong number of decimal points  2:can't get top item at an empty stack  3:can't pop item at an empty stack(number of brackets is invalid?)
 *          4:can't get priority   5:too many arguments   6:unexpected character   7:wrong number of arguments   8:math error
 */

#define PI 3.141592653
#define MAX_SIZE 1000
#define MAX_OPERA_NUM 26
const double allowance = 1.0e-1;
const char *functionName[MAX_OPERA_NUM] = {">=", "<=", "!=", "==", ">", "<", "asin", "acos", "atan",
                                           "sin",
                                           "cos", "tan", "rand", "deg", "if", "rad", "log", "ln",
                                           "exp",
                                           "min", "max", "sign", "round", "floor", "abs", "sqrt"};
const char nameTran[MAX_OPERA_NUM] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
                                      'M', 'N',
                                      'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};


const int nameArgNum[MAX_OPERA_NUM] = {2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 1, 3, 1, 2, 1, 1, 2,
                                       2, 1, 1,
                                       1, 1, 1};//Number of arguments
const char operators[] = {'+', '-', '*', '/', '^', '!', '%', '(', ')', ',', '&', '|', '~', '#'};
const int priority[] = {3, 3, 4, 4, 5, 5, 5, -4, -5, -1, 1, 0, 5, 7};//Operator' priority
const int operatorArgNum[] = {2, 2, 2, 2, 2, 1, 2, 0, 0, 1, 2, 2, 1, 1};//Number of arguments

typedef struct {
    int isOperator;//If isOperator == 0 use the num, else use the opera
    double num;
    char opera;
} SIGN;


class Calculator {
public:
    char input[1000] = {0};

    Calculator();

    void eval(char *s);

    double getVariable(char c);

    void setVariable(char c, double value);

    double getAns();

    static void formatExpression(char *s);

    void clearVariables();

private:

    double varList[26] = {0};
//I didn't use the struct to build a stack because pointer can reduce readability.
    char operatorS[MAX_SIZE] = {0};//Operator stack
    int operatorSTop = -1;
    double numberS[MAX_SIZE] = {0};
    int numberSTop = -1;
    int isError = 0;//0=no error
    SIGN signs[MAX_SIZE]{};
    int signsSize = 0;//To save the "infix expression" by using "struct SIGN"
    SIGN reverseSigns[MAX_SIZE]{};
    int reverseSignsSize = 0;//To save the "Postfix Expression"
    std::map<char, int> namePriority;


/*
 * Example:
 * if user input str = "1+2*3"
 * the signs(Stack) store 5 item : [isOperator=0,num=1,opera='\0'],[isOperator=1,num=0,opera='+'],[isOperator=0,num=2,opera='\0'],[isOperator=1,num=0,opera='*'],[isOperator=0,num=3,opera='\0']
 * the reverseSigns(Stack) store 5 item : [isOperator=0,num=2,opera='\0'],[isOperator=0,num=3,opera='\0'],[isOperator=1,num=0,opera='*'],[isOperator=0,num=1,opera='\0'],[isOperator=1,num=0,opera='+']
 */


    int getPriority(char c);

    void pushSignOpera(char c);

    void pushSignNum(double n);

    void pushSignXY(char c);

    void pushReverseOpera(char c);

    void pushReverseNum(double n);

    void pushReverseXY(char c);

    static int isAlpha(char c);

    static int isNumber(char c);

    static int isOperator(char c);

    void pushOpera(char opera);

    int isNotEmptyOperaS() const;

    char popOpera();

    int isNotEmptyNumberS() const;

    void pushNumber(double num);

    void clrNumber();

    double popNumber();

    char getTopOpera();

    static void replaceString(char *s, const char *oldS, const char *newS);

    int getOperaArgNum(char op);

    double calculate(double *n, char op, int num);


};


int Calculator::getPriority(char c) {
    int i;
    for (i = 0; i < sizeof(operators); i++) {
        if (operators[i] == c) {
            return priority[i];
        }
    }
    if (c >= 'A' && c <= 'Z') {
        return namePriority[c];
    }
    isError = 4;
    return 0;
}

void Calculator::pushSignOpera(char c) {
    signs[signsSize].isOperator = 1;
    signs[signsSize].opera = c;
    signsSize++;
}

void Calculator::pushSignNum(double n) {
    signs[signsSize].isOperator = 0;
    signs[signsSize].num = n;
    signsSize++;
}

void Calculator::pushSignXY(char c) {
    signs[signsSize].isOperator = -1;
    signs[signsSize].opera = c;
    signsSize++;
}


void Calculator::pushReverseOpera(char c) {
    reverseSigns[reverseSignsSize].isOperator = 1;
    reverseSigns[reverseSignsSize].opera = c;
    reverseSignsSize++;
}

void Calculator::pushReverseNum(double n) {
    reverseSigns[reverseSignsSize].isOperator = 0;
    reverseSigns[reverseSignsSize].num = n;
    reverseSignsSize++;
}

void Calculator::pushReverseXY(char c) {
    reverseSigns[reverseSignsSize].isOperator = -1;
    reverseSigns[reverseSignsSize].opera = c;
    reverseSignsSize++;
}

int Calculator::isAlpha(char c) {
    return c >= 'a' && c <= 'z';
}

int Calculator::isNumber(char c) {
    return (c >= '0' && c <= '9') || (c == '.') || isAlpha(c);
}

int Calculator::isOperator(char c) {
    int flag = 0, i;
    for (i = 0; i < sizeof(operators); i++) {
        if (c == operators[i]) {
            flag = 1;
        }
    }
    for (i = 0; i < sizeof(nameTran); i++) {
        if (c == nameTran[i]) {
            flag = 1;
        }
    }
    return flag;
}


void Calculator::pushOpera(char opera) {//Operator stack
    operatorS[++operatorSTop] = opera;
}


int Calculator::isNotEmptyOperaS() const {
    return operatorSTop != -1;
}


char Calculator::popOpera() {
    return operatorS[operatorSTop--];
}

int Calculator::isNotEmptyNumberS() const {
    return numberSTop != -1;
}

void Calculator::pushNumber(double num) {
    numberS[++numberSTop] = num;
}

void Calculator::clrNumber() {
    numberSTop = -1;
}

double Calculator::popNumber() {
    if (numberSTop < 0) {
        isError = 7;
        return 0;
    }
    return numberS[numberSTop--];
}


char Calculator::getTopOpera() {
    if (operatorSTop != -1) {
        return operatorS[operatorSTop];
    } else {
        isError = 2;
        return '\0';
    }
}


//void Calculator::replaceString(char s[], int pos, int len, char s1[]) {//Replace the s from pos to len with s2
//    int i;
//    char s2[1000];
//    int lenS1 = (int) strlen(s1);
//    int lenS = (int) strlen(s);
//    int j;
//    //copy s to s2 and clear the s
//    for (i = 0; i < lenS; i++) {
//        s2[i] = s[i];
//    }
//    memset(s, '\0', sizeof(*s));
//    for (i = 0; i < pos; ++i) {
//        s[i] = s2[i];
//    }
//    for (i = pos; i < pos + lenS1; i++) {
//        s[i] = s1[i - pos];
//    }
//    j = pos + lenS1;
//    for (i = pos + len; i < lenS; i++) {
//        s[j++] = s2[i];
//    }
//    s[j] = '\0';
//}

//Format string. For example "sin(3.14)+abs(-1)" is format to "J(3.14)+Y(-1)"
void Calculator::formatExpression(char *s) {
    int pos = 0;
    int i;
    for (i = 0; i < MAX_OPERA_NUM; i++) {
        char t[2] = {nameTran[i], '\0'};
        replaceString(s, functionName[i], t);

    }
    if (s[0] == '-') {//decide whether the '-' is '#'
        s[0] = '#';
    }
    pos = 1;
    while (pos < strlen(s)) {//decide whether the '-' is '#'
        if (isOperator(s[pos - 1]) && s[pos] == '-' && s[pos - 1] != ')') {
            s[pos] = '#';
        }
        pos++;
    }
}

int Calculator::getOperaArgNum(char op) {//Get operator's number of arguments.
    int i;
    for (i = 0; i < sizeof(nameTran); ++i) {
        if (nameTran[i] == op) {
            return nameArgNum[i];
        }
    }
    for (i = 0; i < sizeof(operators); ++i) {
        if (operators[i] == op) {
            return operatorArgNum[i];
        }
    }
    isError = 6;
    return 0;
}

int long fact(int n) {//return the number's factor
    if (n < 0)
        return -1;
    if (n > 1)
        return fact(n - 1) * n;
    else
        return n;
}

double Calculator::calculate(double *n, char op,
                             int num) {//Arguments are in *n. op is the operator. num is the number of arguments
    switch (op) {
        case ',':
            return n[num - 1];
        case '#':
            return -n[num - 1];
        case '+':
            return n[num - 1] + n[num - 2];
        case '-':
            return n[num - 1] - n[num - 2];
        case '*':
            return n[num - 1] * n[num - 2];
        case '/':
            return n[num - 2] != 0 ? n[num - 1] / n[num - 2] : (0);
        case '%':
            return (double) ((int) n[num - 1] % (int) n[num - 2]);
        case '^':
            return pow(n[num - 1], n[num - 2]);
        case '!':
            return fact((int) n[num - 1]);
        case '&':
            return fabs(n[num - 1]) >= allowance && fabs(n[num - 2]) >= allowance;
        case '|':
            return fabs(n[num - 1]) >= allowance || fabs(n[num - 2]) >= allowance;
        case '~':
            return fabs(n[num - 1]) <= allowance;
        case 'A':
            return n[num - 1] >= n[num - 2];
        case 'B':
            return n[num - 1] <= n[num - 2];
        case 'C':
            return fabs(n[num - 1] - n[num - 2]) >= allowance;
        case 'D':
            return fabs(n[num - 1] - n[num - 2]) <= allowance;
        case 'E':
            return n[num - 1] > n[num - 2];
        case 'F':
            return n[num - 1] < n[num - 2];
        case 'G':
            return n[num - 1] <= 1 && n[num - 1] >= -1 ? asin(n[num - 1]) : (0);
        case 'H':
            return n[num - 1] <= 1 && n[num - 1] >= -1 ? acos(n[num - 1]) : (0);
        case 'I':
            return atan(n[num - 1]);
        case 'J':
            return sin(n[num - 1]);
        case 'K':
            return cos(n[num - 1]);
        case 'L':
            return tan(n[num - 1]);
        case 'M':
            return n[num - 1] >= 0 && n[num - 2] >= 0 && n[num - 2] - n[num - 1] >= 1 ?
                   (rand() % ((int) n[num - 2] - (int) n[num - 1]) + 1) + (int) n[num - 1] : (0);
        case 'N':
            return n[num - 1] / PI * 180.0;
        case 'O':
            return fabs(n[num - 1]) >= allowance ? n[num - 2] : n[num - 3];
        case 'P':
            return n[num - 1] / 180.0 * PI;
        case 'Q':
            return n[num - 1] != 1 && n[num - 1] > 0 && n[num - 2] > 0 ? log(n[num - 2]) /
                                                                         log(n[num - 1]) : (0);
        case 'R':
            return n[num - 1] > 0 ? log(n[num - 1]) : (0);
        case 'S':
            return exp(n[num - 1]);
        case 'T':
            return n[num - 1] <= n[num - 2] ? n[num - 1] : n[num - 2];
        case 'U':
            return n[num - 1] <= n[num - 2] ? n[num - 2] : n[num - 1];
        case 'V':
            return n[num - 1] >= 0 ? 1 : -1;
        case 'W':
            return (double) (int) (lround(n[num - 1]));
        case 'X':
            return (double) (int) (n[num - 1]);
        case 'Y':
            return n[num - 1] >= 0 ? n[num - 1] : -n[num - 1];
        case 'Z':
            return n[num - 1] >= 0 ? sqrt(n[num - 1]) : (0);
        default://not find the operator
            isError = 6;
            return 0.0f;
    }
}


void Calculator::eval(char s[]) {
    double number = 0;
    int numberUsed = 0;
    int numberPoint = 0;
    int i;
    operatorSTop = -1;
    signsSize = 0;
    reverseSignsSize = 0;

    srand(0);//set srand!
    isError = 0;
    //tranString(s);   !!!!You must decide whether use "tranString" function here or before eval() execute. Because formatExpression() use too much time.
    while (*s != '\0') {
        if (isNumber(*s)) {
            if (isAlpha(*s)) {
                pushSignXY(*s);
            } else {
                numberUsed = 1;
                if (*s == '.') {
                    if (numberPoint != 0) {
                        isError = 1;
                    }
                    numberPoint = 1;
                    s++;
                    continue;
                }
                if (numberPoint == 0) {
                    number *= 10.0;
                    number += *s - '0';
                } else {
                    number += pow(10, -(numberPoint++)) * (*s - '0');
                }
            }
        }
        if (isOperator(*s)) {
            if (numberUsed == 1) {
                numberUsed = 0;
                pushSignNum(number);
                number = 0;
                numberPoint = 0;
            }
            pushSignOpera(*s);
        }

        s++;
    }
    if (numberUsed != 0) {
        pushSignNum(number);
    }
    if (isError) {
        return;
    }
    //start calculating the sign stack

    for (i = 0; i < signsSize; i++) {
        SIGN sign = signs[i];
        if (sign.isOperator != 1) {
            //is number
            if (sign.isOperator == -1) {
                pushReverseXY(sign.opera);
            } else {
                pushReverseNum(sign.num);
            }
        } else {
            //is operator
            if (sign.opera == '(') {
                pushOpera(sign.opera);
            } else if (sign.opera == ')') {
                while (getTopOpera() != '(') {
                    if (isNotEmptyOperaS()) {
                        pushReverseOpera(popOpera());
                    } else {
                        isError = 3;
                        break;
                    }
                }
                if (isNotEmptyOperaS()) {
                    popOpera();
                }
            } else {
                while (isNotEmptyOperaS() &&
                       getPriority(getTopOpera()) >= getPriority(sign.opera)) {
                    pushReverseOpera(popOpera());
                }
                pushOpera(sign.opera);
            }
        }
    }
    while (isNotEmptyOperaS()) {
        char tmp = popOpera();
        if (tmp != '(' && tmp != ')') {
            pushReverseOpera(tmp);
        }
    }

//===========================up --This code block is to test print the "infix expression" and the "Postfix Expression"
//    for (i = 0; i < signsSize; i++) {
//        if (!signs[i].isOperator) {
//            LOGE("%f", signs[i].num);
//        } else {
//            LOGE("%c", signs[i].opera);
//        }
//    }
//    LOGE("\n");
//    for (i = 0; i < reverseSignsSize; i++) {
//        if (!reverseSigns[i].isOperator) {
//            LOGE("%f", reverseSigns[i].num);
//        } else {
//            LOGE("%c", reverseSigns[i].opera);
//        }
//    }
//    LOGE("\n");
//============================down  --This code block is to test print the "infix expression" and the "Postfix Expression"

    //start calculate the expression by reverse (Postfix) expression


}

double Calculator::getVariable(char c) {
    return varList[c - 'a'];
}

void Calculator::setVariable(char c, double value) {
    varList[c - 'a'] = value;
}

void Calculator::clearVariables() {
    memset(varList, 0, sizeof(varList));
}

double Calculator::getAns() {
    int i, size;
    int j;
    double a[5] = {0};
    clrNumber();
    for (i = 0; i < reverseSignsSize; i++) {
        if (reverseSigns[i].isOperator != 1) {
            if (isAlpha(reverseSigns[i].opera)) {
                pushNumber(getVariable(reverseSigns[i].opera));
            } else {
                pushNumber(reverseSigns[i].num);
            }
        } else {
            int n = getOperaArgNum(reverseSigns[i].opera);

            size = 0;
            for (j = 0; j < n; j++) {
                a[size++] = popNumber();
            }
            pushNumber(calculate(a, reverseSigns[i].opera, n));
        }
    }
    return popNumber();
}

Calculator::Calculator() {
    namePriority['A'] = 2;
    namePriority['B'] = 2;
    namePriority['C'] = 2;
    namePriority['D'] = 2;
    namePriority['E'] = 2;
    namePriority['F'] = 2;
    namePriority['G'] = 8;
    namePriority['H'] = 8;
    namePriority['I'] = 8;
    namePriority['J'] = 8;
    namePriority['K'] = 8;
    namePriority['L'] = 8;
    namePriority['M'] = 8;
    namePriority['N'] = 8;
    namePriority['O'] = 8;
    namePriority['P'] = 8;
    namePriority['Q'] = 8;
    namePriority['R'] = 8;
    namePriority['S'] = 8;
    namePriority['T'] = 8;
    namePriority['U'] = 8;
    namePriority['V'] = 8;
    namePriority['W'] = 8;
    namePriority['X'] = 8;
    namePriority['Y'] = 8;
    namePriority['Z'] = 8;

}


void Calculator::replaceString(char *s, const char *oldS, const char *newS) {
    char tmp[2000] = {0};
    for (int i = 0; i < strlen(s); ++i) {
        if (!strncmp(s + i, oldS, strlen(oldS))) {
            strcat(tmp, newS);
            i += strlen(oldS) - 1;
        } else {
            strncat(tmp, s + i, 1);
        }
    }
    strcpy(s, tmp);
}

int instanceIndex = 0;
std::map<int, Calculator> instanceMap;

extern "C" jint
Java_com_example_calculatorjni_jni_CalculatorJNI_newInstance(JNIEnv *env, jclass jobj) {
    instanceMap[instanceIndex] = *new Calculator();
    return instanceIndex++;
}

extern "C" void
Java_com_example_calculatorjni_jni_CalculatorJNI_destroyInstance(JNIEnv *env, jclass jobj,
                                                                 jint id) {
    instanceMap.erase(id);
}

extern "C" void
Java_com_example_calculatorjni_jni_CalculatorJNI_setExpression(JNIEnv *env, jclass jobj,
                                                               jint instanceId,
                                                               jstring expression) {
    char *tmp = jstringToChar(env, expression);
    if (instanceMap.find(instanceId) !=
        instanceMap.end()) {
        memset(instanceMap[instanceId].input, 0, sizeof(instanceMap[instanceId].input));
        int i = 0;
        while (*tmp != '\0') {
            instanceMap[instanceId].input[i++] = *tmp;
            tmp++;
        }
    }

}

extern "C" void
Java_com_example_calculatorjni_jni_CalculatorJNI_eval(JNIEnv *env, jclass jobj, jint instanceId) {
    if (instanceMap.find(instanceId) !=
        instanceMap.end()) {
        instanceMap[instanceId].eval(instanceMap[instanceId].input);
    }
}

extern "C" void
Java_com_example_calculatorjni_jni_CalculatorJNI_formatExpression(JNIEnv *env, jclass jobj,
                                                                  jint instanceId) {
    if (instanceMap.find(instanceId) !=
        instanceMap.end()) {
        instanceMap[instanceId].formatExpression(instanceMap[instanceId].input);
    }
}

extern "C" void
Java_com_example_calculatorjni_jni_CalculatorJNI_clearVariable(JNIEnv *env, jclass jobj,
                                                               jint instanceId) {
    if (instanceMap.find(instanceId) !=
        instanceMap.end()) {
        instanceMap[instanceId].clearVariables();
    }
}

extern "C" jdouble
Java_com_example_calculatorjni_jni_CalculatorJNI_getAns(JNIEnv *env, jclass jobj,
                                                        jint instanceId) {
    if (instanceMap.find(instanceId) !=
        instanceMap.end()) {
        return instanceMap[instanceId].getAns();
    } else {
        return 0;
    }
}

extern "C" void
Java_com_example_calculatorjni_jni_CalculatorJNI_setVariable(JNIEnv *env, jclass jobj,
                                                             jint instanceId, jchar c, jdouble d) {
    if (instanceMap.find(instanceId) !=
        instanceMap.end()) {
        instanceMap[instanceId].setVariable(c, d);
    }
}