#include <iostream>
using namespace std;
int ackermann(int m, int n) {
    if (m==0) return n+1;
    else if (n==0) return ackermann(m-1,1);
    else return ackermann(m-1, ackermann(m,n-1));
}
//例如輸入A(2,1) -> A(1,A(2,0)) -> 先算A(2,0) => A(1,1) => A(0,2) =>3 ，再跳回最外面A(1,3)....
int main() {
    int result=ackermann(-1,-1);
    cout<<result<<'\n'; 
    return 0;
}

