#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    string temp;
    // 读取所有string并进行大写的转化
    while(cin >> temp) {
        transform(temp.begin(), temp.end(), temp.begin(), ::toupper);
        cout << temp << endl;
    }
    return 0;
}