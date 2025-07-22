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
