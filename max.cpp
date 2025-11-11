#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int maxAreaRecursive(vector<int>& height, int left, int right) {
    // Base case: when pointers meet
    if (left >= right) return 0;

    // Calculate current area
    int currentArea = min(height[left], height[right]) * (right - left);

    // Move the smaller pointer and compute recursively
    if (height[left] < height[right])
        return max(currentArea, maxAreaRecursive(height, left + 1, right));
    else
        return max(currentArea, maxAreaRecursive(height, left, right - 1));
}

int main() {
    vector<int> height = {1,8,6,2,5,4,8,3,7};
    cout << "Maximum Water Area: " << maxAreaRecursive(height, 0, height.size() - 1);
    return 0;
}
