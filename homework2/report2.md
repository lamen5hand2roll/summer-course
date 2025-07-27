# 51115118

作業2

### 題目一

## 解題說明

本題要做出多項式相加、相乘與求值。

### 解題策略
#### 類別設計
Terms：表示一個多項式的單項，包含 coef（係數）和 exp（指數）。
Polynomial：使用動態陣列 Term* termArray 存儲項目，
newTerm()：插入新項並自動擴充。
Polynomial+ : 多項式加法
Polynomial* : 多項式乘法
Eval(float x)：代入 𝑥 並計算多項式的值。
#### << & >>
operator>>：先輸入項數 n，再輸入 n 組係數和指數
operator<<：輸出格式化為 a$x^n$ + b$x^m$ + c
#### 數學運算
add : 指數相等時係數相加
muti : 迴圈計算所有乘積項，並利用迴圈合併同類項
Eval：逐項累加 
 

## 程式實作

以下為遞迴程式碼：

```cpp
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

class Polynomial; // Forward declaration

class Term {
    friend Polynomial;
    friend std::ostream& operator<<(std::ostream& os, const Term& term);
    friend std::istream& operator>>(std::istream& is, Term& term);

private:
    float coef;
    int exp;

public:
    Term(float c = 0, int e = 0) : coef(c), exp(e) {}
    bool operator<(const Term& rhs) const { return exp < rhs.exp; }
};

class Polynomial {
    friend std::ostream& operator<<(std::ostream& os, const Polynomial& poly);
    friend std::istream& operator>>(std::istream& is, Polynomial& poly);

private:
    Term* termArray;
    int capacity;
    int terms;

    void copy(Term* target, const Term* source, int size) {// 複製陣列
        for (int i = 0; i < size; ++i)
            target[i] = source[i];
    }

public:
    Polynomial(int initialCapacity = 4) : capacity(initialCapacity), terms(0) {// 構造函數
        if (initialCapacity <= 0) {
            std::cerr << "Invalid capacity!" << std::endl;
            std::exit(1);   // 
        }
        termArray = new Term[capacity];
    }

    Polynomial(const Polynomial& other) : capacity(other.capacity), terms(other.terms) {// 複製構造函數
        termArray = new Term[capacity];
        copy(termArray, other.termArray, terms);
    }

    Polynomial& operator=(const Polynomial& other) {// 賦值運算符重載
        if (this != &other) {
            delete[] termArray;
            capacity = other.capacity;
            terms = other.terms;
            termArray = new Term[capacity];
            copy(termArray, other.termArray, terms);
        }
        return *this;
    }

    ~Polynomial() {// 解構函數
        delete[] termArray;
    }

    void newTerm(float coef, int exp) {// 新增一項
        if (coef == 0) return;
        if (terms >= capacity) {
            capacity *= 2;
            Term* newArray = new Term[capacity];
            copy(newArray, termArray, terms);
            delete[] termArray;
            termArray = newArray;
        }
        termArray[terms++] = Term(coef, exp);
    }

    Polynomial operator+(const Polynomial& rhs) const {// 多項式加法
        Polynomial result;
        int i = 0, j = 0;
        while (i < terms && j < rhs.terms) {
            if (termArray[i].exp == rhs.termArray[j].exp) {
                float sumCoef = termArray[i].coef + rhs.termArray[j].coef;
                if (sumCoef != 0)
                    result.newTerm(sumCoef, termArray[i].exp);
                ++i; ++j;
            } else if (termArray[i].exp > rhs.termArray[j].exp) {
                result.newTerm(termArray[i].coef, termArray[i].exp);
                ++i;
            } else {
                result.newTerm(rhs.termArray[j].coef, rhs.termArray[j].exp);
                ++j;
            }
        }
        while (i < terms) result.newTerm(termArray[i].coef, termArray[i++].exp);
        while (j < rhs.terms) result.newTerm(rhs.termArray[j].coef, rhs.termArray[j++].exp);
        return result;
    }


    Polynomial operator*(const Polynomial& rhs) const { // 多項式乘法
        Polynomial result;
    
        for (int i = 0; i < terms; ++i) {
            for (int j = 0; j < rhs.terms; ++j) {
                float prodCoef = termArray[i].coef * rhs.termArray[j].coef;
                int prodExp = termArray[i].exp + rhs.termArray[j].exp;
    
                if (prodCoef != 0) {
                    // 合併同類項
                    bool merged = false;
                    for (int k = 0; k < result.terms; ++k) {
                        if (result.termArray[k].exp == prodExp) {
                            result.termArray[k].coef += prodCoef;
                            merged = true;
                            break;
                        }
                    }
                    if (!merged) result.newTerm(prodCoef, prodExp);
                }
            }
        }
    
        return result;
    }
    // 多項式求值
float Eval(float x) const {
    float result = 0;
    for (int i = 0; i < terms; ++i) {
        result += termArray[i].coef * pow(x, termArray[i].exp);
    }
    return result;
}

}; 
// Stream operators
std::istream& operator>>(std::istream& is, Polynomial& poly) {
    int n;
    float coef;
    int exp;
    is >> n;
    for (int i = 0; i < n; ++i) {
        is >> coef >> exp;
        poly.newTerm(coef, exp);
    }
    return is;
}

std::ostream& operator<<(std::ostream& os, const Term& term) {
    os << term.coef << "x^" << term.exp;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Polynomial& poly) {
    for (int i = 0; i < poly.terms; ++i) {
        if (i > 0) os << " + ";
        os << poly.termArray[i];
    }
    return os;
}

// Main for testing
int main() {
    Polynomial polyA, polyB;
    
    std::cout << "Polynomial A : ";
    std::cin >> polyA;
    std::cout << "Polynomial B : ";
    std::cin >> polyB;
    Polynomial sum = polyA + polyB;
    Polynomial muti = polyA * polyB;
    std::cout << "A + B = " << sum << std::endl;
    std::cout << "A * B = " << muti << std::endl;
    float x;// 求值
    std::cout << "enter x (number) : ";
    std::cin >> x;
    std::cout << "A(" << x << ") = " << polyA.Eval(x) << std::endl;
    std::cout << "B(" << x << ") = " << polyB.Eval(x) << std::endl;
    std::cout << "A + B(" << x << ") = " << sum.Eval(x) << std::endl;
    std::cout << "A * B(" << x << ") = " << muti.Eval(x) << std::endl;
    return 0;
}



```


## 效能分析

|操作|時間複雜度|空間複雜度|分析|
|------|------|------|------|
|newTerm()|O(1)  |O(n)|每次擴容容量翻倍 *=2|
|operator+|O(n+m)|O(n+m)|兩個已排序陣列合併|
|operator*|O(n*m)|O(n+m)|需要雙層迴圈遍歷所有項目並合併|
|eval     |O(n)  |O(1)|只需遍歷一次|

## 測試與驗證

### 測試案例


| 測試案例 | 輸入多項式$A$ | 輸入多項式$B$  | 實際輸出(add)  | 實際輸出(muti) |
| -------- | -----------  | --------| ---------- | ------------- |
| 測試一   |2 1 1 2 0|2 1 1 2 0| 2x^1 + 4x^0 |1x^2 + 4x^1 + 4x^0|
| 測試二   |3 5 4 4 3 2 1|2 5 2 7 0| 5x^4 + 4x^3 + 5x^2 + 2x^1| 25x^6 + 35x^4 + 20x^5 + 38x^3 + 14x^1|
|求值| 輸入x的值| A(x)|B(x) | A + B (x)|A * B (x)|
|測試一|4| A(4)=6|B(4)=6 | A + B (4) =>12|A * B (4) =>36|
|測試二|2.5| A(2.5)=262.812|B(2.5)=38.25 | A + B (2.5) =>294.062|A * B (2.5) =>10052.6|


### 編譯與執行指令

```shell
$ g++ Poly.cpp -o poly
$ ./poly

```

### 結論
基本上輸入的多項式，都能完美的被算出來


## 申論及開發報告
在寫多項式乘法時，我直接複製加法的程式碼並在float sumCoef = termArray[i].coef + rhs.termArray[j].coef;把+改成*。但結果錯了，因為忘記可以這樣做的是減法，因為都是指數相同時相加。不需要雙迴圈來乘



