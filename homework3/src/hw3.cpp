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
