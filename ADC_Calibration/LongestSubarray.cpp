#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;
using ld = long double;
using ull = unsigned long long;

int main()
{
    int offset = 0;
    cin >> offset;
    int a[10] = {0};
    for (int i = 0; i < 10; i++)
    {
        cin >> a[i];
    }
    std::sort(a, a + 10);
    int max = 0;
    int left = 0;
    int right = 9;
    for (int i = 0; i < 10; i++)
    {
        auto it = upper_bound(a + i, a + 10, a[i] + offset);
        if(it - a - i > max)
        {
            max = it - a - i;
            left = i;
            right = it - a - 1;
        }
    }
    for (int i = left; i <= right; i++)
    {
        cout << a[i] << " ";
    }
    cout << "\n";
    return 0;
}