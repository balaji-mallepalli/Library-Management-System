#include <iostream>
#include <string>
#include <vector>
#include <ctime>

using namespace std;

#define MAX_BOOKS 100
#define MAX_STUDENTS 50

// Date class for managing date information
class Date {
public:
    int day, month, year;

    Date() {
        time_t t = time(0);
        tm* now = localtime(&t);
        day = now->tm_mday;
        month = now->tm_mon + 1;
        year = now->tm_year + 1900;
    }

    Date(int d, int m, int y) : day(d), month(m), year(y) {}

    Date addDays(int daysToAdd) const {
        tm t = {0, 0, 0, day + daysToAdd, month - 1, year - 1900};
        mktime(&t);
        return Date(t.tm_mday, t.tm_mon + 1, t.tm_year + 1900);
    }

    bool isPastDue(const Date& dueDate) const {
        if (year > dueDate.year) return true;
        if (year == dueDate.year && month > dueDate.month) return true;
        if (year == dueDate.year && month == dueDate.month && day > dueDate.day) return true;
        return false;
    }

    void displayDate() const {
        cout << day << "/" << month << "/" << year;
    }
};

class Book {
private:
    int bookId;
    string title;
    string author;
    bool isIssued;
    int issuedToStudentId;
    Date issueDate;
    Date dueDate;

public:
    Book(int id, const string& t, const string& a)
        : bookId(id), title(t), author(a), isIssued(false), issuedToStudentId(-1) {}

    int getBookId() const { return bookId; }
    bool getIsIssued() const { return isIssued; }
    int getIssuedToStudentId() const { return issuedToStudentId; }
    Date getDueDate() const { return dueDate; }

    void issue(int studentId) {
        isIssued = true;
        issuedToStudentId = studentId;
        issueDate = Date();
        dueDate = issueDate.addDays(14);
    }

    void returnBook() {
        isIssued = false;
        issuedToStudentId = -1;
        cout << "Book returned successfully.\n";
    }

    void displayInfo() const {
        cout << "Book ID: " << bookId << ", Title: " << title << ", Author: " << author;
        if (isIssued) {
            cout << ", Issued to Student ID: " << issuedToStudentId << ", Issue Date: ";
            issueDate.displayDate();
            cout << ", Due Date: ";
            dueDate.displayDate();
        } else {
            cout << ", Not issued";
        }
        cout << endl;
    }
};

class Student {
private:
    int studentId;
    string name;
    int booksIssued;

public:
    Student(int id, const string& n) : studentId(id), name(n), booksIssued(0) {}

    int getStudentId() const { return studentId; }
    string getName() const { return name; }
    int getBooksIssued() const { return booksIssued; }

    void incrementBooksIssued() { booksIssued++; }
    void decrementBooksIssued() { booksIssued--; }
    bool canIssueMoreBooks() const { return booksIssued < 3; }

    void displayInfo() const {
        cout << "Student ID: " << studentId << ", Name: " << name << ", Books Issued: " << booksIssued << endl;
    }
};

class Library {
private:
    vector<Book> books;
    vector<Student> students;

    Book* findBook(int bookId) {
        for (auto& book : books) {
            if (book.getBookId() == bookId) return &book;
        }
        return nullptr;
    }

    Student* findStudent(int studentId) {
        for (auto& student : students) {
            if (student.getStudentId() == studentId) return &student;
        }
        return nullptr;
    }

public:
    void addBook(int bookId, const string& title, const string& author) {
        if (books.size() < MAX_BOOKS) {
            books.emplace_back(bookId, title, author);
            cout << "Book added successfully.\n";
        } else {
            cout << "Maximum number of books reached.\n";
        }
    }

    void addStudent(int studentId, const string& name) {
        if (students.size() < MAX_STUDENTS) {
            students.emplace_back(studentId, name);
            cout << "Student added successfully.\n";
        } else {
            cout << "Maximum number of students reached.\n";
        }
    }

    void issueBook(int studentId, int bookId) {
        Book* book = findBook(bookId);
        Student* student = findStudent(studentId);

        if (!book || !student) {
            cout << "Invalid Book ID or Student ID.\n";
            return;
        }

        if (book->getIsIssued()) {
            cout << "Book is already issued to another student.\n";
        } else if (!student->canIssueMoreBooks()) {
            cout << "Student has already issued the maximum number of books.\n";
        } else {
            book->issue(studentId);
            student->incrementBooksIssued();
            cout << "Book issued successfully to Student ID " << studentId << ".\n";
        }
    }

    void returnBook(int bookId) {
        Book* book = findBook(bookId);
        if (!book || !book->getIsIssued()) {
            cout << "Invalid Book ID or the book is not currently issued.\n";
            return;
        }

        Student* student = findStudent(book->getIssuedToStudentId());
        if (student) {
            student->decrementBooksIssued();
        }
        book->returnBook();
    }

    void checkOverdueBooks() {
        cout << "Overdue Books:\n";
        Date currentDate;
        bool foundOverdue = false;
        for (const auto& book : books) {
            if (book.getIsIssued() && currentDate.isPastDue(book.getDueDate())) {
                book.displayInfo();
                foundOverdue = true;
            }
        }
        if (!foundOverdue) cout << "No overdue books.\n";
    }

    void checkBookStatus(int bookId) {
        Book* book = findBook(bookId);
        if (book) {
            book->displayInfo();
        } else {
            cout << "Invalid Book ID.\n";
        }
    }

    void displayAvailableBooks() {
        cout << "Available Books:\n";
        bool foundAvailable = false;
        for (const auto& book : books) {
            if (!book.getIsIssued()) {
                book.displayInfo();
                foundAvailable = true;
            }
        }
        if (!foundAvailable) cout << "No available books at the moment.\n";
    }

    void displayAllStudents() {
        cout << "Registered Students:\n";
        for (const auto& student : students) {
            student.displayInfo();
        }
    }
};

int main() {
    Library library;
    int choice, bookId, studentId;
    string title, author, name;

    do {
        cout << "\nLibrary Management System\n";
        cout << "1. Add Book\n";
        cout << "2. Add Student\n";
        cout << "3. Issue Book\n";
        cout << "4. Return Book\n";
        cout << "5. List Overdue Books\n";
        cout << "6. Check Book Status\n";
        cout << "7. Display Available Books\n";
        cout << "8. Display All Students\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Enter Book ID: ";
                cin >> bookId;
                cin.ignore();
                cout << "Enter Title: ";
                getline(cin, title);
                cout << "Enter Author: ";
                getline(cin, author);
                library.addBook(bookId, title, author);
                break;
            case 2:
                cout << "Enter Student ID: ";
                cin >> studentId;
                cin.ignore();
                cout << "Enter Student Name: ";
                getline(cin, name);
                library.addStudent(studentId, name);
                break;
            case 3:
                cout << "Enter Student ID: ";
                cin >> studentId;
                cout << "Enter Book ID: ";
                cin >> bookId;
                library.issueBook(studentId, bookId);
                break;
            case 4:
                cout << "Enter Book ID to return: ";
                cin >> bookId;
                library.returnBook(bookId);
                break;
            case 5:
                library.checkOverdueBooks();
                break;
            case 6:
                cout << "Enter Book ID: ";
                cin >> bookId;
                library.checkBookStatus(bookId);
                break;
            case 7:
                library.displayAvailableBooks();
                break;
            case 8:
                library.displayAllStudents();
                break;
            case 9:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 9);

    return 0;
}
