# 51115118

作業3

### 題目一

## 解題說明


多項式以循環鏈表表示，每個節點包含 coef（係數）、exp（指數）和 link（指向下一個節點的指標）。
指數按降序排列（e1 > e2 > ... > en），n 為項數。
使用可用空間鏈表（available-space list）來高效刪除節點。
實現輸入/輸出運算子、複製建構子、賦值運算子、加減乘法運算子以及評估值計算。



### 解題策略

使用環狀鏈結串列實作多項式。
##### 函數實現
(a) std::istream& operator>>(std::istream& is, Polynomial& x)：讀取輸入多項式並轉換為循環鏈表表示。
(b) std::ostream& operator<<(std::ostream& os, const Polynomial& x)：將鏈表表示轉為外部表示並輸出。
(c) Polynomial::Polynomial(const Polynomial& a)：複製建構函數，將 ( a ) 複製到 ( *this )。
(d) const Polynomial& Polynomial::operator=(const Polynomial& a)：賦值運算子，將 ( a ) 賦值給 ( *this )。
(e) Polynomial::~Polynomial()：解構函數，將 ( *this ) 的所有節點歸還至可用空間鏈表。
(f) Polynomial operator+(const Polynomial& b) const：回傳 ( *this + b )。
(g) Polynomial operator-(const Polynomial& b) const：回傳 ( *this - b )。
(h) Polynomial operator*(const Polynomial& b) const：回傳 ( *this * b )。
(i) float Polynomial::Evaluate(float x) const：評估 ( *this ) 在 ( x ) 處的值。

## 程式實作

以下為程式碼：

```cpp
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
using namespace std;

struct Term {
    float coef;
    int exp;
    Term(float c = 0.0f, int e = 0) : coef(c), exp(e) {}
};

template <class T>
class chain {
    friend class Available;
    friend class Polynomial;
private:
    T term;
    chain<T>* next;
public:
    chain() : term(T()), next(nullptr) {}
    chain(const T& t) : term(t), next(nullptr) {}
    chain(float coef, int exp) : term(T(coef, exp)), next(nullptr) {}
    T& termRef() { return term; }
    const T& termRef() const { return term; }
    chain<T>* getNext() const { return next; }
    void setNext(chain<T>* n) { next = n; }
};

class Available {
private:
    chain<Term>* avaList;
public:
    Available() : avaList(nullptr) {}
    ~Available() {
        while (avaList) {
            chain<Term>* tmp = avaList;
            avaList = avaList->getNext();
            delete tmp;
        }
    }
    void getBack(chain<Term>* node) {
        if (!node) return;
        chain<Term>* tail = node;
        while (tail->getNext()) tail = tail->getNext();
        tail->setNext(avaList);
        avaList = node;
    }
    chain<Term>* getOneNode() {
        if (!avaList) return new chain<Term>();
        chain<Term>* node = avaList;
        avaList = avaList->getNext();
        node->setNext(nullptr);
        return node;
    }
} globalASL;

class Polynomial {
    friend istream& operator>>(istream& is, Polynomial& p);
    friend ostream& operator<<(ostream& os, const Polynomial& p);
public:
    Polynomial() : header(nullptr), last(nullptr), terms(0) {}
    Polynomial(const Polynomial& other) : header(nullptr), last(nullptr), terms(0) { copyFrom(other); }
    Polynomial& operator=(const Polynomial& other) {
        if (this != &other) { clear(); copyFrom(other); }
        return *this;
    }
    ~Polynomial() { clear(); }

    void newTermSorted(float coef, int exp);
    float Evaluate(float x) const;
    Polynomial operator+(const Polynomial& b) const;
    Polynomial operator-(const Polynomial& b) const;
    Polynomial operator*(const Polynomial& b) const;

private:
    chain<Term>* header;// 指向第一個節點
    chain<Term>* last;// 指向最後一個節點
    int terms;
    void clear();
    void copyFrom(const Polynomial& other);
};

void Polynomial::newTermSorted(float coef, int exp) {
    if (fabs(coef) < 1e-7f) return;
    chain<Term>* node = globalASL.getOneNode();
    node->termRef().coef = coef;
    node->termRef().exp = exp;

    if (!header) { // first term
        header = last = node;
        last->setNext(header);
        ++terms;
        return;
    }

    chain<Term>* prev = last;
    chain<Term>* cur = header;
    do {// 遍歷循環鏈表
        if (exp > cur->termRef().exp) { // insert before cur
            node->setNext(cur);
            if (cur == header) header = node;
            prev->setNext(node);
            ++terms;
            return;
        } else if (exp == cur->termRef().exp) { // merge
            cur->termRef().coef += coef;
            if (fabs(cur->termRef().coef) < 1e-7f) { // remove term
                if (cur == header) header = cur->getNext();
                prev->setNext(cur->getNext());
                if (cur == last) last = prev;
                globalASL.getBack(cur);
                --terms;
            }
            return;
        }
        prev = cur;
        cur = cur->getNext();
    } while (cur != header);

    // insert at end
    last->setNext(node);
    last = node;
    last->setNext(header);
    ++terms;
}

void Polynomial::clear() {// 釋放（回收）節點到 ASL
    if (!header) return;
    last->setNext(nullptr);
    globalASL.getBack(header);
    header = last = nullptr;
    terms = 0;
}

void Polynomial::copyFrom(const Polynomial& other) {// 深複製
    clear();
    if (other.terms == 0) return;
    chain<Term>* p = other.header;
    int cnt = other.terms;
    while (cnt--) {
        newTermSorted(p->termRef().coef, p->termRef().exp);
        p = p->getNext();
    }
}

float Polynomial::Evaluate(float x) const {// 計算多項式在 x 處的值
    float sum = 0.0f;
    if (!header) return sum;
    chain<Term>* cur = header;
    int cnt = terms;
    while (cnt--) {
        sum += cur->termRef().coef * powf(x, static_cast<float>(cur->termRef().exp));
        cur = cur->getNext();
    }
    return sum;
}

Polynomial Polynomial::operator+(const Polynomial& b) const {// 加法
    Polynomial res;
    chain<Term>* pa = header; int ca = terms;
    chain<Term>* pb = b.header; int cb = b.terms;
    while (ca > 0 && cb > 0) {
        int ea = pa->termRef().exp, eb = pb->termRef().exp;
        if (ea > eb) { res.newTermSorted(pa->termRef().coef, ea); pa = pa->getNext(); --ca; }
        else if (ea < eb) { res.newTermSorted(pb->termRef().coef, eb); pb = pb->getNext(); --cb; }
        else {
            res.newTermSorted(pa->termRef().coef + pb->termRef().coef, ea);
            pa = pa->getNext(); pb = pb->getNext(); --ca; --cb;
        }
    }
    while (ca-- > 0) { res.newTermSorted(pa->termRef().coef, pa->termRef().exp); pa = pa->getNext(); }
    while (cb-- > 0) { res.newTermSorted(pb->termRef().coef, pb->termRef().exp); pb = pb->getNext(); }
    return res;
}

Polynomial Polynomial::operator-(const Polynomial& b) const {// 減法
    Polynomial res;
    chain<Term>* pa = header; int ca = terms;
    chain<Term>* pb = b.header; int cb = b.terms;
    while (ca > 0 && cb > 0) {
        int ea = pa->termRef().exp, eb = pb->termRef().exp;
        if (ea > eb) { res.newTermSorted(pa->termRef().coef, ea); pa = pa->getNext(); --ca; }
        else if (ea < eb) { res.newTermSorted(-pb->termRef().coef, eb); pb = pb->getNext(); --cb; }
        else {
            res.newTermSorted(pa->termRef().coef - pb->termRef().coef, ea);
            pa = pa->getNext(); pb = pb->getNext(); --ca; --cb;
        }
    }
    while (ca-- > 0) { res.newTermSorted(pa->termRef().coef, pa->termRef().exp); pa = pa->getNext(); }
    while (cb-- > 0) { res.newTermSorted(-pb->termRef().coef, pb->termRef().exp); pb = pb->getNext(); }
    return res;
}

Polynomial Polynomial::operator*(const Polynomial& b) const {// 乘法運算
    Polynomial res;
    if (!header || !b.header) return res;
    chain<Term>* pa = header;
    int ca = terms;
    while (ca--) {
        chain<Term>* pb = b.header;
        int cb = b.terms;
        while (cb--) {
            res.newTermSorted(pa->termRef().coef * pb->termRef().coef,
                              pa->termRef().exp + pb->termRef().exp);
            pb = pb->getNext();
        }
        pa = pa->getNext();
    }
    return res;
}

istream& operator>>(istream& is, Polynomial& p) {// 讀入多項式
    p.clear();
    int n; if (!(is >> n)) return is;// 讀不到 n 就直接回傳
    char comma;
    for (int i = 0; i < n; ++i) {
        float c; int e;
        is >> comma >> c >> comma >> e;
        p.newTermSorted(c, e);
    }
    return is;
}

ostream& operator<<(ostream& os, const Polynomial& x) {// 輸出多項式
    os << x.terms;
    if (x.terms == 0) return os;// 如果沒有項目，直接返回
    chain<Term>* cur = x.header; int cnt = x.terms;
    while (cnt--) {
        os << "," << cur->termRef().coef << "," << cur->termRef().exp;
        cur = cur->getNext();
    }
    return os;
}

int main() {
    Polynomial A, B;
   
    cin >> A;  // 讀第一行
    cin >> B;  // 讀第二行
  

    cout << "A = " << A << "\n";
    cout << "B = " << B << "\n";
    cout << "A + B = " << (A + B) << "\n";
    cout << "A - B = " << (A - B) << "\n";
    cout << "A * B = " << (A * B) << "\n";
    cout << "A(2) = " << A.Evaluate(2.0f) << "\n";
    return 0;
}



```
```txt
3,7,3,2,1,1,0
4,5,6,1,4,2,3,1,1

```


## 效能分析

1. 時間複雜度：插入/刪除單項：$ O(n) $，加/減法：$ O(n + m) $，乘法：$ O(n \cdot m) $
2. 空間複雜度：單個 Polynomial 物件：$ O(n) $、加/減法$ O(\max(n, m)) $、乘法$ O(n \cdot m) $

## 測試與驗證

### 測試案例


| 測試案例 | 讀取第一行 | 讀取第二行 | $A + B$       | $A - B$            |$A * B$ |
| -------- | ----------- | ----------- | -------------- | ------------------ |----|
| 測試一   | 3,7,3,2,1,1,0 |4,5,6,1,4,2,3,1,1  |5,5,6,1,4,9,3,3,1,1,0|5,-5,6,-1,4,5,3,1,1,1,0|8,35,9,17,7,19,6,2,5,12,4,2,3,2,2,1,1|
| 測試二   |2,5,6,2,3|2,5,6,1,4|3,10,6,1,4,2,3|2,-1,4,2,3|4,25,12,5,10,10,9,2,7|
| 測試三   |0(空值)|0(空值)|0|0|0|
| 測試四   | 2,1,3,6,1|(空值)| 2,1,3,6,1| 2,1,3,6,1|0|



### 編譯與執行指令

```shell
$ g++ hw3.cpp -o hw3 
$ ./hw3 < poly.txt

```

### 結論

1. 處理正常輸入的多項式(測試一、二)
2. 無輸入或輸入0，(測試三)
3. 處理單一輸入時的結果(四)

## 心得
一開始在使用vscode的termianl執行測資檔時跳出了'<' 運算子保留供未來使用。 + CategoryInfo : ParserError: (:) [], ParentContainsErrorRecordException + FullyQualifiedErrorId : RedirectionNotSupported。但後來想到直接用cmd執行就好了。
不過我是在vscode編譯，cmd執行(是不是有點多此一舉的感覺)


