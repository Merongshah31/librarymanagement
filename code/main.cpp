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
    // Struct definitions
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
    //linked list
    struct MemberNode {
        int id;
        string name;
        string matric;
        bool active;
        MemberNode* next;
        MemberNode() : next(nullptr), active(true) {}
    };

    // Class member variables
    vector<book> books;
    vector<user> users;
    vector<borrow_record> records;
    queue<Request> requestQueue;
    MemberNode* memberHead;
    int nextMemberId;

public:
    // Constructor and destructor
    Library() {
        memberHead = nullptr;
        nextMemberId = 1000;
        loadBooks();
        loadBorrowRecords(); // Load borrow records after books
        loadMembers();
    }

    ~Library() {
        // Clean up memory
        MemberNode* current = memberHead;
        while (current != nullptr) {
            MemberNode* temp = current;
            current = current->next;
            delete temp;
        }
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
                     << (book.available ? "1" : "0") << "\n";
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
            books.clear();
            string line;
            while (getline(file, line)) {
                book newbook;
                size_t pos = 0;
                
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
                
                // Parse availability (1 for available, 0 for borrowed)
                newbook.available = (line == "1");
                
                books.push_back(newbook);
            }
            file.close();
            cout << "Books loaded successfully!\n";
        } else {
            cout << "Error: Unable to load books.\n";
        }
    }

    // Method to display books
    void displayBooks() {
        if (books.empty()) {
            cout << "No books in library.\n";
            return;
        }

        // First, update book status based on borrow records
        for (auto& book : books) {
            bool isBorrowed = false;
            for (const auto& record : records) {
                if (record.borrowed_book.title == book.title && !record.is_returned) {
                    isBorrowed = true;
                    break;
                }
            }
            book.available = !isBorrowed;
        }
        
        // Display the books
        cout << "\nLibrary Books:\n";
        cout << "----------------------------------------\n";
        
        for (size_t i = 0; i < books.size(); i++) {
            cout << "Book " << (i + 1) << ":\n";
            cout << "Title: " << books[i].title << "\n";
            cout << "Author: " << books[i].author << "\n";
            cout << "Year: " << books[i].year << "\n";
            cout << "Price: RM" << fixed << setprecision(2) << books[i].price << "\n";
            cout << "Status: " << (books[i].available ? "Available" : "Borrowed") << "\n";
            cout << "----------------------------------------\n";
        }
    }

    // Method to update book information
    void updateBookInformation() {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string title;
        cout << "Enter the title of the book to update: ";
        getline(cin, title);

        for (auto& book : books) {
            if (book.title == title) {
                cout << "Updating information for: " << book.title << "\n";
                
                cout << "Enter new author (leave blank to keep current): ";
                string newAuthor;
                getline(cin, newAuthor);
                if (!newAuthor.empty()) {
                    book.author = newAuthor;
                }

                cout << "Enter new year (0 to keep current): ";
                int newYear;
                cin >> newYear;
                if (newYear != 0) {
                    book.year = newYear;
                }

                cout << "Enter new price (-1 to keep current): ";
                double newPrice;
                cin >> newPrice;
                if (newPrice != -1) {
                    book.price = newPrice;
                }

                cout << "Is the book available? (1 for Yes, 0 for No): ";
                int availability;
                cin >> availability;
                book.available = (availability == 1);

                saveBook(); // Save updated book list to file
                cout << "Book information updated successfully!\n";
                return;
            }
        }
        cout << "Book with title '" << title << "' not found.\n";
    }

    // Method to delete a book record
    void deleteBookRecord() {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string title;
        cout << "Enter the title of the book to delete: ";
        getline(cin, title);

        auto it = remove_if(books.begin(), books.end(), [&](const book& b) {
            return b.title == title;
        });

        if (it != books.end()) {
            books.erase(it, books.end());
            saveBook(); // Save updated book list to file
            cout << "Book '" << title << "' deleted successfully!\n";
        } else {
            cout << "Book with title '" << title << "' not found.\n";
        }
    }

    // Method to process borrowing a book
    void processBorrow(const Request& req) {
        // First verify the user exists and is active
        bool userFound = false;
        MemberNode* current = memberHead;
        while (current != nullptr) {
            if (current->matric == req.user_matric && current->active) {
                userFound = true;
                break;
            }
            current = current->next;
        }
        
        if (!userFound) {
            cout << "Error: Active user with matric number '" << req.user_matric << "' not found.\n";
            return;
        }

        // Find the book
        //linear search algorithm in book vector
        auto bookIt = find_if(books.begin(), books.end(), 
            [&](const book& b) { return b.title == req.book_title; });

        if (bookIt == books.end()) {
            cout << "Error: Book '" << req.book_title << "' not found.\n";
            return;
        }

        if (!bookIt->available) {
            cout << "Error: Book '" << req.book_title << "' is currently borrowed.\n";
            return;
        }

        // Check for existing unreturned books by this user
        for (const auto& record : records) {
            if (record.no_matric == req.user_matric && !record.is_returned) {
                cout << "Error: User has unreturned books. Please return them first.\n";
                return;
            }
        }

        // Process the borrow
        bookIt->available = false;  // Mark book as unavailable
        
        // Create and save the borrow record
        borrow_record record;
        record.no_matric = req.user_matric;
        record.borrowed_book = *bookIt;
        record.borrow_time = time(nullptr);
        record.is_returned = false;
        record.return_time = 0;
        records.push_back(record);
        
        // Save changes to files
        recordBorrowing(record);
        saveBook();  // Make sure changes are saved to the books file
        
        cout << "Book '" << req.book_title << "' borrowed successfully by user '" << req.user_matric << "'.\n";
    }

    // Method to record borrowing details in borrowing.txt
    void recordBorrowing(const borrow_record& record) {
        ofstream file("borrowing.txt", ios::app); // Open in append mode
        if (file.is_open()) {
            file << record.no_matric << "|"
                 << record.borrowed_book.title << "|"
                 << record.borrowed_book.author << "|"
                 << record.borrowed_book.year << "|"
                 << record.borrowed_book.price << "|"
                 << record.borrow_time << "|"
                 << "0" << "\n"; // 0 indicates not returned
            file.close();
            
            // Update the book's availability status
            for (auto& book : books) {
                if (book.title == record.borrowed_book.title) {
                    book.available = false;
                    break;
                }
            }
            saveBook(); // Save the updated book status
            cout << "Borrowing record saved successfully!\n";
        } else {
            cout << "Error: Unable to save borrowing record.\n";
        }
    }

    // Method to process returning a book
    void processReturn(const Request& req) {
        // Find the borrow record first
        bool recordFound = false;
        for (auto& record : records) {
            if (record.no_matric == req.user_matric && 
                record.borrowed_book.title == req.book_title && 
                !record.is_returned) {
                
                recordFound = true;
                // Update the record
                record.is_returned = true;
                record.return_time = time(nullptr);
                
                // Update the book's availability
                for (auto& book : books) {
                    if (book.title == req.book_title) {
                        book.available = true;
                        break;
                    }
                }
                
                // Record the return
                recordReturn(record);
                saveBook(); // Save the updated book status
                
                cout << "Book '" << req.book_title << "' returned successfully.\n";
                return;
            }
        }
        
        if (!recordFound) {
            cout << "Error: No active borrow record found for book '" 
                 << req.book_title << "' by user '" << req.user_matric << "'.\n";
        }
    }

    // Method to record returns
    void recordReturn(const borrow_record& record) {
        // Update returns.txt
        ofstream returnFile("returns.txt", ios::app);
        if (returnFile.is_open()) {
            returnFile << record.no_matric << "|"
                      << record.borrowed_book.title << "|"
                      << record.borrowed_book.author << "|"
                      << record.return_time << "\n";
            returnFile.close();
        }
        
        // Update borrowing.txt with return status
        vector<borrow_record> allRecords;
        ifstream borrowFile("borrowing.txt");
        if (borrowFile.is_open()) {
            string line;
            while (getline(borrowFile, line)) {
                size_t pos = 0;
                borrow_record br;
                
                // Parse matric
                pos = line.find("|");
                br.no_matric = line.substr(0, pos);
                line.erase(0, pos + 1);
                
                // Parse book title
                pos = line.find("|");
                br.borrowed_book.title = line.substr(0, pos);
                line.erase(0, pos + 1);
                
                // Parse remaining fields
                pos = line.find("|");
                br.borrowed_book.author = line.substr(0, pos);
                line.erase(0, pos + 1);
                
                pos = line.find("|");
                br.borrowed_book.year = stoi(line.substr(0, pos));
                line.erase(0, pos + 1);
                
                pos = line.find("|");
                br.borrowed_book.price = stod(line.substr(0, pos));
                line.erase(0, pos + 1);
                
                pos = line.find("|");
                br.borrow_time = stoll(line.substr(0, pos));
                line.erase(0, pos + 1);
                
                br.is_returned = (line == "1");
                
                // Update status if this is the matching record
                if (br.no_matric == record.no_matric && 
                    br.borrowed_book.title == record.borrowed_book.title && 
                    br.borrow_time == record.borrow_time) {
                    br.is_returned = true;
                }
                
                allRecords.push_back(br);
            }
            borrowFile.close();
        }
        
        // Save updated records back to borrowing.txt
        ofstream updateFile("borrowing.txt");
        if (updateFile.is_open()) {
            for (const auto& br : allRecords) {
                updateFile << br.no_matric << "|"
                          << br.borrowed_book.title << "|"
                          << br.borrowed_book.author << "|"
                          << br.borrowed_book.year << "|"
                          << br.borrowed_book.price << "|"
                          << br.borrow_time << "|"
                          << (br.is_returned ? "1" : "0") << "\n";
            }
            updateFile.close();
        }
        
        cout << "Return record saved successfully!\n";
    }

    // Method to request borrowing a book
    void requestBorrow(const string& matric, const string& title) {
        if (matric.empty() || title.empty()) {
            cout << "Error: Matric number and title cannot be empty.\n";
            return;
        }

        // Verify user exists
        bool userExists = false;
        MemberNode* current = memberHead;
        while (current != nullptr) {
            if (current->matric == matric && current->active) {
                userExists = true;
                break;
            }
            current = current->next;
        }

        if (!userExists) {
            cout << "Error: Active user with matric number '" << matric << "' not found.\n";
            return;
        }

        // Verify book exists
        bool bookExists = false;
        for (const auto& book : books) {
            if (book.title == title) {
                bookExists = true;
                break;
            }
        }

        if (!bookExists) {
            cout << "Error: Book '" << title << "' not found.\n";
            return;
        }

        // Check for existing requests
        queue<Request> tempQueue = requestQueue;
        while (!tempQueue.empty()) {
            Request req = tempQueue.front();
            if (req.user_matric == matric && !req.is_return) {
                cout << "Error: User already has a pending borrow request.\n";
                return;
            }
            tempQueue.pop();
        }

        Request req;
        req.user_matric = matric;
        req.book_title = title;
        req.request_time = time(nullptr);
        req.is_return = false;
        
        requestQueue.push(req);
        cout << "Borrow request for '" << title << "' added to queue.\n";
    }

    // Method to request returning a book
    void requestReturn(const string& matric, const string& title) {
        if (matric.empty() || title.empty()) {
            cout << "Error: Matric number and title cannot be empty.\n";
            return;
        }

        bool validBorrow = false;
        // Check if the book is actually borrowed by this user
        for (const auto& record : records) {
            if (record.no_matric == matric && 
                record.borrowed_book.title == title && 
                !record.is_returned) {
                validBorrow = true;
                break;
            }
        }

        if (!validBorrow) {
            cout << "Error: You have not borrowed this book or it's already returned.\n"; //error handling
            return;
        }

        Request req;
        req.user_matric = matric;
        req.book_title = title;
        req.request_time = time(nullptr);
        req.is_return = true;
        
        requestQueue.push(req);
        cout << "Return request added to queue successfully.\n";
    }
    //FIFO
    // Method to process the next request in the queue
    void processNextRequest() {
        if (requestQueue.empty()) {
            cout << "No requests in queue.\n";
            return;
        }

        Request req = requestQueue.front();
        requestQueue.pop();

        cout << "\nProcessing " << (req.is_return ? "return" : "borrow") 
             << " request for book '" << req.book_title 
             << "' from user '" << req.user_matric << "'.\n";

        if (req.is_return) {
            processReturn(req);
        } else {
            processBorrow(req);
        }
    }

    // Method to register a new member
    void registerNewMember() {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        MemberNode* newMember = new MemberNode();
        newMember->id = nextMemberId++;
        
        cout << "Enter member name: ";
        getline(cin, newMember->name);
        
        cout << "Enter matric number: ";
        getline(cin, newMember->matric);
        
        // Add to linked list
        if (!memberHead) {
            memberHead = newMember;
        } else {
            MemberNode* current = memberHead;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newMember;
        }
        
        saveMembers();
        cout << "Member registered successfully with ID: " << newMember->id << "\n";
    }

    // Method to display members
    void displayMembers() {
        if (!memberHead) {
            cout << "No members registered.\n";
            return;
        }
        
        cout << "\nRegistered Members:\n";
        cout << "----------------------------------------\n";
        MemberNode* current = memberHead;
        while (current != nullptr) {
            cout << "ID: " << current->id << "\n"
                 << "Name: " << current->name << "\n"
                 << "Matric: " << current->matric << "\n"
                 << "Status: " << (current->active ? "Active" : "Inactive") << "\n"
                 << "----------------------------------------\n";
            current = current->next;
        }
    }

    // Method to update member information
    void updateMemberInformation() {
        displayMembers(); // Show current members first
        
        string searchMatric;
        cout << "\nEnter matric number of member to update: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, searchMatric);

        MemberNode* current = memberHead;
        bool found = false;

        while (current != nullptr) {
            if (current->matric == searchMatric) {
                found = true;
                cout << "\nCurrent Member Information:\n";
                cout << "----------------------------------------\n";
                cout << "ID: " << current->id << "\n";
                cout << "Name: " << current->name << "\n";
                cout << "Matric: " << current->matric << "\n";
                cout << "Status: " << (current->active ? "Active" : "Inactive") << "\n";
                cout << "----------------------------------------\n";

                cout << "\nUpdate Information:\n";
                cout << "1. Update Name\n";
            }
            current = current->next;
        }

        if (!found) {
            cout << "Member with matric number '" << searchMatric << "' not found.\n";
        }
    }

    // Method to update member record
    void updateMemberRecord() {
        int memberId;
        cout << "Enter the ID of the member to update: ";
        cin >> memberId;

        MemberNode* current = memberHead;
        while (current != nullptr) {
            if (current->id == memberId) {
                cout << "Updating information for member ID: " << memberId << "\n";
                
                cout << "Enter new name (leave blank to keep current): ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                string newName;
                getline(cin, newName);
                if (!newName.empty()) {
                    current->name = newName;
                }

                cout << "Enter new matric number (leave blank to keep current): ";
                string newMatric;
                getline(cin, newMatric);
                if (!newMatric.empty()) {
                    current->matric = newMatric;
                }

                cout << "Is the member active? (1 for Yes, 0 for No): ";
                int activeStatus;
                cin >> activeStatus;
                current->active = (activeStatus == 1);

                saveMembers();
                cout << "Member information updated successfully!\n";
                return;
            }
            current = current->next;
        }
        cout << "Member with ID '" << memberId << "' not found.\n";
    }

    // Method to delete a member record
    void deleteMemberRecord() {
        int memberId;
        cout << "Enter the ID of the member to delete: ";
        cin >> memberId;

        MemberNode* current = memberHead;
        MemberNode* previous = nullptr;

        while (current != nullptr) {
            if (current->id == memberId) {
                if (previous == nullptr) {
                    // Deleting the head
                    memberHead = current->next;
                } else {
                    previous->next = current->next;
                }
                delete current;
                saveMembers(); // Save updated member list to file
                cout << "Member with ID '" << memberId << "' deleted successfully!\n";
                return;
            }
            previous = current;
            current = current->next;
        }
        cout << "Member with ID '" << memberId << "' not found.\n";
    }

    // Method to load members from file
    void loadMembers() {
        ifstream file("members.txt");
        if (file.is_open()) {
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
                
                // Parse matric
                pos = line.find("|");
                newMember->matric = line.substr(0, pos);
                line.erase(0, pos + 1);
                
                // Parse active status
                newMember->active = (line == "1");

                if (!memberHead) {
                    memberHead = newMember;
                } else {
                    MemberNode* current = memberHead;
                    while (current->next != nullptr) {
                        current = current->next;
                    }
                    current->next = newMember;
                }
            }
            file.close();
            cout << "Members loaded successfully!\n";
        } else {
            cout << "Error: Unable to load members.\n";
        }
    }

    // Method to save members to file
    void saveMembers() {
        ofstream file("members.txt");
        if (file.is_open()) {
            MemberNode* current = memberHead;
            while (current != nullptr) {
                file << current->id << "|"
                     << current->name << "|"
                     << current->matric << "|"
                     << (current->active ? "1" : "0") << "\n";
                current = current->next;
            }
            file.close();
            cout << "Members saved successfully!\n";
        } else {
            cout << "Error: Unable to save members.\n"; //error handling
        }
    }

    // Method to load borrow records
    void loadBorrowRecords() {
        records.clear(); // Clear existing records
        ifstream file("borrowing.txt");
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                borrow_record record;
                size_t pos = 0;
                
                // Parse matric
                pos = line.find("|");
                record.no_matric = line.substr(0, pos);
                line.erase(0, pos + 1);
                
                // Parse book details
                book borrowed_book;
                
                pos = line.find("|");
                borrowed_book.title = line.substr(0, pos);
                line.erase(0, pos + 1);
                
                pos = line.find("|");
                borrowed_book.author = line.substr(0, pos);
                line.erase(0, pos + 1);
                
                pos = line.find("|");
                borrowed_book.year = stoi(line.substr(0, pos));
                line.erase(0, pos + 1);
                
                pos = line.find("|");
                borrowed_book.price = stod(line.substr(0, pos));
                line.erase(0, pos + 1);
                
                pos = line.find("|");
                record.borrow_time = stoll(line.substr(0, pos));
                line.erase(0, pos + 1);
                
                record.is_returned = (line == "1");
                record.borrowed_book = borrowed_book;
                
                records.push_back(record);
            }
            file.close();
            
            // Update book availability based on records
            for (auto& book : books) {
                bool isBorrowed = false;
                for (const auto& record : records) {
                    if (record.borrowed_book.title == book.title && !record.is_returned) {
                        isBorrowed = true;
                        break;
                    }
                }
                book.available = !isBorrowed;
            }
            saveBook(); // Save the updated book statuses
        }
    }

    // Method to view borrowing history
    void viewBorrowingHistory() {
        if (records.empty()) {
            cout << "No borrowing history available.\n";
            return;
        }

        string searchMatric;
        cout << "Enter matric number to view history (or press Enter to view all): ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, searchMatric);

        cout << "\nBorrowing History:\n";
        cout << "----------------------------------------\n";

        bool found = false;
        for (const auto& record : records) {
            // If searchMatric is empty, show all records
            // Otherwise, show only records matching the matric number
            if (searchMatric.empty() || record.no_matric == searchMatric) {
                found = true;
                cout << "Matric Number: " << record.no_matric << "\n";
                cout << "Book Title: " << record.borrowed_book.title << "\n";
                cout << "Author: " << record.borrowed_book.author << "\n";
                cout << "Borrow Date: " << formatTime(record.borrow_time) << "\n";
                
                if (record.is_returned) {
                    cout << "Return Date: " << formatTime(record.return_time) << "\n";
                    cout << "Status: Returned\n";
                } else {
                    cout << "Status: Currently Borrowed\n";
                }
                cout << "----------------------------------------\n";
            }
        }

        if (!found && !searchMatric.empty()) {
            cout << "No borrowing history found for matric number: " << searchMatric << "\n";
        }
    }

    // Helper method to format time
    string formatTime(time_t timestamp) {
        char buffer[26];
        struct tm* timeinfo = localtime(&timestamp);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        return string(buffer);
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
        cout << "3. Update Book Information\n";  // New option
        cout << "4. Delete Book Record\n";        // New option
        cout << "5. Update Member Record\n";      // New option
        cout << "6. Delete Member Record\n";      // New option
        cout << "7. Borrow Book\n";
        cout << "8. Return Book\n";
        cout << "9. Process Next Request\n";
        cout << "10. Register New Member\n";
        cout << "11. Display Members\n";
        cout << "12. View Borrowing History\n";  // New option
        cout << "13. Exit\n";
        cout << "----------------------------------------\n";
        cout << "Enter choice (1-13): ";
        
        while (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number (1-13): ";
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
            case 3:
                lib.updateBookInformation();  // Call to update book information
                break;
            case 4:
                lib.deleteBookRecord();        // Call to delete book record
                break;
            case 5:
                lib.updateMemberRecord();       // Call to update member record
                break;
            case 6:
                lib.deleteMemberRecord();       // Call to delete member record
                break;
            case 7: {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Enter user matric: ";
                getline(cin, matric);
                cout << "Enter book title: ";
                getline(cin, title);
                lib.requestBorrow(matric, title);
                break;
            }
            case 8: {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Enter user matric: ";
                getline(cin, matric);
                cout << "Enter book title: ";
                getline(cin, title);
                lib.requestReturn(matric, title);
                break;
            }
            case 9:
                lib.processNextRequest();
                break;
            case 10:
                lib.registerNewMember();
                break;
            case 11:
                lib.displayMembers();
                break;
            case 12:
                lib.viewBorrowingHistory();
                break;
            case 13:
                cout << "Thank you for using Budiman Library Management System!\n";
                break;
            default:
                cout << "Invalid choice! Please enter a number between 1 and 13.\n";
        }
    } while (choice != 13);
    
    return 0;
}