#include <iostream>
#include <cmath>
using namespace std;

struct Node {
    int data;
    Node* left;
    Node* right;
    Node(int val) : data(val), left(nullptr), right(nullptr) {}
};

// Standard BST insert (not AVL rotations)
Node* insert(Node* root, int val) {
    if (!root) return new Node(val);
    if (val < root->data)
        root->left = insert(root->left, val);
    else
        root->right = insert(root->right, val);
    return root;
}

// Helper function: check height & balance
int checkHeight(Node* root, bool &isBalanced) {
    if (!root) return 0;

    int leftHeight = checkHeight(root->left, isBalanced);
    int rightHeight = checkHeight(root->right, isBalanced);

    if (abs(leftHeight - rightHeight) > 1)
        isBalanced = false;

    return max(leftHeight, rightHeight) + 1;
}

// Main check function
bool isAVL(Node* root) {
    bool isBalanced = true;
    checkHeight(root, isBalanced);
    return isBalanced;
}

// ----------------------------
// Example usage
// ----------------------------
int main() {
    int arr[] = {50, 30, 70, 60, 80, 90};
    int n = 6;

    Node* root = nullptr;
    for (int i = 0; i < n; i++)
        root = insert(root, arr[i]);

    if (isAVL(root))
        cout << "Output: true (Tree is balanced)\n";
    else
        cout << "Output: false (Tree is NOT balanced)\n";

    return 0;
}
