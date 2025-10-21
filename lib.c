#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>

#define MAX_STR 100
#define FILENAME "library.dat"
#define USERFILE "users.dat"
#define LOGFILE "library.log"
#define MAX_BORROWER_NAME 50
#define MAX_USERS 100
#define MAX_USERNAME 30
#define MAX_PASSWORD 30
#define FINE_PER_DAY 5.0
#define MAX_ATTEMPTS 3

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

// User structure
typedef struct User {
    char username[MAX_USERNAME];
    unsigned long password_hash;
    int is_admin;
    char full_name[MAX_STR];
} User;

// Statistics structure
typedef struct {
    int total_books;
    int issued_books;
    int available_books;
    double total_fines;
} LibraryStats;

// Log levels
typedef enum {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} LogLevel;

// Global variables
Book* head = NULL;
User users[MAX_USERS];
int book_count = 0;
int user_count = 0;
int next_id = 1;
User* current_user = NULL;
volatile sig_atomic_t save_needed = 0;

// Function prototypes
void displayMainMenu();
void adminMenu();
void userMenu();
void registerUser();
int loginUser();
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
void saveUsersToFile();
void loadUsersFromFile();
Book* createBook(int id, char* title, char* author, char* isbn, int year);
void insertBook(Book* newBook);
Book* searchBook(int id);
Book* searchBookByTitle(char* title);
Book* searchBookByISBN(char* isbn);
void freeList();
void clearInputBuffer();
int getIntegerInput(const char* prompt);
int getIntegerInputSafe(const char* prompt, int min, int max);
void safe_strcpy(char* dest, const char* src, size_t dest_size);
void printBookDetails(Book* book);
int validateISBN(char* isbn);
int validateISBN13(const char* isbn);
void sortBooksByTitle();
void sortBooksByAuthor();
Book* mergeSort(Book* head, int (*compare)(Book*, Book*));
int compareByTitle(Book* a, Book* b);
int compareByAuthor(Book* a, Book* b);
void initializeDefaultAdmin();
unsigned long hash_password(const char* password);
void log_message(LogLevel level, const char* message);
double calculateFine(Book* book);
void backupDatabase();
void signal_handler(int signum);
void cleanup_and_exit();
char* strcasestr_custom(const char* haystack, const char* needle);

int main() {
    // Setup signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Initialize default admin account
    initializeDefaultAdmin();

    // Load users from file (this will override default if file exists)
    loadUsersFromFile();

    // Load library data
    loadFromFile();

    int choice;

    printf("=== Enhanced Library Management System ===\n");
    printf("Version 3.1 - Secure & Feature Complete\n");
    printf("Initialized with %d books and %d users\n", book_count, user_count);
    printf("   \n\n");
    log_message(LOG_INFO, "System started");

    do {
        displayMainMenu();
        choice = getIntegerInput("Enter your choice: ");

        switch(choice) {
            case 1:
                if (loginUser()) {
                    if (current_user->is_admin) {
                        printf("\nWelcome Admin: %s\n", current_user->full_name);
                        log_message(LOG_INFO, "Admin logged in");
                        adminMenu();
                    } else {
                        printf("\nWelcome User: %s\n", current_user->full_name);
                        log_message(LOG_INFO, "User logged in");
                        userMenu();
                    }
                    current_user = NULL; // Logout
                }
                break;
            case 2:
                registerUser();
                break;
            case 3:
                printf("Exiting... Thank you for using the Library Management System!\n");
                cleanup_and_exit();
                break;
            default:
                printf("Invalid choice! Please try again.\n");
        }
        printf("\n");
    } while(choice != 3);

    return 0;
}

void signal_handler(int signum) {
    printf("\n\nReceived interrupt signal. Saving data...\n");
    cleanup_and_exit();
}

void cleanup_and_exit() {
    saveToFile();
    saveUsersToFile();
    freeList();
    log_message(LOG_INFO, "System shutdown gracefully");
    exit(0);
}

void displayMainMenu() {
    printf("\n=== Library Management System ===\n");
    printf("1. Login\n");
    printf("2. Register as User\n");
    printf("3. Exit\n");
    printf("=================================\n");
}

void adminMenu() {
    int choice;

    do {
        printf("\n=== Admin Menu ===\n");
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
        printf("13. Backup Database\n");
        printf("14. Logout\n");
        printf("===================\n");

        choice = getIntegerInput("Enter your choice: ");

        switch(choice) {
            case 1:
                addBook();
                save_needed = 1;
                break;
            case 2:
                removeBook();
                save_needed = 1;
                break;
            case 3:
                issueBook();
                save_needed = 1;
                break;
            case 4:
                returnBook();
                save_needed = 1;
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
                saveUsersToFile();
                save_needed = 0;
                printf("Data saved successfully!\n");
                break;
            case 12:
                exportToText();
                break;
            case 13:
                backupDatabase();
                break;
            case 14:
                if (save_needed) {
                    printf("Save changes before logout? (y/n): ");
                    char ch;
                    scanf(" %c", &ch);
                    clearInputBuffer();
                    if (ch == 'y' || ch == 'Y') {
                        saveToFile();
                        saveUsersToFile();
                    }
                }
                printf("Logging out...\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
        }
    } while(choice != 14);
}

void userMenu() {
    int choice;

    do {
        printf("\n=== User Menu ===\n");
        printf("1. Display All Books\n");
        printf("2. Search Books\n");
        printf("3. View Book Details\n");
        printf("4. Request For Borrowing Books\n");
        printf("5. Return Book\n");
        printf("6. Sort Books by Title\n");
        printf("7. Sort Books by Author\n");
        printf("8. Library Statistics\n");
        printf("9. Logout\n");
        printf("==================\n");

        choice = getIntegerInput("Enter your choice: ");

        switch(choice) {
            case 1:
                displayBooks();
                break;
            case 2:
                searchBooks();
                break;
            case 3:
                viewBookDetails();
                break;
            case 4:
                issueBook();
                break;
            case 5:
                returnBook();
                break;
            case 6:
                sortBooksByTitle();
                break;
            case 7:
                sortBooksByAuthor();
                break;
            case 8:
                libraryStatistics();
                break;
            case 9:
                printf("Logging out...\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
        }
    } while(choice != 9);
}

unsigned long hash_password(const char* password) {
    unsigned long hash = 5381;
    int c;
    while ((c = *password++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

void log_message(LogLevel level, const char* message) {
    FILE* log = fopen(LOGFILE, "a");
    if (log) {
        time_t now = time(NULL);
        struct tm* t = localtime(&now);
        char* level_str[] = {"INFO", "WARNING", "ERROR"};
        fprintf(log, "[%04d-%02d-%02d %02d:%02d:%02d] %s: %s\n",
                t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                t->tm_hour, t->tm_min, t->tm_sec,
                level_str[level], message);
        fclose(log);
    }
}

void registerUser() {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    char full_name[MAX_STR];
    char confirm_password[MAX_PASSWORD];

    printf("\n=== User Registration ===\n");

    if (user_count >= MAX_USERS) {
        printf("Sorry, maximum user limit reached!\n");
        log_message(LOG_WARNING, "User registration failed - limit reached");
        return;
    }

    printf("Enter full name: ");
    fgets(full_name, MAX_STR, stdin);
    full_name[strcspn(full_name, "\n")] = 0;

    if (strlen(full_name) == 0) {
        printf("Name cannot be empty!\n");
        return;
    }

    printf("Enter username (4-29 chars): ");
    fgets(username, MAX_USERNAME, stdin);
    username[strcspn(username, "\n")] = 0;

    if (strlen(username) < 4) {
        printf("Username must be at least 4 characters long!\n");
        return;
    }

    // Check if username already exists
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            printf("Username already exists! Please choose a different username.\n");
            return;
        }
    }

    printf("Enter password (min 4 chars): ");
    fgets(password, MAX_PASSWORD, stdin);
    password[strcspn(password, "\n")] = 0;

    if (strlen(password) < 4) {
        printf("Password must be at least 4 characters long!\n");
        return;
    }

    printf("Confirm password: ");
    fgets(confirm_password, MAX_PASSWORD, stdin);
    confirm_password[strcspn(confirm_password, "\n")] = 0;

    if (strcmp(password, confirm_password) != 0) {
        printf("Passwords do not match! Registration failed.\n");
        return;
    }

    // Add new user with hashed password
    safe_strcpy(users[user_count].username, username, MAX_USERNAME);
    users[user_count].password_hash = hash_password(password);
    safe_strcpy(users[user_count].full_name, full_name, MAX_STR);
    users[user_count].is_admin = 0;
    user_count++;

    saveUsersToFile();
    log_message(LOG_INFO, "New user registered");
    printf("Registration successful! You can now login with your credentials.\n");
}

int loginUser() {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    int attempts = 0;

    printf("\n=== Login ===\n");

    while (attempts < MAX_ATTEMPTS) {
        printf("Enter username: ");
        fgets(username, MAX_USERNAME, stdin);
        username[strcspn(username, "\n")] = 0;

        printf("Enter password: ");
        fgets(password, MAX_PASSWORD, stdin);
        password[strcspn(password, "\n")] = 0;

        unsigned long input_hash = hash_password(password);

        // Check credentials
        for (int i = 0; i < user_count; i++) {
            if (strcmp(users[i].username, username) == 0 &&
                users[i].password_hash == input_hash) {
                current_user = &users[i];
                return 1; // Login successful
            }
        }

        attempts++;
        printf("Invalid username or password! Attempts remaining: %d\n",
               MAX_ATTEMPTS - attempts);
        log_message(LOG_WARNING, "Failed login attempt");
    }

    printf("Too many failed attempts. Returning to main menu.\n");
    return 0; // Login failed
}

void initializeDefaultAdmin() {
    // Create default admin account with hashed password
    safe_strcpy(users[0].username, "admin", MAX_USERNAME);
    users[0].password_hash = hash_password("admin123");
    safe_strcpy(users[0].full_name, "System Administrator", MAX_STR);
    users[0].is_admin = 1;
    user_count = 1;
}

void saveUsersToFile() {
    FILE* file = fopen(USERFILE, "w");
    if (file == NULL) {
        printf("Error: Cannot open user file for writing!\n");
        log_message(LOG_ERROR, "Cannot save users to file");
        return;
    }

    for (int i = 0; i < user_count; i++) {
        fprintf(file, "%s|%lu|%d|%s\n",
                users[i].username, users[i].password_hash,
                users[i].is_admin, users[i].full_name);
    }

    fclose(file);
}

void loadUsersFromFile() {
    FILE* file = fopen(USERFILE, "r");
    if (file == NULL) {
        printf("No existing user file found. Using default admin account.\n");
        log_message(LOG_INFO, "Using default admin account");
        return;
    }

    char line[256];
    int temp_count = 0;
    User temp_users[MAX_USERS];

    while (fgets(line, sizeof(line), file) && temp_count < MAX_USERS) {
        char* token;

        token = strtok(line, "|");
        if (token) safe_strcpy(temp_users[temp_count].username, token, MAX_USERNAME);

        token = strtok(NULL, "|");
        if (token) temp_users[temp_count].password_hash = strtoul(token, NULL, 10);

        token = strtok(NULL, "|");
        if (token) temp_users[temp_count].is_admin = atoi(token);

        token = strtok(NULL, "|");
        if (token) {
            token[strcspn(token, "\n")] = 0;
            safe_strcpy(temp_users[temp_count].full_name, token, MAX_STR);
        }

        temp_count++;
    }

    fclose(file);

    // If we loaded users, replace the default admin
    if (temp_count > 0) {
        user_count = temp_count;
        for (int i = 0; i < temp_count; i++) {
            users[i] = temp_users[i];
        }
        printf("Loaded %d users from file.\n", user_count);
        log_message(LOG_INFO, "Users loaded from file");
    } else {
        printf("No users found in file. Using default admin account.\n");
        log_message(LOG_INFO, "Using default admin account");
    }
}

void addBook() {
    if (current_user == NULL || !current_user->is_admin) {
        printf("Error: Only administrators can add books!\n");
        return;
    }

    char title[MAX_STR], author[MAX_STR], isbn[20];
    int year;

    printf("\n=== Add New Book ===\n");

    // Get book details
    printf("Enter book title: ");
    fgets(title, MAX_STR, stdin);
    title[strcspn(title, "\n")] = 0;

    if (strlen(title) == 0) {
        printf("Title cannot be empty!\n");
        return;
    }

    printf("Enter author name: ");
    fgets(author, MAX_STR, stdin);
    author[strcspn(author, "\n")] = 0;

    if (strlen(author) == 0) {
        printf("Author cannot be empty!\n");
        return;
    }

    printf("Enter ISBN: ");
    fgets(isbn, 20, stdin);
    isbn[strcspn(isbn, "\n")] = 0;

    if (!validateISBN(isbn)) {
        printf("Warning: ISBN format may be invalid. Continuing anyway...\n");
    }

    year = getIntegerInputSafe("Enter publication year", 1000, 2100);
    if (year == -1) return;

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
        log_message(LOG_ERROR, "Memory allocation failed for new book");
        return;
    }

    insertBook(newBook);
    book_count++;
    printf("Book added successfully! Book ID: %d\n", next_id);
    log_message(LOG_INFO, "Book added to library");
    next_id++;
}

void removeBook() {
    if (current_user == NULL || !current_user->is_admin) {
        printf("Error: Only administrators can remove books!\n");
        return;
    }

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
                log_message(LOG_INFO, "Book removed from library");
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
    if (current_user == NULL || !current_user->is_admin) {
        printf("Please go to admin officer to borrow books\n");
        return;
    }

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
        time_t now = time(NULL);
        printf("Issued on: %s", ctime(&book->issue_date));
        printf("Due date: %s", ctime(&book->due_date));
        return;
    }

    printf("Enter borrower's name: ");
    fgets(issued_to, MAX_BORROWER_NAME, stdin);
    issued_to[strcspn(issued_to, "\n")] = 0;

    if (strlen(issued_to) == 0) {
        printf("Borrower name cannot be empty!\n");
        return;
    }

    days = getIntegerInputSafe("Enter number of days for issuance", 1, 365);
    if (days == -1) return;

    book->is_issued = 1;
    safe_strcpy(book->issued_to, issued_to, MAX_BORROWER_NAME);
    book->issue_date = time(NULL);
    book->due_date = book->issue_date + (days * 24 * 60 * 60);

    printf("Book '%s' issued successfully to %s!\n", book->title, issued_to);
    printf("Due date: %s", ctime(&book->due_date));
    log_message(LOG_INFO, "Book issued");
}

double calculateFine(Book* book) {
    if (!book->is_issued) return 0.0;

    time_t now = time(NULL);
    double days_overdue = difftime(now, book->due_date) / (24 * 60 * 60);

    if (days_overdue > 0) {
        return days_overdue * FINE_PER_DAY;
    }
    return 0.0;
}

void returnBook() {
    if (current_user == NULL || !current_user->is_admin) {
        printf("Go to admin officer to return books\n");
        return;
    }

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
    double fine = calculateFine(book);

    printf("Book '%s' returned by %s!\n", book->title, book->issued_to);

    if (days_overdue > 0) {
        printf("Warning: This book is %.1f days overdue!\n", days_overdue);
        printf("Fine amount: %.2f currency units\n", fine);
    } else {
        printf("Book returned on time. No fine.\n");
    }

    book->is_issued = 0;
    book->issued_to[0] = '\0';
    book->issue_date = 0;
    book->due_date = 0;

    log_message(LOG_INFO, "Book returned");
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

char* strcasestr_custom(const char* haystack, const char* needle) {
    if (!*needle) return (char*)haystack;

    for (; *haystack; haystack++) {
        const char* h = haystack;
        const char* n = needle;

        while (*h && *n && (tolower((unsigned char)*h) == tolower((unsigned char)*n))) {
            h++;
            n++;
        }

        if (!*n) return (char*)haystack;
    }

    return NULL;
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
        if (strcasestr_custom(current->title, query) ||
            strcasestr_custom(current->author, query) ||
            strcasestr_custom(current->isbn, query)) {

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
    LibraryStats stats = {0, 0, 0, 0.0};
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
            stats.total_fines += calculateFine(current);
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
    printf("Total Pending Fines: %.2f currency units\n", stats.total_fines);
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
        log_message(LOG_ERROR, "Cannot save to file");
        return;
    }

    // Write metadata header
    fprintf(file, "VERSION:2\n");
    fprintf(file, "NEXT_ID:%d\n", next_id);
    fprintf(file, "BOOK_COUNT:%d\n", book_count);
    fprintf(file, "---\n");

    Book* current = head;
    while (current != NULL) {
        fprintf(file, "%d|%s|%s|%s|%d|%d|%s|%ld|%ld\n",
                current->id, current->title, current->author, current->isbn,
                current->year, current->is_issued, current->issued_to,
                (long)current->issue_date, (long)current->due_date);
        current = current->next;
    }

    fclose(file);
    log_message(LOG_INFO, "Data saved to file");
}

void loadFromFile() {
    FILE* file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("No existing data file found. Starting with empty library.\n");
        return;
    }

    char line[512];
    int version = 1;

    // Try to read version info
    if (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "VERSION:", 8) == 0) {
            version = atoi(line + 8);

            // Read next_id
            if (fgets(line, sizeof(line), file) && strncmp(line, "NEXT_ID:", 8) == 0) {
                next_id = atoi(line + 8);
            }

            // Read book_count (we'll verify this)
            if (fgets(line, sizeof(line), file) && strncmp(line, "BOOK_COUNT:", 11) == 0) {
                // Just skip, we'll count books as we load
            }

            // Read separator
            fgets(line, sizeof(line), file);
        } else {
            // Old format, rewind
            rewind(file);
        }
    }

    book_count = 0;
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
        if (token) book.issue_date = (time_t)atol(token);

        token = strtok(NULL, "|");
        if (token) book.due_date = (time_t)atol(token);

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

    // If we didn't get next_id from file, calculate it
    if (version == 1 || next_id <= max_id) {
        next_id = max_id;
    }

    fclose(file);
    printf("Loaded %d books from file.\n", book_count);
    log_message(LOG_INFO, "Data loaded from file");
}

void exportToText() {
    char base_filename[MAX_STR - 10];
    char filename[MAX_STR];

    printf("Enter filename for export (without extension): ");
    fgets(base_filename, sizeof(base_filename), stdin);
    base_filename[strcspn(base_filename, "\n")] = 0;

    if (strlen(base_filename) == 0) {
        strcpy(base_filename, "library_export");
    }

    // Safe concatenation
    snprintf(filename, MAX_STR, "%s.txt", base_filename);

    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Cannot create file %s!\n", filename);
        log_message(LOG_ERROR, "Cannot create export file");
        return;
    }

    time_t now = time(NULL);
    fprintf(file, "=== Library Catalog Export ===\n");
    fprintf(file, "Generated on: %s", ctime(&now));
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
            double fine = calculateFine(current);
            if (fine > 0) {
                fprintf(file, "Fine: %.2f currency units\n", fine);
            }
        }
        fprintf(file, "---------------------------\n");
        current = current->next;
    }

    fclose(file);
    printf("Library catalog exported to %s successfully!\n", filename);
    log_message(LOG_INFO, "Catalog exported to text file");
}

void backupDatabase() {
    char backup_name[MAX_STR];
    time_t now = time(NULL);
    struct tm* t = localtime(&now);

    snprintf(backup_name, MAX_STR, "library_backup_%04d%02d%02d_%02d%02d%02d.dat",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);

    FILE* src = fopen(FILENAME, "r");
    FILE* dest = fopen(backup_name, "w");

    if (!src) {
        printf("No data file to backup!\n");
        return;
    }

    if (!dest) {
        printf("Backup failed - cannot create backup file!\n");
        fclose(src);
        log_message(LOG_ERROR, "Backup creation failed");
        return;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), src)) {
        fputs(buffer, dest);
    }

    fclose(src);
    fclose(dest);
    printf("Backup created successfully: %s\n", backup_name);
    log_message(LOG_INFO, "Database backup created");
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

int getIntegerInputSafe(const char* prompt, int min, int max) {
    int value, attempts = 0;
    char buffer[100];

    while (attempts < MAX_ATTEMPTS) {
        printf("%s (%d-%d): ", prompt, min, max);
        if (fgets(buffer, sizeof(buffer), stdin)) {
            if (sscanf(buffer, "%d", &value) == 1) {
                if (value >= min && value <= max) {
                    return value;
                }
                printf("Value must be between %d and %d\n", min, max);
            } else {
                printf("Invalid input! Please enter a number.\n");
            }
        }
        attempts++;
    }
    printf("Too many invalid attempts.\n");
    return -1;
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
            double fine = calculateFine(book);
            printf("WARNING: This book is %.1f days overdue!\n", days_overdue);
            printf("Fine: %.2f currency units\n", fine);
        } else {
            double days_remaining = difftime(book->due_date, current_time) / (24 * 60 * 60);
            printf("Days remaining: %.1f\n", days_remaining);
        }
    }
}

int validateISBN13(const char* isbn) {
    if (strlen(isbn) != 13) return 0;

    int sum = 0;
    for (int i = 0; i < 12; i++) {
        if (!isdigit((unsigned char)isbn[i])) return 0;
        int digit = isbn[i] - '0';
        sum += (i % 2 == 0) ? digit : digit * 3;
    }

    int checksum = (10 - (sum % 10)) % 10;
    return checksum == (isbn[12] - '0');
}

int validateISBN(char* isbn) {
    int len = strlen(isbn);

    // Remove hyphens for validation
    char clean_isbn[20];
    int j = 0;
    for (int i = 0; i < len && j < 19; i++) {
        if (isdigit((unsigned char)isbn[i]) || isbn[i] == 'X' || isbn[i] == 'x') {
            clean_isbn[j++] = isbn[i];
        }
    }
    clean_isbn[j] = '\0';
    len = j;

    // Check for ISBN-10 or ISBN-13
    if (len == 10) {
        // Basic ISBN-10 validation
        return 1;
    } else if (len == 13) {
        return validateISBN13(clean_isbn);
    }

    return 0;
}

void sortBooksByTitle() {
    if (head == NULL || head->next == NULL) {
        printf("Not enough books to sort!\n");
        return;
    }
    head = mergeSort(head, compareByTitle);
    printf("Books sorted by title successfully!\n");
    log_message(LOG_INFO, "Books sorted by title");
}

void sortBooksByAuthor() {
    if (head == NULL || head->next == NULL) {
        printf("Not enough books to sort!\n");
        return;
    }
    head = mergeSort(head, compareByAuthor);
    printf("Books sorted by author successfully!\n");
    log_message(LOG_INFO, "Books sorted by author");
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

