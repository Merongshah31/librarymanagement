#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <ctime>
#include <limits>
#include <algorithm>
#include <iomanip>

using namespace std;

class Library {
private:
    // Structs definition
    struct book {
        string author;
        string title; 
        int year; 
        double price;
        bool available;
    };

    struct user {
        string name;
        string no_matric;
        int time_borrow;
        int time_return;
    };

    struct borrow_record {
        string no_matric;
        time_t borrow_time;
        time_t return_time;
        book borrowed_book;
        bool is_returned;   
    };

    struct Request {
        string user_matric;
        string book_title;
        time_t request_time;
        bool is_return;  // false for borrow, true for return
    };

     struct MemberNode {
        int id;
        string name;
        string contact;
        bool active;
        MemberNode* next;
        MemberNode() : next(nullptr) {}
    };

    struct BookNode {
        book bookData;
        BookNode* next;
        BookNode() : next(nullptr) {}
    };

    struct BorrowNode {
        borrow_record borrowData;
        BorrowNode* next;
        BorrowNode() : next(nullptr) {}
    };

    // Class member variables
    vector<book> books;
    vector<user> users;
    vector<borrow_record> records;
    queue<Request> requestQueue;
    MemberNode* memberHead;
    BookNode* bookHead;
    BorrowNode* borrowHead;
    int nextMemberId;

public:
    // Constructor
    Library() {
        memberHead = nullptr;
        bookHead = nullptr;
        borrowHead = nullptr;
        nextMemberId = 1000;
        loadBooks();
    }

    // Method to add a new book
    void addBook() {
        book newbook;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        cout << "Enter book title: ";
        getline(cin, newbook.title);
        
        cout << "Enter author: ";
        getline(cin, newbook.author);
        
        cout << "Enter year: ";
        while (!(cin >> newbook.year)) {
            cout << "Invalid input. Please enter a valid year: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
        cout << "Enter price: ";
        while (!(cin >> newbook.price)) {
            cout << "Invalid input. Please enter a valid price: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
        newbook.available = true;
        books.push_back(newbook);
        saveBook();
    }

    // Method to save books to file
    void saveBook() {
        ofstream file("books.txt");
        if (file.is_open()) {
            for (const auto& book : books) {
                file << book.title << "|"
                     << book.author << "|"
                     << book.year << "|"
                     << book.price << "|"
                     << book.available << "\n";
            }
            file.close();
            cout << "Books saved successfully!\n";
        } else {
            cout << "Error: Unable to save books.\n";
        }
    }

    // Method to load books from file
    void loadBooks() {
        ifstream file("books.txt");
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                book newbook;
                size_t pos = 0;
                string token;
                
                // Parse title
                pos = line.find("|");
                newbook.title = line.substr(0, pos);
                line.erase(0, pos + 1);
                
                // Parse author
                pos = line.find("|");
                newbook.author = line.substr(0, pos);
                line.erase(0, pos + 1);
                
                // Parse year
                pos = line.find("|");
                newbook.year = stoi(line.substr(0, pos));
                line.erase(0, pos + 1);
                
                // Parse price
                pos = line.find("|");
                newbook.price = stod(line.substr(0, pos));
                line.erase(0, pos + 1);
                
                // Parse availability
                newbook.available = (line == "1");
                
                books.push_back(newbook);
            }
            file.close();
            cout << "Books loaded successfully!\n";
        }
    }

    // Method to display books
    void displayBooks() {
        if (books.empty()) {
            cout << "No books in library.\n";
            return;
        }
        
        cout << "\nLibrary Books:\n";
        cout << "----------------------------------------\n";
        for (size_t i = 0; i < books.size(); i++) {
            cout << "Book #" << (i + 1) << "\n"
                 << "Title: " << books[i].title << "\n"
                 << "Author: " << books[i].author << "\n"
                 << "Year: " << books[i].year << "\n"
                 << "Price: RM 2" << fixed << setprecision(2) << books[i].price << "\n"
                 << "Status: " << (books[i].available ? "Available" : "Borrowed") << "\n"
                 << "----------------------------------------\n";
        }
    }

    // Method to request borrowing a book
    void requestBorrow(const string& matric, const string& title) {
        if (matric.empty() || title.empty()) {
            cout << "Error: Matric number and title cannot be empty.\n";
            return;
        }

        Request req;
        req.user_matric = matric;
        req.book_title = title;
        req.request_time = time(nullptr);
        req.is_return = false;
        
        requestQueue.push(req);
        cout << "Borrow request added to queue.\n";
    }

    // Method to request returning a book
    void requestReturn(const string& matric, const string& title) {
        if (matric.empty() || title.empty()) {
            cout << "Error: Matric number and title cannot be empty.\n";
            return;
        }

        Request req;
        req.user_matric = matric;
        req.book_title = title;
        req.request_time = time(nullptr);
        req.is_return = true;
        
        requestQueue.push(req);
        cout << "Return request added to queue.\n";
    }

    void viewBorrowingHistory() {
        // Implementation
    }

    void processNextRequest() {
        if (requestQueue.empty()) {
            cout << "No requests in queue.\n";
            return;
        }

        Request req = requestQueue.front();
        requestQueue.pop();

        if (req.is_return) {
            processReturn(req);
        } else {
            processBorrow(req);
        }
    }

    void displayQueue() {
        if (requestQueue.empty()) {
            cout << "Request queue is empty.\n";
            return;
        }

        queue<Request> temp = requestQueue;
        int count = 1;
        
        cout << "\nCurrent Request Queue:\n";
        cout << "----------------------------------------\n";
        while (!temp.empty()) {
            Request req = temp.front();
            cout << "Request #" << count++ << "\n"
                 << "User: " << req.user_matric << "\n"
                 << "Book: " << req.book_title << "\n"
                 << "Type: " << (req.is_return ? "Return" : "Borrow") << "\n"
                 << "----------------------------------------\n";
            temp.pop();
        }
    }

private:
    void processBorrow(const Request& req) {
        for (auto& book : books) {
            if (book.title == req.book_title) {
                if (book.available) {
                    book.available = false;
                    borrow_record record;
                    record.no_matric = req.user_matric;
                    record.borrowed_book = book;
                    record.borrow_time = time(nullptr);
                    record.is_returned = false;
                    records.push_back(record);
                    cout << "Book '" << req.book_title << "' borrowed successfully.\n";
                    saveBook();
                    return;
                } else {
                    cout << "Book is not available.\n";
                    return;
                }
            }
        }
        cout << "Book not found.\n";
    }

    void processReturn(const Request& req) {
        for (auto& record : records) {
            if (record.no_matric == req.user_matric && 
                record.borrowed_book.title == req.book_title && 
                !record.is_returned) {
                record.is_returned = true;
                record.return_time = time(nullptr);
                
                for (auto& book : books) {
                    if (book.title == req.book_title) {
                        book.available = true;
                        break;
                    }
                }
                
                cout << "Book '" << req.book_title << "' returned successfully.\n";
                saveBook();
                return;
            }
        }
        cout << "No matching borrow record found.\n";
    }

    void loadMembers() {
        ifstream file("members.txt");
        if (!file) return;

        MemberNode* last = nullptr;
        int maxId = 1000;  // Starting ID for members

        string line;
        while (getline(file, line)) {
            MemberNode* newMember = new MemberNode();
            size_t pos = 0;
            
            // Parse ID
            pos = line.find("|");
            newMember->id = stoi(line.substr(0, pos));
            line.erase(0, pos + 1);
            
            // Parse name
            pos = line.find("|");
            newMember->name = line.substr(0, pos);
            line.erase(0, pos + 1);
            
            // Parse contact
            pos = line.find("|");
            newMember->contact = line.substr(0, pos);
            line.erase(0, pos + 1);
            
            // Parse active status
            newMember->active = (line == "1");

            // Update maxId if necessary
            if (newMember->id > maxId) {
                maxId = newMember->id;
            }

            // Add to linked list
            if (!memberHead) {
                memberHead = newMember;
            } else {
                last->next = newMember;
            }
            last = newMember;
        }
        
        nextMemberId = maxId + 1;  // Set next ID
        file.close();
        cout << "Members loaded successfully!\n";
    }

    void saveMembers() {
        ofstream file("members.txt");
        if (file.is_open()) {
            MemberNode* current = memberHead;
            while (current != nullptr) {
                file << current->id << "|"
                     << current->name << "|"
                     << current->contact << "|"
                     << current->active << "\n";
                current = current->next;
            }
            file.close();
            cout << "Members saved successfully!\n";
        } else {
            cout << "Error: Unable to save members.\n";
        }
    }

    // Helper method to find a member
    MemberNode* findMember(int id) {
        MemberNode* current = memberHead;
        while (current != nullptr) {
            if (current->id == id) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }

    // Add this to deleteAllLists()
    void deleteAllLists() {
        // Clean up books
        while (bookHead != nullptr) {
            BookNode* temp = bookHead;
            bookHead = bookHead->next;
            delete temp;
        }

        // Clean up members
        while (memberHead != nullptr) {
            MemberNode* temp = memberHead;
            memberHead = memberHead->next;
            delete temp;
        }

        // Clean up borrows (if implemented)
        while (borrowHead != nullptr) {
            BorrowNode* temp = borrowHead;
            borrowHead = borrowHead->next;
            delete temp;
        }
    }
};

int main() {
    Library lib;
    int choice;
    string matric, title;
    
    do {
        cout << "\nBudiman Library Management System\n";
        cout << "----------------------------------------\n";
        cout << "1. Register New Book\n";
        cout << "2. Display Books\n";
        cout << "3. Request to Borrow Book\n";
        cout << "4. Request to Return Book\n";
        cout << "5. Process Next Request\n";
        cout << "6. Display Request Queue\n";
        cout << "7. Exit\n";
        cout << "----------------------------------------\n";
        cout << "Enter choice (1-7): ";
        
        while (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number (1-7): ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
        switch(choice) {
            case 1:
                lib.addBook();
                break;
            case 2:
                lib.displayBooks();
                break;
            case 3: {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Enter user matric: ";
                getline(cin, matric);
                cout << "Enter book title: ";
                getline(cin, title);
                lib.requestBorrow(matric, title);
                break;
            }
            case 4: {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Enter user matric: ";
                getline(cin, matric);
                cout << "Enter book title: ";
                getline(cin, title);
                lib.requestReturn(matric, title);
                break;
            }
            case 5:
                lib.processNextRequest();
                break;
            case 6:
                lib.displayQueue();
                break;
            case 7:
                cout << "Thank you for using Budiman Library Management System!\n";
                break;
            default:
                cout << "Invalid choice! Please enter a number between 1 and 7.\n";
        }
    } while (choice != 7);
    
    return 0;
}
