#include <iostream>
#include <queue>
using namespace std;

int josephus(int n, int k) {
    queue<int> q;

    // Step 1: Insert all people into the queue (1 to n)
    for (int i = 1; i <= n; i++) {
        q.push(i);
    }

    // Step 2: Eliminate every k-th person
    while (q.size() > 1) {
        // Move first (k-1) people to the end of the queue
        for (int i = 1; i < k; i++) {
            q.push(q.front());
            q.pop();
        }
        // Eliminate the k-th person
        q.pop();
    }

    // The last remaining person
    return q.front();
}

int main() {
    int n, k;
    cout << "Enter number of people (n): ";
    cin >> n;
    cout << "Enter step count (k): ";
    cin >> k;

    int survivor = josephus(n, k);
    cout << "The last person standing is at position: " << survivor << endl;

    return 0;
}
