#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STR 100
#define FILENAME "library.dat"

// Book structure
typedef struct Book {
    int id;
    char title[MAX_STR];
    char author[MAX_STR];
    int is_issued;
    char issued_to[MAX_STR];
    struct Book* next;
} Book;

// Global variables
Book* head = NULL;
int book_count = 0;

// Function prototypes
void displayMenu();
void addBook();
void removeBook();
void issueBook();
void returnBook();
void displayBooks();
void saveToFile();
void loadFromFile();
Book* createBook(int id, char* title, char* author);
void insertBook(Book* newBook);
Book* searchBook(int id);
Book* searchBookByTitle(char* title);
void freeList();
void clearInputBuffer();

int main() {
    loadFromFile();
    
    int choice;
    
    printf("=== Library Management System ===\n");
    
    do {
        displayMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clearInputBuffer();
        
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
                saveToFile();
                printf("Data saved successfully!\n");
                break;
            case 7:
                printf("Exiting... Thank you!\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
        }
        printf("\n");
    } while(choice != 7);
    
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
    printf("6. Save Data to File\n");
    printf("7. Exit\n");
}

void addBook() {
    char title[MAX_STR], author[MAX_STR];
    int id;
    
    printf("\n=== Add New Book ===\n");
    
    // Generate automatic ID
    id = book_count + 1;
    
    printf("Enter book title: ");
    fgets(title, MAX_STR, stdin);
    title[strcspn(title, "\n")] = 0; // Remove newline
    
    printf("Enter author name: ");
    fgets(author, MAX_STR, stdin);
    author[strcspn(author, "\n")] = 0; // Remove newline
    
    // Check if book with same title already exists
    Book* existing = searchBookByTitle(title);
    if (existing != NULL) {
        printf("Error: A book with title '%s' already exists!\n", title);
        return;
    }
    
    Book* newBook = createBook(id, title, author);
    if (newBook == NULL) {
        printf("Error: Failed to add book!\n");
        return;
    }
    
    insertBook(newBook);
    book_count++;
    printf("Book added successfully! Book ID: %d\n", id);
}

void removeBook() {
    int id;
    
    printf("\n=== Remove Book ===\n");
    
    if (head == NULL) {
        printf("No books in the library!\n");
        return;
    }
    
    printf("Enter book ID to remove: ");
    scanf("%d", &id);
    clearInputBuffer();
    
    Book* current = head;
    Book* prev = NULL;
    
    while (current != NULL) {
        if (current->id == id) {
            if (current->is_issued) {
                printf("Cannot remove book! It's currently issued to: %s\n", current->issued_to);
                return;
            }
            
            if (prev == NULL) {
                // Removing head
                head = current->next;
            } else {
                prev->next = current->next;
            }
            
            free(current);
            book_count--;
            printf("Book removed successfully!\n");
            return;
        }
        prev = current;
        current = current->next;
    }
    
    printf("Book with ID %d not found!\n", id);
}

void issueBook() {
    int id;
    char issued_to[MAX_STR];
    
    printf("\n=== Issue Book ===\n");
    
    if (head == NULL) {
        printf("No books in the library!\n");
        return;
    }
    
    printf("Enter book ID to issue: ");
    scanf("%d", &id);
    clearInputBuffer();
    
    Book* book = searchBook(id);
    if (book == NULL) {
        printf("Book with ID %d not found!\n", id);
        return;
    }
    
    if (book->is_issued) {
        printf("Book is already issued to: %s\n", book->issued_to);
        return;
    }
    
    printf("Enter borrower's name: ");
    fgets(issued_to, MAX_STR, stdin);
    issued_to[strcspn(issued_to, "\n")] = 0;
    
    book->is_issued = 1;
    strcpy(book->issued_to, issued_to);
    
    printf("Book '%s' issued successfully to %s!\n", book->title, issued_to);
}

void returnBook() {
    int id;
    
    printf("\n=== Return Book ===\n");
    
    if (head == NULL) {
        printf("No books in the library!\n");
        return;
    }
    
    printf("Enter book ID to return: ");
    scanf("%d", &id);
    clearInputBuffer();
    
    Book* book = searchBook(id);
    if (book == NULL) {
        printf("Book with ID %d not found!\n", id);
        return;
    }
    
    if (!book->is_issued) {
        printf("Book is not issued to anyone!\n");
        return;
    }
    
    printf("Book '%s' returned by %s!\n", book->title, book->issued_to);
    book->is_issued = 0;
    strcpy(book->issued_to, "");
}

void displayBooks() {
    Book* current = head;
    
    printf("\n=== All Books in Library ===\n");
    
    if (head == NULL) {
        printf("No books in the library!\n");
        return;
    }
    
    printf("%-5s %-30s %-25s %-10s %-20s\n", 
           "ID", "Title", "Author", "Status", "Issued To");
    printf("----------------------------------------------------------------------------\n");
    
    while (current != NULL) {
        char status[10];
        char issued_info[20];
        
        if (current->is_issued) {
            strcpy(status, "Issued");
            strcpy(issued_info, current->issued_to);
        } else {
            strcpy(status, "Available");
            strcpy(issued_info, "-");
        }
        
        printf("%-5d %-30s %-25s %-10s %-20s\n", 
               current->id, current->title, current->author, status, issued_info);
        current = current->next;
    }
    printf("Total books: %d\n", book_count);
}

Book* createBook(int id, char* title, char* author) {
    Book* newBook = (Book*)malloc(sizeof(Book));
    if (newBook == NULL) {
        return NULL;
    }
    
    newBook->id = id;
    strcpy(newBook->title, title);
    strcpy(newBook->author, author);
    newBook->is_issued = 0;
    strcpy(newBook->issued_to, "");
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

void saveToFile() {
    FILE* file = fopen(FILENAME, "wb");
    if (file == NULL) {
        printf("Error: Cannot open file for writing!\n");
        return;
    }
    
    Book* current = head;
    while (current != NULL) {
        fwrite(current, sizeof(Book), 1, file);
        current = current->next;
    }
    
    fclose(file);
}

void loadFromFile() {
    FILE* file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("No existing data file found. Starting with empty library.\n");
        return;
    }
    
    Book book;
    while (fread(&book, sizeof(Book), 1, file)) {
        Book* newBook = createBook(book.id, book.title, book.author);
        if (newBook != NULL) {
            newBook->is_issued = book.is_issued;
            strcpy(newBook->issued_to, book.issued_to);
            insertBook(newBook);
            book_count++;
        }
    }
    
    fclose(file);
    printf("Data loaded successfully! %d books found.\n", book_count);
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
