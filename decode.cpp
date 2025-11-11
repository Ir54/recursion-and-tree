#include <iostream>
#include <string>
using namespace std;

int decodeWays(string s, int index) {
    // Base case: reached the end → valid decoding
    if (index == s.length()) return 1;

    // If current digit is '0', invalid path
    if (s[index] == '0') return 0;

    // Decode one digit
    int ways = decodeWays(s, index + 1);

    // Decode two digits if valid (10 to 26)
    if (index + 1 < s.length()) {
        int twoDigit = stoi(s.substr(index, 2));
        if (twoDigit >= 10 && twoDigit <= 26)
            ways += decodeWays(s, index + 2);
    }

    return ways;
}

int main() {
    string s = "11106";
    cout << "Number of ways to decode: " << decodeWays(s, 0);
    return 0;
}
