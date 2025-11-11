#include <iostream>
#include <queue>
#include <vector>
#include <functional>
using namespace std;

// ----------------- Book class -----------------
class Book {
public:
    long long ISBN;         // 13-digit primary key
    string title;
    string author;
    int publicationYear;
    int copies;
    bool isRareEdition;
    string category;

    Book() = default;
    Book(long long isbn, string t="", string a="", int y=0, int c=1, bool rare=false, string cat="")
        : ISBN(isbn), title(std::move(t)), author(std::move(a)),
          publicationYear(y), copies(c), isRareEdition(rare), category(std::move(cat)) {}

    // Comparison operators based on ISBN
    bool operator<(const Book& other) const { return ISBN < other.ISBN; }
    bool operator>(const Book& other) const { return ISBN > other.ISBN; }
    bool operator==(const Book& other) const { return ISBN == other.ISBN; }
};

// ----------------- Node and BST templates -----------------
template<typename T>
struct Node {
    T data;
    Node* left;
    Node* right;
    Node(const T& d) : data(d), left(nullptr), right(nullptr) {}
};

template<typename T>
class BST {
private:
    Node<T>* root;

    // Recursive helpers
    Node<T>* insertRec(Node<T>* node, const T& data, bool &insertedNew) {
        if (!node) {
            insertedNew = true;
            return new Node<T>(data);
        }
        if (data < node->data) {
            node->left = insertRec(node->left, data, insertedNew);
        } else if (data > node->data) {
            node->right = insertRec(node->right, data, insertedNew);
        } else { // same ISBN: update copies and other info if needed
            // We'll increment copies (assume incoming data.copies is number of copies to add).
            // Also update metadata if you prefer to replace title/author etc.
            node->data.copies += data.copies;
            insertedNew = false;
        }
        return node;
    }

    Node<T>* findMin(Node<T>* node) {
        while (node && node->left) node = node->left;
        return node;
    }

    Node<T>* removeRec(Node<T>* node, const T& data, bool &removed) {
        if (!node) return nullptr;
        if (data < node->data) {
            node->left = removeRec(node->left, data, removed);
        } else if (data > node->data) {
            node->right = removeRec(node->right, data, removed);
        } else { // found node to delete
            removed = true;
            // Case 1: no child or single child
            if (!node->left) {
                Node<T>* r = node->right;
                delete node;
                return r;
            } else if (!node->right) {
                Node<T>* l = node->left;
                delete node;
                return l;
            } else {
                // Case 2: two children - replace with inorder successor
                Node<T>* succ = findMin(node->right);
                node->data = succ->data;
                node->right = removeRec(node->right, succ->data, removed /*will set true again, but fine*/);
            }
        }
        return node;
    }

    Node<T>* searchRec(Node<T>* node, const T& data) const {
        if (!node) return nullptr;
        if (data == node->data) return node;
        if (data < node->data) return searchRec(node->left, data);
        return searchRec(node->right, data);
    }

    int countNodesRec(Node<T>* node) const {
        if (!node) return 0;
        return 1 + countNodesRec(node->left) + countNodesRec(node->right);
    }

    int countLeavesRec(Node<T>* node) const {
        if (!node) return 0;
        if (!node->left && !node->right) return 1;
        return countLeavesRec(node->left) + countLeavesRec(node->right);
    }

    int maxDepthRec(Node<T>* node) const {
        if (!node) return 0;
        int l = maxDepthRec(node->left);
        int r = maxDepthRec(node->right);
        return 1 + max(l, r);
    }

    Node<T>* findLCARec(Node<T>* node, long long isbn1, long long isbn2) const {
        if (!node) return nullptr;
        long long curr = node->data.ISBN;
        if (curr > isbn1 && curr > isbn2) return findLCARec(node->left, isbn1, isbn2);
        if (curr < isbn1 && curr < isbn2) return findLCARec(node->right, isbn1, isbn2);
        // Otherwise current node is the split point
        return node;
    }

    long long countCopiesRangeRec(Node<T>* node, long long low, long long high) const {
        if (!node) return 0;
        long long sum = 0;
        if (node->data.ISBN >= low && node->data.ISBN <= high) {
            sum += node->data.copies;
        }
        if (node->data.ISBN > low) sum += countCopiesRangeRec(node->left, low, high);
        if (node->data.ISBN < high) sum += countCopiesRangeRec(node->right, low, high);
        return sum;
    }

    void inorderTraverse(Node<T>* node, function<void(const T&)> fn) const {
        if (!node) return;
        inorderTraverse(node->left, fn);
        fn(node->data);
        inorderTraverse(node->right, fn);
    }

    void deleteRec(Node<T>* node) {
        if (!node) return;
        deleteRec(node->left);
        deleteRec(node->right);
        delete node;
    }

public:
    BST() : root(nullptr) {}
    ~BST() { clear(); }

    // Insert: returns true if a new node was created; false if ISBN existed and copies were updated
    bool insert(const T& data) {
        bool insertedNew = false;
        root = insertRec(root, data, insertedNew);
        return insertedNew;
    }

    // Remove: returns true if node removed, false if not found
    bool remove(const T& data) {
        bool removed = false;
        root = removeRec(root, data, removed);
        return removed;
    }

    // Search by exact ISBN (data.ISBN must be set)
    Node<T>* search(const T& data) const {
        return searchRec(root, data);
    }

    int totalUniqueBooks() const {
        return countNodesRec(root);
    }

    // Count leaf nodes (standalone/rare editions might be leaves)
    int countLeafNodes() const {
        return countLeavesRec(root);
    }

    int maxDepth() const {
        return maxDepthRec(root);
    }

    // Find LCA node for two ISBNs (pass ISBNs). Returns Node* or nullptr if not found.
    Node<T>* findLCA(long long isbn1, long long isbn2) const {
        if (!root) return nullptr;
        // Ensure isbn1 <= isbn2 for simpler comparisons
        if (isbn1 > isbn2) swap(isbn1, isbn2);
        return findLCARec(root, isbn1, isbn2);
    }

    // Merge another BST into this one (in-order traversal of other and insert each book)
    void mergeFrom(const BST<T>& other) {
        other.inorderTraverse(other.root, [this](const T& book) {
            this->insert(book); // insert handles copies merging
        });
    }

    // Sum copies in ISBN inclusive range
    long long countCopiesInRange(long long lowISBN, long long highISBN) const {
        if (lowISBN > highISBN) swap(lowISBN, highISBN);
        return countCopiesRangeRec(root, lowISBN, highISBN);
    }

    // Level-by-level print (hierarchy)
    void displayCatalogHierarchy() const {
        if (!root) {
            cout << "[Empty catalog]\n";
            return;
        }
        queue<Node<T>*> q;
        q.push(root);
        while (!q.empty()) {
            size_t levelSize = q.size();
            cout << "Level " << (/*just print level marker*/ (int)0) << ": "; // we will print visually; to get actual depth numbers we'd need a counter
            // Print items of current level
            for (size_t i = 0; i < levelSize; ++i) {
                Node<T>* cur = q.front(); q.pop();
                cout << "(ISBN: " << cur->data.ISBN << ", title: " << cur->data.title << ", copies: " << cur->data.copies << ")  ";
                if (cur->left) q.push(cur->left);
                if (cur->right) q.push(cur->right);
            }
            cout << "\n";
        }
    }

    // Better display with level numbering (more informative)
    void displayCatalogHierarchyWithLevels() const {
        if (!root) {
            cout << "[Empty catalog]\n";
            return;
        }
        queue<Node<T>*> q;
        q.push(root);
        int level = 0;
        while (!q.empty()) {
            size_t levelSize = q.size();
            cout << "Level " << level << " -> ";
            for (size_t i = 0; i < levelSize; ++i) {
                Node<T>* cur = q.front(); q.pop();
                cout << "[" << cur->data.ISBN << " | " << cur->data.title << " | c:" << cur->data.copies << "] ";
                if (cur->left) q.push(cur->left);
                if (cur->right) q.push(cur->right);
            }
            cout << "\n";
            ++level;
        }
    }

    // Clear entire tree
    void clear() {
        deleteRec(root);
        root = nullptr;
    }

    // Expose root for internal use (const)
    Node<T>* getRoot() const { return root; }

    // Utility: print inorder (for debugging)
    void printInorder() const {
        inorderTraverse(root, [](const T& book) {
            cout << book.ISBN << " (" << book.title << ", copies=" << book.copies << ")\n";
        });
    }
};

// ----------------- Example usage -----------------
int main() {
    BST<Book> catalog;

    // Insert books (ISBNs are illustrative)
    catalog.insert(Book(9780131103627, "The C Programming Language", "Kernighan & Ritchie", 1988, 5, false, "Computer Science"));
    catalog.insert(Book(9780262033848, "Introduction to Algorithms", "Cormen et al.", 2009, 3, false, "Computer Science"));
    catalog.insert(Book(9780134093413, "Clean Code", "Robert C. Martin", 2008, 2, false, "Programming"));
    catalog.insert(Book(9780201616224, "The Pragmatic Programmer", "Andrew Hunt", 1999, 4, false, "Programming"));
    catalog.insert(Book(9780672329784, "Design Patterns", "Gamma et al.", 1994, 1, true, "Software Engineering"));

    // Insert copies for an existing ISBN (adds copies)
    catalog.insert(Book(9780201616224, "", "", 0, 1)); // adds 1 more copy to Pragmatic Programmer

    cout << "Total unique books: " << catalog.totalUniqueBooks() << "\n";
    cout << "Standalone (leaf) books: " << catalog.countLeafNodes() << "\n";
    cout << "Max depth: " << catalog.maxDepth() << "\n\n";

    cout << "Catalog (inorder):\n";
    catalog.printInorder();
    cout << "\nCatalog hierarchy (by level):\n";
    catalog.displayCatalogHierarchyWithLevels();

    // Search
    Book probe; probe.ISBN = 9780131103627;
    Node<Book>* found = catalog.search(probe);
    if (found) {
        cout << "\nFound: " << found->data.title << " | copies: " << found->data.copies << "\n";
    }

    // Count copies in range
    long long sumCopies = catalog.countCopiesInRange(9780130000000LL, 9780269999999LL);
    cout << "\nTotal copies in ISBN range: " << sumCopies << "\n";

    // Find LCA between two ISBNs
    long long a = 9780131103627, b = 9780134093413;
    Node<Book>* lca = catalog.findLCA(a, b);
    if (lca) cout << "LCA ISBN: " << lca->data.ISBN << " (" << lca->data.title << ")\n";

    // Remove a book
    bool removed = catalog.remove(Book(9780672329784));
    cout << "\nRemoved Design Patterns? " << (removed ? "yes" : "no") << "\n";
    cout << "After removal, total unique: " << catalog.totalUniqueBooks() << "\n";

    // Merge example: create another small BST and merge
    BST<Book> branch;
    branch.insert(Book(9780132350884, "Clean Architecture", "Robert C. Martin", 2017, 2));
    branch.insert(Book(9781491950357, "Designing Data-Intensive Applications", "Martin Kleppmann", 2017, 2));
    catalog.mergeFrom(branch);
    cout << "\nAfter merge, total unique: " << catalog.totalUniqueBooks() << "\n";
    cout << "Final catalog (inorder):\n"; catalog.printInorder();

    return 0;
}
