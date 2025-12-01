// Q2_HuffmanFixedArray.cpp
#include <bits/stdc++.h>
using namespace std;

const int MAXSYMS = 256;
const int MAXNODES = MAXSYMS * 2; // safe

struct Node {
    char ch;
    long long freq;
    int left, right;
    bool active;
    Node() : ch(0), freq(0), left(-1), right(-1), active(false) {}
};

Node nodeArray[MAXNODES];
string codeTable[256];
int nodeCount = 0;

int newNode(char c, long long f, int l=-1, int r=-1) {
    nodeArray[nodeCount].ch = c;
    nodeArray[nodeCount].freq = f;
    nodeArray[nodeCount].left = l;
    nodeArray[nodeCount].right = r;
    nodeArray[nodeCount].active = true;
    return nodeCount++;
}

void buildInitial(const string &s) {
    long long freq[256] = {0};
    for (unsigned char c : s) freq[c]++;
    nodeCount = 0;
    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) newNode((char)i, freq[i], -1, -1);
    }
    if (nodeCount == 0) {
        // empty input: create a dummy
        newNode('\0', 1, -1, -1);
    }
}

int buildHuffmanTree() {
    if (nodeCount == 1) return 0;
    while (true) {
        // count active nodes
        int activeCount = 0;
        for (int i = 0; i < nodeCount; ++i) if (nodeArray[i].active) activeCount++;
        if (activeCount <= 1) {
            // find root index
            for (int i = 0; i < nodeCount; ++i) if (nodeArray[i].active) return i;
        }
        // find two minimum active nodes
        int min1 = -1, min2 = -1;
        for (int i = 0; i < nodeCount; ++i) {
            if (!nodeArray[i].active) continue;
            if (min1 == -1 || nodeArray[i].freq < nodeArray[min1].freq) {
                min2 = min1;
                min1 = i;
            } else if (min2 == -1 || nodeArray[i].freq < nodeArray[min2].freq) {
                min2 = i;
            }
        }
        // create parent node combining min1 and min2
        long long fsum = nodeArray[min1].freq + nodeArray[min2].freq;
        int left = min1;
        int right = min2;
        // mark children inactive (they remain as nodes but not considered active leaves)
        nodeArray[min1].active = false;
        nodeArray[min2].active = false;
        // allocate new parent node
        newNode('\0', fsum, left, right);
        // continue until only one active node (the root) remains
    }
}

void buildCodesRec(int idx, string path) {
    if (idx < 0) return;
    if (nodeArray[idx].left == -1 && nodeArray[idx].right == -1) {
        // leaf
        unsigned char uc = (unsigned char)nodeArray[idx].ch;
        codeTable[uc] = (path.empty() ? "0" : path); // edge-case: single symbol -> "0"
        return;
    }
    if (nodeArray[idx].left != -1) buildCodesRec(nodeArray[idx].left, path + "0");
    if (nodeArray[idx].right != -1) buildCodesRec(nodeArray[idx].right, path + "1");
}

void generateCodes(int root) {
    for (int i = 0; i < 256; ++i) codeTable[i].clear();
    buildCodesRec(root, "");
}

string encodeString(const string &s) {
    string out;
    for (unsigned char c : s) {
        out += codeTable[c];
    }
    return out;
}

// Demo
int main() {
    string input;
    cout << "Enter input string to compress: ";
    getline(cin, input);
    buildInitial(input);
    int root = buildHuffmanTree();
    generateCodes(root);
    cout << "Codes generated (for chars present):\n";
    for (int i = 0; i < 256; ++i) {
        if (!codeTable[i].empty()) {
            if (isprint(i)) cout << (char)i << " : ";
            else cout << int(i) << " : ";
            cout << codeTable[i] << "\n";
        }
    }
    string compressed = encodeString(input);
    cout << "Compressed bit stream:\n" << compressed << "\n";
    return 0;
}
