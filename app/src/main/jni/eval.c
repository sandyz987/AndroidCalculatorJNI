//
// Created by lenovo on 2021/3/22.
//
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <jni.h>
#include "utils.h"


/**
 * Developed by sandyz987
 * Function 'eval'
 * @param Expression string (chars' length <= MAX_SIZE)
 * @return Answer : double
 * @isError 0:no error  1:wrong number of decimal points  2:can't get top item at an empty stack  3:can't pop item at an empty stack(number of brackets is invalid?)
 *          4:can't get priority   5:too many arguments   6:unexpect character   7:wrong number of arguments   8:math error
 */

#define PI 3.141592653
#define MAX_SIZE 320
#define MAX_OPERA_NUM 26
double allowance = 1.0e-1;

char *functionName[MAX_OPERA_NUM] = {">=", "<=", "!=", "==", ">", "<", "asin", "acos", "atan", "sin",
                                     "cos", "tan", "rand", "deg", "if", "rad", "log", "ln", "exp",
                                     "min", "max", "sign", "round", "floor", "abs", "sqrt"};
char nameTran[MAX_OPERA_NUM] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
                                'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
int namePriority[MAX_OPERA_NUM] = {2, 2, 2, 2, 2, 2, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
                                   8, 8, 8, 8};//function's priority
int nameArgNum[MAX_OPERA_NUM] = {2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 1, 3, 1, 2, 1, 1, 2, 2, 1, 1,
                                 1, 1, 1};//Number of arguments

char operator[] = {'+', '-', '*', '/', '^', '!', '%', '(', ')', ',', '&', '|', '~', '#'};
int priority[] = {3, 3, 4, 4, 5, 5, 5, -4, -5, -1, 1, 0, 5, 7};//Operator' priority
int operatorArgNum[] = {2, 2, 2, 2, 2, 1, 2, 0, 0, 1, 2, 2, 1, 1};//Number of arguments

double varList[26] = {0};


//I didn't use the struct to build a stack because pointer can reduce readability.
char operatorS[MAX_SIZE] = {0};//Operator stack
int operatorSTop = -1;
double numberS[MAX_SIZE] = {0};
int numberSTop = -1;
int isError = 0;//0=no error


typedef struct {
    int isOperator;//If isOperator == 0 use the num, else use the opera
    double num;
    char opera;
} SIGN;
SIGN signs[MAX_SIZE];
int signsSize = 0;//To save the "infix expression" by using "struct SIGN"
SIGN reverseSigns[MAX_SIZE];
int reverseSignsSize = 0;//To save the "Postfix Expression"
/*
 * Example:
 * if user input str = "1+2*3"
 * the signs(Stack) store 5 item : [isOperator=0,num=1,opera='\0'],[isOperator=1,num=0,opera='+'],[isOperator=0,num=2,opera='\0'],[isOperator=1,num=0,opera='*'],[isOperator=0,num=3,opera='\0']
 * the reverseSigns(Stack) store 5 item : [isOperator=0,num=2,opera='\0'],[isOperator=0,num=3,opera='\0'],[isOperator=1,num=0,opera='*'],[isOperator=0,num=1,opera='\0'],[isOperator=1,num=0,opera='+']
 */


int getPriority(char c) {
    int i;
    for (i = 0; i < sizeof(operator); i++) {
        if (operator[i] == c) {
            return priority[i];
        }
    }
    for (i = 0; i < sizeof(nameTran); i++) {
        if (nameTran[i] == c) {
            return namePriority[i];
        }
    }
    isError = 4;
    return 0;
}

void pushSignOpera(char c) {
    signs[signsSize].isOperator = 1;
    signs[signsSize].opera = c;
    signsSize++;
}

void pushSignNum(double n) {
    signs[signsSize].isOperator = 0;
    signs[signsSize].num = n;
    signsSize++;
}

void pushSignXY(char c) {
    signs[signsSize].isOperator = -1;
    signs[signsSize].opera = c;
    signsSize++;
}


void pushReverseOpera(char c) {
    reverseSigns[reverseSignsSize].isOperator = 1;
    reverseSigns[reverseSignsSize].opera = c;
    reverseSignsSize++;
}

void pushReverseNum(double n) {
    reverseSigns[reverseSignsSize].isOperator = 0;
    reverseSigns[reverseSignsSize].num = n;
    reverseSignsSize++;
}

void pushReverseXY(char c) {
    reverseSigns[reverseSignsSize].isOperator = -1;
    reverseSigns[reverseSignsSize].opera = c;
    reverseSignsSize++;
}

int isAlpha(char c) {
    return c >= 'a' && c <= 'z';
}

int isNumber(char c) {
    return (c >= '0' && c <= '9') || (c == '.') || isAlpha(c);
}

int isOperator(char c) {
    int flag = 0, i;
    for (i = 0; i < sizeof(operator); i++) {
        if (c == operator[i]) {
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


void pushOpera(char opera) {//Operator stack
    operatorS[++operatorSTop] = opera;
}


int isNotEmptyOperaS() {
    return operatorSTop != -1;
}


char popOpera() {
    return operatorS[operatorSTop--];
}

int isNotEmptyNumberS() {
    return numberSTop != -1;
}

void pushNumber(double num) {
    numberS[++numberSTop] = num;
}

void clrNumber() {
    numberSTop = -1;
}

double popNumber() {
    if (numberSTop < 0) {
        isError = 7;
        return 0;
    }
    return numberS[numberSTop--];
}


char getTopOpera() {
    if (operatorSTop != -1) {
        return operatorS[operatorSTop];
    } else {
        isError = 2;
        return '\0';
    }
}


void replaceString(char s[], int pos, int len, char s1[]) {//Replace the s from pos to len with s2
    int i;
    char s2[1000];
    int lenS1 = (int) strlen(s1);
    int lenS = (int) strlen(s);
    int j;
    //copy s to s2 and clear the s
    for (i = 0; i < lenS; i++) {
        s2[i] = s[i];
    }
    memset(s, '\0', sizeof(*s));
    for (i = 0; i < pos; ++i) {
        s[i] = s2[i];
    }
    for (i = pos; i < pos + lenS1; i++) {
        s[i] = s1[i - pos];
    }
    j = pos + lenS1;
    for (i = pos + len; i < lenS; i++) {
        s[j++] = s2[i];
    }
    s[j] = '\0';
}

//Format string. For example "sin(3.14)+abs(-1)" is format to "J(3.14)+Y(-1)"
void formatExpression(char *s) {
    int pos = 0;
    int i;
    while (pos < strlen(s)) {
        for (i = 0; i < MAX_OPERA_NUM; i++) {
            if (pos + (int) strlen(functionName[i]) <= (int) strlen(s)) {
                char tmp[20];
                memset(tmp, '\0', sizeof(tmp));
                strncpy(tmp, s + pos, strlen(functionName[i]));
                if (strcmp(functionName[i], tmp) == 0) {
                    char tmpChar[2] = {'\0', '\0'};
                    tmpChar[0] = nameTran[i];
                    replaceString(s, pos, (int) strlen(functionName[i]), tmpChar);
                }
            }

        }
        pos++;
    }
    if (s[0] == '-') {//decide whether the '-' is '#'
        char tmpChar[2] = {'#', '\0'};
        replaceString(s, 0, 1, tmpChar);
    }
    pos = 1;
    while (pos < strlen(s)) {//decide whether the '-' is '#'
        if (isOperator(s[pos - 1]) && s[pos] == '-' && s[pos - 1] != ')') {
            char tmpChar[2] = {'#', '\0'};
            replaceString(s, pos, 1, tmpChar);
        }
        pos++;
    }
}

int getOperaArgNum(char op) {//Get operator's number of arguments.
    int i;
    for (i = 0; i < sizeof(nameTran); ++i) {
        if (nameTran[i] == op) {
            return nameArgNum[i];
        }
    }
    for (i = 0; i < sizeof(operator); ++i) {
        if (operator[i] == op) {
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

double calculate(double *n, char op,
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
            return (double) (int) (n[num - 1] + 0.5);
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




void eval(char s[]) {
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
    for (i = 0; i < signsSize; i++) {
        if (!signs[i].isOperator) {
            LOGE("%f,", signs[i].num);
        } else {
            LOGE("%c,", signs[i].opera);
        }
    }
    LOGE("\n");
    for (i = 0; i < reverseSignsSize; i++) {
        if (!reverseSigns[i].isOperator) {
            LOGE("%f,", reverseSigns[i].num);
        } else {
            LOGE("%c,", reverseSigns[i].opera);
        }
    }
    LOGE("\n");
//============================down  --This code block is to test print the "infix expression" and the "Postfix Expression"

    //start calculate the expression by reverse (Postfix) expression


}

double getVariable(char c) {
    return varList[c - 'a'];
}

void setVariable(char c, double value) {
    varList[c - 'a'] = value;
}

void clearVariables() {
    memset(varList, 0, sizeof(varList));
}

double getAns() {
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

char s[1000] = {0};

void Java_com_example_CalculatorJNI_jni_CalculatorJNI_setExpression(JNIEnv *env, jobject jobj, jstring expression) {
    char *tmp = jstringToChar(env, expression);
    memset(s, 0, sizeof(s));
    int i = 0;
    while (*tmp != '\0') {
        s[i++] = *tmp;
        tmp++;
    }
}

void Java_com_example_CalculatorJNI_jni_CalculatorJNI_eval(JNIEnv *env, jobject jobj) {
    eval(s);
}

void Java_com_example_CalculatorJNI_jni_CalculatorJNI_formatExpression(JNIEnv *env, jobject jobj) {
    formatExpression(s);
}

void Java_com_example_CalculatorJNI_jni_CalculatorJNI_clearVariable(JNIEnv *env, jobject jobj) {
    clearVariables();
}

jdouble Java_com_example_CalculatorJNI_jni_CalculatorJNI_getAns(JNIEnv *env, jobject jobj) {
    return getAns();
}

void Java_com_example_CalculatorJNI_jni_CalculatorJNI_setVariable(JNIEnv *env, jobject jobj, jchar c, jdouble d) {
    setVariable(c, d);
}