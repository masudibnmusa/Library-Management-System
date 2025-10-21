#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_STR 100
#define FILENAME "library.dat"
#define MAX_BORROWER_NAME 50

// Book structure
typedef struct Book {
    int id;
    char title[MAX_STR];
    char author[MAX_STR];
    char isbn[20];
    int year;
    int is_issued;
    char issued_to[MAX_BORROWER_NAME];
    time_t issue_date;
    time_t due_date;
    struct Book* next;
} Book;

// Statistics structure
typedef struct {
    int total_books;
    int issued_books;
    int available_books;
} LibraryStats;

// Global variables
Book* head = NULL;
int book_count = 0;
int next_id = 1;

// Function prototypes
void displayMenu();
void addBook();
void removeBook();
void issueBook();
void returnBook();
void displayBooks();
void searchBooks();
void viewBookDetails();
void libraryStatistics();
void saveToFile();
void loadFromFile();
void exportToText();
Book* createBook(int id, char* title, char* author, char* isbn, int year);
void insertBook(Book* newBook);
Book* searchBook(int id);
Book* searchBookByTitle(char* title);
Book* searchBookByISBN(char* isbn);
void freeList();
void clearInputBuffer();
int getIntegerInput(const char* prompt);
void safe_strcpy(char* dest, const char* src, size_t dest_size);
void printBookDetails(Book* book);
int validateISBN(char* isbn);
void sortBooksByTitle();
void sortBooksByAuthor();
Book* mergeSort(Book* head, int (*compare)(Book*, Book*));
int compareByTitle(Book* a, Book* b);
int compareByAuthor(Book* a, Book* b);

int main() {
    loadFromFile();

    int choice;

    printf("=== Enhanced Library Management System ===\n");
    printf("Initialized with %d books\n", book_count);

    do {
        displayMenu();
        choice = getIntegerInput("Enter your choice: ");

        switch(choice) {
            case 1:
                addBook();
                break;
            case 2:
                removeBook();
                break;
            case 3:
                issueBook();
                break;
            case 4:
                returnBook();
                break;
            case 5:
                displayBooks();
                break;
            case 6:
                searchBooks();
                break;
            case 7:
                viewBookDetails();
                break;
            case 8:
                sortBooksByTitle();
                break;
            case 9:
                sortBooksByAuthor();
                break;
            case 10:
                libraryStatistics();
                break;
            case 11:
                saveToFile();
                printf("Data saved successfully!\n");
                break;
            case 12:
                exportToText();
                break;
            case 13:
                printf("Exiting... Thank you for using the Library Management System!\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
        }
        printf("\n");
    } while(choice != 13);

    freeList();
    return 0;
}

void displayMenu() {
    printf("\n=== Main Menu ===\n");
    printf("1. Add Book\n");
    printf("2. Remove Book\n");
    printf("3. Issue Book\n");
    printf("4. Return Book\n");
    printf("5. Display All Books\n");
    printf("6. Search Books\n");
    printf("7. View Book Details\n");
    printf("8. Sort Books by Title\n");
    printf("9. Sort Books by Author\n");
    printf("10. Library Statistics\n");
    printf("11. Save Data to File\n");
    printf("12. Export to Text File\n");
    printf("13. Exit\n");
    printf("==================\n");
}

void addBook() {
    char title[MAX_STR], author[MAX_STR], isbn[20];
    int year;

    printf("\n=== Add New Book ===\n");

    // Get book details
    printf("Enter book title: ");
    fgets(title, MAX_STR, stdin);
    title[strcspn(title, "\n")] = 0;

    printf("Enter author name: ");
    fgets(author, MAX_STR, stdin);
    author[strcspn(author, "\n")] = 0;

    printf("Enter ISBN: ");
    fgets(isbn, 20, stdin);
    isbn[strcspn(isbn, "\n")] = 0;

    if (!validateISBN(isbn)) {
        printf("Warning: ISBN format may be invalid. Continuing anyway...\n");
    }

    year = getIntegerInput("Enter publication year: ");

    // Check if book with same title or ISBN already exists
    Book* existing = searchBookByTitle(title);
    if (existing != NULL) {
        printf("Error: A book with title '%s' already exists (ID: %d)!\n", title, existing->id);
        return;
    }

    existing = searchBookByISBN(isbn);
    if (existing != NULL) {
        printf("Error: A book with ISBN '%s' already exists (ID: %d)!\n", isbn, existing->id);
        return;
    }

    Book* newBook = createBook(next_id, title, author, isbn, year);
    if (newBook == NULL) {
        printf("Error: Failed to add book! Memory allocation failed.\n");
        return;
    }

    insertBook(newBook);
    book_count++;
    printf("Book added successfully! Book ID: %d\n", next_id);
    next_id++;
}

void removeBook() {
    int id;

    printf("\n=== Remove Book ===\n");

    if (head == NULL) {
        printf("No books in the library!\n");
        return;
    }

    id = getIntegerInput("Enter book ID to remove: ");

    Book* current = head;
    Book* prev = NULL;

    while (current != NULL) {
        if (current->id == id) {
            if (current->is_issued) {
                printf("Cannot remove book! It's currently issued to: %s\n", current->issued_to);
                return;
            }

            printf("Are you sure you want to remove '%s' by %s? (y/n): ",
                   current->title, current->author);
            char confirm;
            scanf(" %c", &confirm);
            clearInputBuffer();

            if (confirm == 'y' || confirm == 'Y') {
                if (prev == NULL) {
                    head = current->next;
                } else {
                    prev->next = current->next;
                }

                free(current);
                book_count--;
                printf("Book removed successfully!\n");
            } else {
                printf("Removal cancelled.\n");
            }
            return;
        }
        prev = current;
        current = current->next;
    }

    printf("Book with ID %d not found!\n", id);
}

void issueBook() {
    int id, days;
    char issued_to[MAX_BORROWER_NAME];

    printf("\n=== Issue Book ===\n");

    if (head == NULL) {
        printf("No books in the library!\n");
        return;
    }

    id = getIntegerInput("Enter book ID to issue: ");

    Book* book = searchBook(id);
    if (book == NULL) {
        printf("Book with ID %d not found!\n", id);
        return;
    }

    if (book->is_issued) {
        printf("Book is already issued to: %s\n", book->issued_to);
        printf("Issued on: %s", ctime(&book->issue_date));
        printf("Due date: %s", ctime(&book->due_date));
        return;
    }

    printf("Enter borrower's name: ");
    fgets(issued_to, MAX_BORROWER_NAME, stdin);
    issued_to[strcspn(issued_to, "\n")] = 0;

    days = getIntegerInput("Enter number of days for issuance: ");

    book->is_issued = 1;
    safe_strcpy(book->issued_to, issued_to, MAX_BORROWER_NAME);
    book->issue_date = time(NULL);
    book->due_date = book->issue_date + (days * 24 * 60 * 60);

    printf("Book '%s' issued successfully to %s!\n", book->title, issued_to);
    printf("Due date: %s", ctime(&book->due_date));
}

void returnBook() {
    int id;

    printf("\n=== Return Book ===\n");

    if (head == NULL) {
        printf("No books in the library!\n");
        return;
    }

    id = getIntegerInput("Enter book ID to return: ");

    Book* book = searchBook(id);
    if (book == NULL) {
        printf("Book with ID %d not found!\n", id);
        return;
    }

    if (!book->is_issued) {
        printf("Book is not issued to anyone!\n");
        return;
    }

    time_t current_time = time(NULL);
    double days_overdue = difftime(current_time, book->due_date) / (24 * 60 * 60);

    printf("Book '%s' returned by %s!\n", book->title, book->issued_to);
    if (days_overdue > 0) {
        printf("Warning: This book is %.1f days overdue!\n", days_overdue);
    }

    book->is_issued = 0;
    book->issued_to[0] = '\0';
    book->issue_date = 0;
    book->due_date = 0;
}

void displayBooks() {
    Book* current = head;
    int count = 0;

    printf("\n=== All Books in Library ===\n");

    if (head == NULL) {
        printf("No books in the library!\n");
        return;
    }

    printf("%-5s %-30s %-25s %-15s %-6s %-10s %-20s\n",
           "ID", "Title", "Author", "ISBN", "Year", "Status", "Issued To");
    printf("--------------------------------------------------------------------------------------------------------\n");

    while (current != NULL) {
        char status[10];
        char issued_info[20];

        if (current->is_issued) {
            strcpy(status, "Issued");
            safe_strcpy(issued_info, current->issued_to, 20);
        } else {
            strcpy(status, "Available");
            strcpy(issued_info, "-");
        }

        printf("%-5d %-30s %-25s %-15s %-6d %-10s %-20s\n",
               current->id, current->title, current->author,
               current->isbn, current->year, status, issued_info);
        current = current->next;
        count++;
    }
    printf("Total books: %d\n", count);
}

void searchBooks() {
    char query[MAX_STR];
    int found = 0;

    printf("\n=== Search Books ===\n");

    if (head == NULL) {
        printf("No books in the library!\n");
        return;
    }

    printf("Enter title, author, or ISBN to search: ");
    fgets(query, MAX_STR, stdin);
    query[strcspn(query, "\n")] = 0;

    if (strlen(query) == 0) {
        printf("No search query entered!\n");
        return;
    }

    Book* current = head;

    printf("\n=== Search Results ===\n");
    printf("%-5s %-30s %-25s %-15s %-6s %-10s\n",
           "ID", "Title", "Author", "ISBN", "Year", "Status");
    printf("----------------------------------------------------------------------------\n");

    while (current != NULL) {
        if (strstr(current->title, query) ||
            strstr(current->author, query) ||
            strstr(current->isbn, query)) {

            char status[10];
            strcpy(status, current->is_issued ? "Issued" : "Available");

            printf("%-5d %-30s %-25s %-15s %-6d %-10s\n",
                   current->id, current->title, current->author,
                   current->isbn, current->year, status);
            found = 1;
        }
        current = current->next;
    }

    if (!found) {
        printf("No books found matching '%s'\n", query);
    }
}

void viewBookDetails() {
    int id;

    printf("\n=== View Book Details ===\n");

    if (head == NULL) {
        printf("No books in the library!\n");
        return;
    }

    id = getIntegerInput("Enter book ID: ");

    Book* book = searchBook(id);
    if (book == NULL) {
        printf("Book with ID %d not found!\n", id);
        return;
    }

    printBookDetails(book);
}

void libraryStatistics() {
    LibraryStats stats = {0, 0, 0};
    Book* current = head;

    printf("\n=== Library Statistics ===\n");

    if (head == NULL) {
        printf("No books in the library!\n");
        return;
    }

    while (current != NULL) {
        stats.total_books++;
        if (current->is_issued) {
            stats.issued_books++;
        } else {
            stats.available_books++;
        }
        current = current->next;
    }

    printf("Total Books: %d\n", stats.total_books);
    printf("Available Books: %d\n", stats.available_books);
    printf("Issued Books: %d\n", stats.issued_books);
    printf("Availability Rate: %.1f%%\n",
           (float)stats.available_books / stats.total_books * 100);
}

Book* createBook(int id, char* title, char* author, char* isbn, int year) {
    Book* newBook = (Book*)malloc(sizeof(Book));
    if (newBook == NULL) {
        return NULL;
    }

    newBook->id = id;
    safe_strcpy(newBook->title, title, MAX_STR);
    safe_strcpy(newBook->author, author, MAX_STR);
    safe_strcpy(newBook->isbn, isbn, 20);
    newBook->year = year;
    newBook->is_issued = 0;
    newBook->issued_to[0] = '\0';
    newBook->issue_date = 0;
    newBook->due_date = 0;
    newBook->next = NULL;

    return newBook;
}

void insertBook(Book* newBook) {
    if (head == NULL) {
        head = newBook;
    } else {
        Book* current = head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newBook;
    }
}

Book* searchBook(int id) {
    Book* current = head;
    while (current != NULL) {
        if (current->id == id) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

Book* searchBookByTitle(char* title) {
    Book* current = head;
    while (current != NULL) {
        if (strcasecmp(current->title, title) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

Book* searchBookByISBN(char* isbn) {
    Book* current = head;
    while (current != NULL) {
        if (strcmp(current->isbn, isbn) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void saveToFile() {
    FILE* file = fopen(FILENAME, "w");
    if (file == NULL) {
        printf("Error: Cannot open file for writing!\n");
        return;
    }

    Book* current = head;
    while (current != NULL) {
        fprintf(file, "%d|%s|%s|%s|%d|%d|%s|%ld|%ld\n",
                current->id, current->title, current->author, current->isbn,
                current->year, current->is_issued, current->issued_to,
                current->issue_date, current->due_date);
        current = current->next;
    }

    fclose(file);
}

void loadFromFile() {
    FILE* file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("No existing data file found. Starting with empty library.\n");
        return;
    }

    char line[512];
    int max_id = 0;

    while (fgets(line, sizeof(line), file)) {
        Book book;
        char* token;

        token = strtok(line, "|");
        if (token) book.id = atoi(token);

        token = strtok(NULL, "|");
        if (token) safe_strcpy(book.title, token, MAX_STR);

        token = strtok(NULL, "|");
        if (token) safe_strcpy(book.author, token, MAX_STR);

        token = strtok(NULL, "|");
        if (token) safe_strcpy(book.isbn, token, 20);

        token = strtok(NULL, "|");
        if (token) book.year = atoi(token);

        token = strtok(NULL, "|");
        if (token) book.is_issued = atoi(token);

        token = strtok(NULL, "|");
        if (token) safe_strcpy(book.issued_to, token, MAX_BORROWER_NAME);

        token = strtok(NULL, "|");
        if (token) book.issue_date = atol(token);

        token = strtok(NULL, "|");
        if (token) book.due_date = atol(token);

        Book* newBook = createBook(book.id, book.title, book.author, book.isbn, book.year);
        if (newBook != NULL) {
            newBook->is_issued = book.is_issued;
            safe_strcpy(newBook->issued_to, book.issued_to, MAX_BORROWER_NAME);
            newBook->issue_date = book.issue_date;
            newBook->due_date = book.due_date;
            insertBook(newBook);
            book_count++;

            if (book.id >= max_id) {
                max_id = book.id + 1;
            }
        }
    }

    next_id = max_id;
    fclose(file);
}

void exportToText() {
    char filename[MAX_STR];
    printf("Enter filename for export (without extension): ");
    fgets(filename, MAX_STR, stdin);
    filename[strcspn(filename, "\n")] = 0;

    strcat(filename, ".txt");

    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Cannot create file %s!\n", filename);
        return;
    }

    fprintf(file, "=== Library Catalog Export ===\n");
    fprintf(file, "Generated on: %s", ctime(&(time_t){time(NULL)}));
    fprintf(file, "Total books: %d\n\n", book_count);

    Book* current = head;
    while (current != NULL) {
        fprintf(file, "ID: %d\n", current->id);
        fprintf(file, "Title: %s\n", current->title);
        fprintf(file, "Author: %s\n", current->author);
        fprintf(file, "ISBN: %s\n", current->isbn);
        fprintf(file, "Year: %d\n", current->year);
        fprintf(file, "Status: %s\n", current->is_issued ? "Issued" : "Available");
        if (current->is_issued) {
            fprintf(file, "Issued to: %s\n", current->issued_to);
            fprintf(file, "Issue date: %s", ctime(&current->issue_date));
            fprintf(file, "Due date: %s", ctime(&current->due_date));
        }
        fprintf(file, "---------------------------\n");
        current = current->next;
    }

    fclose(file);
    printf("Library catalog exported to %s successfully!\n", filename);
}

void freeList() {
    Book* current = head;
    while (current != NULL) {
        Book* temp = current;
        current = current->next;
        free(temp);
    }
    head = NULL;
    book_count = 0;
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int getIntegerInput(const char* prompt) {
    int value;
    char buffer[100];

    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin)) {
            if (sscanf(buffer, "%d", &value) == 1) {
                return value;
            }
        }
        printf("Invalid input! Please enter a valid number.\n");
    }
}

void safe_strcpy(char* dest, const char* src, size_t dest_size) {
    if (dest_size > 0) {
        strncpy(dest, src, dest_size - 1);
        dest[dest_size - 1] = '\0';
    }
}

void printBookDetails(Book* book) {
    printf("\n=== Book Details ===\n");
    printf("ID: %d\n", book->id);
    printf("Title: %s\n", book->title);
    printf("Author: %s\n", book->author);
    printf("ISBN: %s\n", book->isbn);
    printf("Publication Year: %d\n", book->year);
    printf("Status: %s\n", book->is_issued ? "Issued" : "Available");

    if (book->is_issued) {
        printf("Issued to: %s\n", book->issued_to);
        printf("Issue date: %s", ctime(&book->issue_date));
        printf("Due date: %s", ctime(&book->due_date));

        time_t current_time = time(NULL);
        if (current_time > book->due_date) {
            double days_overdue = difftime(current_time, book->due_date) / (24 * 60 * 60);
            printf("This book is %.1f days overdue!\n", days_overdue);
        }
    }
}

int validateISBN(char* isbn) {
    int len = strlen(isbn);
    // Basic ISBN validation (can be enhanced)
    return (len == 10 || len == 13);
}

void sortBooksByTitle() {
    head = mergeSort(head, compareByTitle);
    printf("Books sorted by title successfully!\n");
}

void sortBooksByAuthor() {
    head = mergeSort(head, compareByAuthor);
    printf("Books sorted by author successfully!\n");
}

Book* mergeSort(Book* head, int (*compare)(Book*, Book*)) {
    if (head == NULL || head->next == NULL) {
        return head;
    }

    // Split the list
    Book* slow = head;
    Book* fast = head->next;

    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }

    Book* mid = slow->next;
    slow->next = NULL;

    // Recursively sort both halves
    Book* left = mergeSort(head, compare);
    Book* right = mergeSort(mid, compare);

    // Merge
    Book dummy;
    Book* tail = &dummy;
    dummy.next = NULL;

    while (left != NULL && right != NULL) {
        if (compare(left, right) <= 0) {
            tail->next = left;
            left = left->next;
        } else {
            tail->next = right;
            right = right->next;
        }
        tail = tail->next;
    }

    tail->next = (left != NULL) ? left : right;
    return dummy.next;
}

int compareByTitle(Book* a, Book* b) {
    return strcasecmp(a->title, b->title);
}

int compareByAuthor(Book* a, Book* b) {
    return strcasecmp(a->author, b->author);
}
