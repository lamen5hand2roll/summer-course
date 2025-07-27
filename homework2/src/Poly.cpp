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
