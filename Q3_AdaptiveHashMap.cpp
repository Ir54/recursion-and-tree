// Q3_AdaptiveHashMap.cpp
#include <bits/stdc++.h>
using namespace std;

struct Entry {
    string key;
    string value;
    Entry() {}
    Entry(string k, string v) : key(k), value(v) {}
};

struct TreeNode {
    Entry e;
    TreeNode *l, *r;
    TreeNode(const Entry &en) : e(en), l(nullptr), r(nullptr) {}
};

class Bucket {
public:
    bool isTree;
    list<Entry> lst; // used when not tree
    TreeNode *root;  // used when tree
    int count;
    int threshold;

    Bucket(int k=5) : isTree(false), root(nullptr), count(0), threshold(k) {}

    ~Bucket() {
        clearTree(root);
    }
    void clearTree(TreeNode* node) {
        if (!node) return;
        clearTree(node->l);
        clearTree(node->r);
        delete node;
    }

    void listInsertOrUpdate(const Entry &en) {
        for (auto &x : lst) {
            if (x.key == en.key) {
                x.value = en.value;
                return;
            }
        }
        lst.push_back(en);
        count++;
        if (!isTree && count > threshold) treeify();
    }

    string listSearch(const string &key) {
        for (auto &x : lst) if (x.key == key) return x.value;
        return "";
    }

    bool listDelete(const string &key) {
        for (auto it = lst.begin(); it != lst.end(); ++it) {
            if (it->key == key) {
                lst.erase(it);
                count--;
                return true;
            }
        }
        return false;
    }

    // Build BST from list
    void treeify() {
        if (isTree) return;
        root = nullptr;
        for (auto &e : lst) {
            root = treeInsert(root, e);
        }
        lst.clear();
        isTree = true;
    }

    // Convert tree back to list
    void listify() {
        if (!isTree) return;
        lst.clear();
        inorderToList(root, lst);
        clearTree(root);
        root = nullptr;
        isTree = false;
    }

    TreeNode* treeInsert(TreeNode* node, const Entry &e) {
        if (!node) {
            count++;
            return new TreeNode(e);
        }
        if (e.key < node->e.key) node->l = treeInsert(node->l, e);
        else if (e.key > node->e.key) node->r = treeInsert(node->r, e);
        else node->e.value = e.value; // update
        return node;
    }

    string treeSearch(const string &key) {
        TreeNode *cur = root;
        while (cur) {
            if (key == cur->e.key) return cur->e.value;
            if (key < cur->e.key) cur = cur->l;
            else cur = cur->r;
        }
        return "";
    }

    bool treeDelete(const string &key) {
        bool deleted = false;
        root = treeRemove(root, key, deleted);
        if (deleted) count--;
        return deleted;
    }

    TreeNode* treeRemove(TreeNode* node, const string &key, bool &deleted) {
        if (!node) return nullptr;
        if (key < node->e.key) node->l = treeRemove(node->l, key, deleted);
        else if (key > node->e.key) node->r = treeRemove(node->r, key, deleted);
        else {
            deleted = true;
            // remove this node
            if (!node->l) {
                TreeNode* r = node->r;
                delete node;
                return r;
            } else if (!node->r) {
                TreeNode* l = node->l;
                delete node;
                return l;
            } else {
                // two children: find inorder successor
                TreeNode *succ = node->r;
                while (succ->l) succ = succ->l;
                node->e = succ->e;
                node->r = treeRemove(node->r, succ->e.key, deleted);
                // 'deleted' remains true
            }
        }
        return node;
    }

    void inorderToList(TreeNode* node, list<Entry> &out) {
        if (!node) return;
        inorderToList(node->l, out);
        out.push_back(node->e);
        inorderToList(node->r, out);
    }
};

class AdaptiveHashMap {
private:
    int capacity;
    int threshold;
    vector<Bucket> buckets;

    int hashKey(const string &s) {
        // simple polynomial hash -> index
        unsigned long long h = 1469598103934665603ULL;
        for (char c : s) h = (h * 1099511628211ULL) ^ (unsigned char)c;
        return (int)(h % capacity);
    }

public:
    AdaptiveHashMap(int cap, int k) : capacity(cap), threshold(k), buckets(cap) {
        for (int i = 0; i < cap; ++i) buckets[i] = Bucket(k);
    }

    void insert(const string &key, const string &value) {
        int idx = hashKey(key);
        Bucket &b = buckets[idx];
        if (b.isTree) {
            // insert/update in tree
            b.treeInsert(b.root, Entry(key, value));
        } else {
            // list mode
            // if exists update, else push
            bool found = false;
            for (auto &e : b.lst) {
                if (e.key == key) {
                    e.value = value;
                    found = true; break;
                }
            }
            if (!found) {
                b.lst.emplace_back(key, value);
                b.count++;
                if (b.count > threshold) b.treeify();
            }
        }
    }

    string search(const string &key) {
        int idx = hashKey(key);
        Bucket &b = buckets[idx];
        if (b.isTree) return b.treeSearch(key);
        else return b.listSearch(key);
    }

    void remove(const string &key) {
        int idx = hashKey(key);
        Bucket &b = buckets[idx];
        bool deleted = false;
        if (b.isTree) {
            deleted = b.treeDelete(key);
            if (deleted && b.count <= threshold) {
                b.listify();
            }
        } else {
            deleted = b.listDelete(key);
            if (deleted && b.count > threshold && b.count <= threshold) {
                // not applicable
            }
        }
    }
};

// Demo
int main() {
    AdaptiveHashMap map(50, 5);
    // Insert many keys mapping to same bucket (for demo you may craft keys that hash collide)
    map.insert("file1.txt", "/path/file1");
    map.insert("file2.txt", "/path/file2");
    cout << "search file1.txt: " << map.search("file1.txt") << "\n";
    map.insert("file1.txt", "/new/path/file1");
    cout << "search file1.txt after update: " << map.search("file1.txt") << "\n";
    map.remove("file1.txt");
    cout << "after delete file1.txt: " << map.search("file1.txt") << "\n";
    return 0;
}
