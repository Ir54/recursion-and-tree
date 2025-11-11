#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
using namespace std;

// ----------------------------
// Post structure
// ----------------------------
struct Post {
    int postID;
    string author;
    int engagementScore; // likes + comments + shares

    Post(int id = 0, string a = "", int score = 0)
        : postID(id), author(a), engagementScore(score) {}

    bool operator<(const Post& other) const {
        return engagementScore < other.engagementScore;
    }
    bool operator>(const Post& other) const {
        return engagementScore > other.engagementScore;
    }
    bool operator==(const Post& other) const {
        return engagementScore == other.engagementScore;
    }
};

// ----------------------------
// BST Node
// ----------------------------
template <typename T>
struct Node {
    T data;
    Node* left;
    Node* right;
    Node(const T& d) : data(d), left(nullptr), right(nullptr) {}
};

// ----------------------------
// BST Class (Recursive Implementation)
// ----------------------------
template <typename T>
class BST {
private:
    Node<T>* root;

    // Recursive insert
    Node<T>* insert(Node<T>* node, const T& data) {
        if (!node) return new Node<T>(data);
        if (data < node->data)
            node->left = insert(node->left, data);
        else if (data > node->data)
            node->right = insert(node->right, data);
        return node;
    }

    // Find minimum node
    Node<T>* findMin(Node<T>* node) const {
        if (!node) return nullptr;
        return node->left ? findMin(node->left) : node;
    }

    // Find maximum node
    Node<T>* findMax(Node<T>* node) const {
        if (!node) return nullptr;
        return node->right ? findMax(node->right) : node;
    }

    // Sum engagement in range
    int sumInRange(Node<T>* node, int minS, int maxS) const {
        if (!node) return 0;
        int sum = 0;
        if (node->data.engagementScore >= minS && node->data.engagementScore <= maxS)
            sum += node->data.engagementScore;
        if (node->data.engagementScore > minS)
            sum += sumInRange(node->left, minS, maxS);
        if (node->data.engagementScore < maxS)
            sum += sumInRange(node->right, minS, maxS);
        return sum;
    }

    // Reverse inorder for Top K
    void reverseInorder(Node<T>* node, vector<T>& result, int k) const {
        if (!node || result.size() >= k) return;
        reverseInorder(node->right, result, k);
        if (result.size() < k) {
            result.push_back(node->data);
            reverseInorder(node->left, result, k);
        }
    }

    // Find node
    Node<T>* find(Node<T>* node, const T& data) const {
        if (!node) return nullptr;
        if (data == node->data) return node;
        if (data < node->data) return find(node->left, data);
        return find(node->right, data);
    }

    // Find successor
    Node<T>* findSuccessor(Node<T>* root, const T& data) const {
        Node<T>* curr = find(root, data);
        if (!curr) return nullptr;

        // Case 1: right subtree exists
        if (curr->right) return findMin(curr->right);

        // Case 2: find nearest ancestor for which current node is in left subtree
        Node<T>* successor = nullptr;
        Node<T>* ancestor = root;
        while (ancestor != curr) {
            if (data < ancestor->data) {
                successor = ancestor;
                ancestor = ancestor->left;
            } else
                ancestor = ancestor->right;
        }
        return successor;
    }

    // Find predecessor
    Node<T>* findPredecessor(Node<T>* root, const T& data) const {
        Node<T>* curr = find(root, data);
        if (!curr) return nullptr;

        if (curr->left) return findMax(curr->left);

        Node<T>* predecessor = nullptr;
        Node<T>* ancestor = root;
        while (ancestor != curr) {
            if (data > ancestor->data) {
                predecessor = ancestor;
                ancestor = ancestor->right;
            } else
                ancestor = ancestor->left;
        }
        return predecessor;
    }

    // Delete nodes in range
    Node<T>* deleteInRange(Node<T>* node, int minS, int maxS, bool& deleted) {
        if (!node) return nullptr;

        node->left = deleteInRange(node->left, minS, maxS, deleted);
        node->right = deleteInRange(node->right, minS, maxS, deleted);

        if (node->data.engagementScore >= minS && node->data.engagementScore <= maxS) {
            deleted = true;
            if (!node->left) {
                Node<T>* r = node->right;
                delete node;
                return r;
            } else if (!node->right) {
                Node<T>* l = node->left;
                delete node;
                return l;
            } else {
                Node<T>* minNode = findMin(node->right);
                node->data = minNode->data;
                node->right = deleteInRange(node->right, minNode->data.engagementScore, minNode->data.engagementScore, deleted);
            }
        }
        return node;
    }

    // Recursive destructor helper
    void clear(Node<T>* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

public:
    BST() : root(nullptr) {}
    ~BST() { clear(root); }

    bool insert(const T& data) {
        root = insert(root, data);
        return true;
    }

    Node<T>* findMin() const { return findMin(root); }
    Node<T>* findMax() const { return findMax(root); }

    int sumInRange(int minScore, int maxScore) const {
        return sumInRange(root, minScore, maxScore);
    }

    vector<T> getTopK(int k) const {
        vector<T> result;
        reverseInorder(root, result, k);
        return result;
    }

    Node<T>* findSuccessor(const T& data) const {
        return findSuccessor(root, data);
    }

    Node<T>* findPredecessor(const T& data) const {
        return findPredecessor(root, data);
    }

    bool deleteInRange(int minScore, int maxScore) {
        bool deleted = false;
        root = deleteInRange(root, minScore, maxScore, deleted);
        return deleted;
    }

    // Iterative inorder traversal
    void printInorderIterative() const {
        stack<Node<T>*> st;
        Node<T>* curr = root;
        while (curr || !st.empty()) {
            while (curr) {
                st.push(curr);
                curr = curr->left;
            }
            curr = st.top();
            st.pop();
            cout << "PostID: " << curr->data.postID
                 << " | Author: " << curr->data.author
                 << " | Engagement: " << curr->data.engagementScore << endl;
            curr = curr->right;
        }
    }
};

// ----------------------------
// Example usage
// ----------------------------
int main() {
    BST<Post> bst;
    bst.insert(Post(1, "Alice", 1500));
    bst.insert(Post(2, "Bob", 5000));
    bst.insert(Post(3, "Charlie", 800));
    bst.insert(Post(4, "David", 3000));
    bst.insert(Post(5, "Eve", 2000));

    cout << "\nAll posts (sorted):\n";
    bst.printInorderIterative();

    cout << "\nMin engagement: " << bst.findMin()->data.engagementScore << endl;
    cout << "Max engagement: " << bst.findMax()->data.engagementScore << endl;

    cout << "\nSum of engagement [1000, 4000]: " << bst.sumInRange(1000, 4000) << endl;

    cout << "\nTop 3 posts:\n";
    for (auto& p : bst.getTopK(3))
        cout << p.author << " (" << p.engagementScore << ")\n";

    Post target(0, "", 2000);
    auto succ = bst.findSuccessor(target);
    auto pred = bst.findPredecessor(target);
    cout << "\nSuccessor of 2000: " << (succ ? succ->data.engagementScore : -1) << endl;
    cout << "Predecessor of 2000: " << (pred ? pred->data.engagementScore : -1) << endl;

    cout << "\nDeleting posts in [1500, 3000]...\n";
    bst.deleteInRange(1500, 3000);

    cout << "\nRemaining posts:\n";
    bst.printInorderIterative();

    return 0;
}
