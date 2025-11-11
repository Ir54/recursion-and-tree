#include <iostream>
#include <string>
#include <stdexcept>
using namespace std;

// ======================================
// TEMPLATE STACK CLASS IMPLEMENTATION
// ======================================
template<typename T>
class Stack {
private:
    T* arr;
    int capacity;
    int count;
    int topIndex;
    int bottomIndex;
    bool flipped;

    void resize(int newCap) {
        T* newArr = new T[newCap];
        for (int i = 0; i < count; ++i)
            newArr[i] = arr[(bottomIndex + i) % capacity];
        delete[] arr;
        arr = newArr;
        capacity = newCap;
        bottomIndex = 0;
        topIndex = count - 1;
    }

public:
    Stack(int cap = 5) {
        capacity = cap;
        arr = new T[capacity];
        count = 0;
        bottomIndex = 0;
        topIndex = -1;
        flipped = false;
    }

    ~Stack() { delete[] arr; }

    bool isEmpty() const { return count == 0; }
    bool isFull() const { return count == capacity; }

    void push(const T& element) {
        if (isFull()) resize(capacity * 2);
        if (!flipped)
            topIndex = (bottomIndex + count) % capacity;
        else
            bottomIndex = (bottomIndex - 1 + capacity) % capacity;

        arr[flipped ? bottomIndex : topIndex] = element;
        count++;
    }

    T pop() {
        if (isEmpty()) throw runtime_error("Stack Underflow");

        T element;
        if (!flipped) {
            element = arr[topIndex];
            topIndex = (topIndex - 1 + capacity) % capacity;
        } else {
            element = arr[bottomIndex];
            bottomIndex = (bottomIndex + 1) % capacity;
        }
        count--;

        if (count > 0 && count <= capacity / 2)
            resize(max(5, capacity / 2));

        return element;
    }

    T top() const {
        if (isEmpty()) throw runtime_error("Stack Empty");
        return flipped ? arr[bottomIndex] : arr[topIndex];
    }

    T peek() const { return top(); }

    void flipStack() { flipped = !flipped; }

    int size() const { return count; }
};

// ======================================
// MYSTRING CLASS IMPLEMENTATION
// ======================================
class MyString {
private:
    string current;
    Stack<string> undoStack;
    Stack<string> redoStack;
    const int MAX_HISTORY = 5;

public:
    MyString() {}

    void printString() {
        cout << "\nCurrent String: \"" << current << "\"\n";
    }

    void addWord(const string& word) {
        // Maintain last 5 undos
        if (undoStack.size() == MAX_HISTORY) undoStack.pop();
        undoStack.push(current);

        // Append word(s)
        current += (current.empty() ? "" : " ") + word;

        // Clear redo history
        while (!redoStack.isEmpty()) redoStack.pop();

        cout << "Word(s) added successfully.\n";
    }

    void undo() {
        if (undoStack.isEmpty()) {
            cout << "Nothing to undo!\n";
            return;
        }
        redoStack.push(current);
        current = undoStack.pop();
        cout << "Undo successful.\n";
    }

    void redo() {
        if (redoStack.isEmpty()) {
            cout << "Nothing to redo!\n";
            return;
        }
        undoStack.push(current);
        current = redoStack.pop();
        cout << "Redo successful.\n";
    }
};

// ======================================
// MAIN FUNCTION WITH INTERACTIVE MENU
// ======================================
int main() {
    MyString editor;
    int choice;
    string input;

    while (true) {
        cout << "\n========== TEXT EDITOR MENU ==========\n";
        cout << "1. Print String\n";
        cout << "2. Add Word(s)\n";
        cout << "3. Undo Last Word(s)\n";
        cout << "4. Redo Last Word(s)\n";
        cout << "5. Test flipStack() (Stack Inversion Demo)\n";
        cout << "6. Exit\n";
        cout << "======================================\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            editor.printString();
            break;
        case 2:
            cout << "Enter word(s) to add: ";
            getline(cin, input);
            editor.addWord(input);
            break;
        case 3:
            editor.undo();
            break;
        case 4:
            editor.redo();
            break;
        case 5: {
            cout << "\n--- flipStack() Demonstration ---\n";
            Stack<int> demo;
            for (int i = 1; i <= 5; i++) demo.push(i);

            cout << "Original Stack (Top -> Bottom): 5 4 3 2 1\n";
            demo.flipStack();
            cout << "After flipStack(): popping elements -> ";
            while (!demo.isEmpty())
                cout << demo.pop() << " ";
            cout << "\n---------------------------------\n";
            break;
        }
        case 6:
            cout << "Exiting program...\n";
            return 0;
        default:
            cout << "Invalid choice! Try again.\n";
        }
    }
}