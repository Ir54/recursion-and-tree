#include <iostream>
#include <vector>
#include <string>
using namespace std;

void generateParentheses(int n, int open, int close, string current, vector<string>& result) {
    // Base case: valid combination complete
    if (open == n && close == n) {
        result.push_back(current);
        return;
    }

    // Add '(' if we still have some left to add
    if (open < n)
        generateParentheses(n, open + 1, close, current + "(", result);

    // Add ')' if we have more opens than closes
    if (close < open)
        generateParentheses(n, open, close + 1, current + ")", result);
}

int main() {
    int n = 2;
    vector<string> result;
    generateParentheses(n, 0, 0, "", result);

    cout << "All valid combinations:\n";
    for (string s : result) cout << s << endl;

    cout << "Total combinations: " << result.size() << endl;
    return 0;
}
