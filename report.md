# 51115118

作業1 

## 解題說明

本題要求實現一個阿克曼函數，並且使用遞迴的方式算出A(m,n)的n值。

### 解題策略

輸入A(m,n)

1.如果m=0 則 n+1。

2.如果n=0 則 A(m-1,1)。

3.如果都不滿足上面兩個條件 則 A(m-1,A(m,n-1))。

## 程式實作

以下為遞迴程式碼：

```cpp
#include <iostream>
using namespace std;
int ackermann(int m, int n) {
    if (m==0) return n+1;
    else if (n==0) return ackermann(m-1,1);
    else return ackermann(m-1, ackermann(m,n-1));
}
//例如輸入A(2,1) -> A(1,A(2,0)) -> 先算A(2,0) => A(1,1) => A(0,2) =>3 ，再跳回最外面A(1,3)....
int main() {
    int result=ackermann(2,1);
    cout<<result<<'\n'; 
    return 0;
}


```
以下為非遞迴的程式碼 :
``` cpp
#include <iostream>
#include <stack>
#include <utility>
using namespace std;

int ackermann(int m, int n) {
    stack<pair<int, int>> stk;

    stk.push({m, n});

    while (!stk.empty()) {
        pair<int, int> top = stk.top();
        stk.pop();
        m = top.first;
        n = top.second;

        if (m == 0) {
            n = n + 1;
        } else if (n == 0) {
            stk.push({m - 1, 1});
            continue;
        } else {
            // 模擬 A(m-1, A(m, n-1))：
            // 要先算 A(m, n-1)，等它算好後再拿來做 A(m-1, result)
            stk.push({m - 1, -1});   // 用 -1 當標記：下一步是要做 A(m-1, result)
            stk.push({m, n - 1});    // 先算 A(m, n-1)
            continue;
        }

        // 傳遞回傳值給上一層
        while (!stk.empty() && stk.top().second == -1) {
            pair<int, int> tmp = stk.top(); stk.pop(); // A(m-1, result)
            stk.push({tmp.first, n}); // 把 result 套進來變成 A(m-1, result)
            break;
        }
    }

    return n;
}

int main() {
    int m, n;
    cout << "Enter m and n: ";
    cin >> m >> n;

    int result = ackermann(m, n);
    cout << "Ackermann(" << m << ", " << n << ") = " << result << endl;

    return 0;
}

```

## 效能分析

1. 時間複雜度：m值越高，複雜度也愈高
2. 空間複雜度：空間取決於遞迴的呼叫堆疊深度，空間使用量與遞迴深度成正比

## 測試與驗證

### 測試案例


| 測試案例 | 輸入參數$m$ | 輸入參數$n$ | 預期輸出 | 實際輸出 |
| -------- | ------------| -----------| -------- | -------- |
| 測試一   | $m = 0$     |$n = 0$     | 1        | 1        |
| 測試二   | $m = 1$     |$n = 1$     | 3        | 3        |
| 測試三   | $m = 2$     |$n = 3$     | 9        | 9        |
| 測試四   | $m = 4$    |$n = 5$     | stack overflow | 無(沒有東西出現!!)|
| 測試五   | $m = -1$    |$n = -1$     | 異常拋出 | 異常拋出 |

### 編譯與執行指令

```shell
$ g++ ackermann.cpp -o ackermann.exe 
$ ./ackermann.exe
$ g++ non.cpp -std=c++17 -o non   
$ ./non
```

### 結論


2. 在 $n < 0$ 的情況下，程式會拋出異常。
3. 在 $m$與$n$ 過大的情況下，程式會卡住


## 申論及開發報告

使用遞迴的版本與非遞迴的比起來相對容易理解，畢竟上課教的大多都是遞迴。應該說沒有使用過c++的堆疊函式。
