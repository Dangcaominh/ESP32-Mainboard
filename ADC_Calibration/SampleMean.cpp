#include <iostream>
#include <vector>
using namespace std;
using ld = long double;
using ull = unsigned long long;

int main()
{
    ull a = 0;
    ull x;
    ull size = 0;
    while(cin >> x)
    {
        a += x;
        size++;
    }
    cout << "Mean: " << ld(a) / ld(size) << "\n";
}