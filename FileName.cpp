#include <iostream>
#include <fstream>
#include <stack>
#include <string>
#include <sstream>
#include <windows.h>

using namespace std;

// Node structure for the doubly linked list
struct Node {
    string sentence;
    Node* next;
    Node* prev;
    Node(const string& s) : sentence(s), next(nullptr), prev(nullptr) {}
};

// Class to represent the text editor
class TextEditor {
private:
    Node* head;  // Pointer to the head of the linked list
    Node* tail;  // Pointer to the tail of the linked list
    stack<Node*> undoStack; // Stack to keep track of operations for undo
    stack<Node*> redoStack; // Stack to keep track of operations for redo

public:
    TextEditor() : head(nullptr), tail(nullptr) {}

    // Function to insert a sentence at the end
    void insert(const string& sentence) {
        Node* newNode = new Node(sentence);
        if (!head) 
        {
            head = tail = newNode;
        }
        else 
        {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        undoStack.push(newNode);
        // Clear the redo stack on new insertion
        while (!redoStack.empty()) redoStack.pop();
    }

    // Function to delete the last sentence
    void deleteLast()
    {
        if (!tail) return;  // Check if the text editor is empty
        Node* temp = tail; 
        if (tail->prev) {  
            tail = tail->prev;  
            tail->next = nullptr;  //Set the next pointer of the new last node to nullptr
        }
        else {
            head = tail = nullptr;  // If there is only one node, set both head and tail to nullptr
        }
        undoStack.push(temp);  // Push the deleted node onto the undo stack
        // Clear the redo stack on new deletion
        while (!redoStack.empty()) redoStack.pop();  // Clear the redo stack
    }


    void undo() {
        if (undoStack.empty()) return;  
        Node* lastOperation = undoStack.top(); 
        undoStack.pop();  
        redoStack.push(lastOperation); 

        if (lastOperation->next == nullptr && lastOperation->prev == nullptr) {
            if (!tail) {
                head = tail = lastOperation;  // Restore deleted node as the only node in the list
            }
            else {
                tail->next = lastOperation;  // Restore deleted node as the new last node
                lastOperation->prev = tail;  // Set the previous pointer of the deleted node to the old last node
                tail = lastOperation;  // Update tail to point to the restored last node
            }
        }
        else {
  
            if (lastOperation == tail) {
                tail = tail->prev;  // Revert insertion by moving tail pointer to the previous node
                if (tail) 
                    tail->next = nullptr;  // Set the next pointer of the new last node to nullptr
                else 
                    head = nullptr;  // If there is no previous node, set head to nullptr
            }
            lastOperation->next = nullptr;  // Set next pointer of the inserted node to nullptr
            lastOperation->prev = nullptr;  // Set prev pointer of the inserted node to nullptr
        }
    }


    void redo() {
        if (redoStack.empty()) return;  
        Node* lastUndo = redoStack.top();  // Get the last undone operation from the redo stack
        redoStack.pop();  // Pop the last undone operation

        // Redo the operation based on its type
        if (lastUndo->next == nullptr && lastUndo->prev == nullptr) {
            // It was a delete operation, redo it
            if (!tail) {
                head = tail = lastUndo;  // Restore deleted node as the only node in the list
            }
            else {
                tail->next = lastUndo;  // Restore deleted node as the new last node
                lastUndo->prev = tail;  // Set the previous pointer of the deleted node to the old last node
                tail = lastUndo;  // Update tail to point to the restored last node
            }
        }
        else {
            if (lastUndo == tail) {
                tail = tail->prev;  // Revert insertion by moving tail pointer to the previous node
                if (tail) tail->next = nullptr;  // Set the next pointer of the new last node to nullptr
                else head = nullptr;  // If there is no previous node, set head to nullptr
            }
            lastUndo->next = nullptr; 
            lastUndo->prev = nullptr; 
        }

        undoStack.push(lastUndo);  // Push the redone operation onto the undo stack
    }


    // Function to display the current text
    void display() {
        Node* temp = head;
        while (temp) {
            cout << temp->sentence << endl;
            temp = temp->next;
        }
    }

    // Function to clear the editor
    void clear() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        head = tail = nullptr;
        while (!undoStack.empty()) undoStack.pop();
        while (!redoStack.empty()) redoStack.pop();
    }

    // Function to save text to a file
    void saveToFile(const string& filename) {
        ofstream outputFile(filename);
        if (outputFile.is_open()) {
            Node* temp = head;
            while (temp) {
                outputFile << temp->sentence << endl;
                temp = temp->next;
            }
            outputFile.close();
            cout << "Text saved to file: " << filename << endl;
        }
        else {
            cout << "Error: Unable to open file for writing." << endl;
        }
    }

    // Function to find and replace text
    void findAndReplace(const string& findStr, const string& replaceStr) {
        Node* temp = head;
        while (temp) {
            size_t pos = temp->sentence.find(findStr);
            while (pos != string::npos) {
                temp->sentence.replace(pos, findStr.length(), replaceStr);
                pos = temp->sentence.find(findStr, pos + replaceStr.length());
            }
            temp = temp->next;
        }
    }

    // Function to get text as a string
    string getText() {
        string text;
        Node* temp = head;
        while (temp) {
            text += temp->sentence + "\n";
            temp = temp->next;
        }
        return text;
    }

    // Function to set text from a string
    void setText(const string& text) {
        clear();
        istringstream iss(text);
        string sentence;
        while (getline(iss, sentence)) {
            insert(sentence);
        }
    }

    // Function to count words
    int countWords() {
        string text = getText();
        stringstream ss(text); //tokenize complete text into words
        string word;
        int count = 0;
        while (ss >> word) {
            count++;
        }
        return count;
    }

    // Function to count the number of sentences
    int countSentences() {
        int count = 0;
        Node* temp = head;
        while (temp) {
            count++;
            temp = temp->next;
        }
        return count;
    }

    // Function to go to a specific line
    void goToLine(int lineNumber) {
        int count = 1;
        Node* temp = head;
        while (temp && count < lineNumber) {
            temp = temp->next;
            count++;
        }
        if (temp) { 
            cout << "Line " << lineNumber << ": " << temp->sentence << endl;
        }
        else {
            cout << "Line number out of range." << endl;
        }
    }

    // Function to display the menu
    void displayMenu() {
        cout << "\n==================== Text Editor ====================\n";
        cout << "1. Insert Sentence\n";
        cout << "2. Delete Last Sentence\n";
        cout << "3. Undo\n";
        cout << "4. Redo\n";
        cout << "5. Display Text\n";
        cout << "6. Save Text to File\n";
        cout << "7. Find and Replace\n";
        cout << "8. Count Sentences\n";
        cout << "9. Clear Text\n";
        cout << "10. Count Words\n";
        cout << "11. Go to Line\n";
        cout << "12. Exit\n";
        cout << "\n===================================================\n";
        cout << "Enter your choice: ";
    }
};

// Function to clear the console
void clearConsole() {
    system("cls");
}

// Function to display the welcome message
void displayWelcomeMessage() {
    system("color 70");
    cout << " \n\n\n\n\n\n\n\n\n\n\n\n ";
    cout << "\t\t\t\t==================================================================" << endl;
    cout << "\t\t\t\t                     Welcome To My Text Editor!        " << endl;
    cout << "\t\t\t\t==================================================================" << endl;
    cout << " \n\n\n\n ";
    Sleep(3000);
    clearConsole();
}

// Main function to run the text editor
int main() {
    TextEditor editor;
    int choice, lineNumber;
    string sentence, filename, findStr, replaceStr;

    displayWelcomeMessage();

    while (true) {
        clearConsole();
        editor.displayMenu();
        cin >> choice;

        switch (choice) {
        case 1:
            cout << "Enter sentence to insert: ";
            cin.ignore(); // Ignore newline character from previous input
            getline(cin, sentence);
            editor.insert(sentence);
            break;
        case 2:
            editor.deleteLast();
            break;
        case 3:
            editor.undo();
            break;
        case 4:
            editor.redo();
            break;
        case 5:
            clearConsole();
            editor.display();
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();
            break;
        case 6:
            cout << "Enter filename to save: ";
            cin >> filename;
            editor.saveToFile(filename);
            break;
        case 7:
            cout << "Enter text to find: ";
            cin.ignore();
            getline(cin, findStr);
            cout << "Enter text to replace with: ";
            getline(cin, replaceStr);
            editor.findAndReplace(findStr, replaceStr);
            break;
        case 8:
            cout << "Total number of sentences: " << editor.countSentences() << endl;
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();
            break;
        case 9:
            editor.clear();
            cout << "Text cleared." << endl;
            break;
        case 10:
            cout << "Total number of words: " << editor.countWords() << endl;
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();
            break;
        case 11:
            cout << "Enter line number to go to: ";
            cin >> lineNumber;
            editor.goToLine(lineNumber);
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();
            break;
        case 12:
            cout << "Exiting editor." << endl;
            return 0;
        default:
            cout << "Invalid choice. Please try again." << endl;
            break;
        }
    }

    return 0;
}
