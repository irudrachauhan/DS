#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ----------------------------------------------
// STRUCTURE DEFINITIONS
// ----------------------------------------------

struct Book {
    int id;
    char title[50];
    char author[50];
    int isIssued; // 0 = available, 1 = issued
    struct Book *next;
};

struct Admin {
    char username[20];
    char password[20];
};

// ----------------------------------------------
// GLOBAL VARIABLES
// ----------------------------------------------
struct Book *head = NULL;
struct Admin admin = {"admin", "12345"};

// ---- Stack for Recently Returned Books ----
#define MAX_STACK 50
char returnedStack[MAX_STACK][50];
int top = -1;

// ---- Queue for Book Issue Requests ----
#define MAX_QUEUE 50
char requestQueue[MAX_QUEUE][50];
int front = -1, rear = -1;

// ---- Array for Book Titles (for Binary Search) ----
char bookTitles[100][50];
int totalBooks = 0;

// ----------------------------------------------
// FUNCTION DECLARATIONS
// ----------------------------------------------
int adminLogin();
void menu();
void addBook();
void displayBooks();
void deleteBook();
void issueBook();
void returnBook();
void searchBookBinary();
void enqueue(char title[]);
void dequeue();
void pushReturned(char title[]);
void showLastReturned();
void storeTitle(char title[]);
int binarySearch(char arr[][50], int n, char key[]);
void bubbleSortTitles();

// ----------------------------------------------
// MAIN FUNCTION
// ----------------------------------------------
int main() {
    printf("==============================================\n");
    printf("   📚 LIBRARY MANAGEMENT SYSTEM (v4.0) 📚     \n");
    printf("==============================================\n");

    if (!adminLogin()) {
        printf("\nAccess Denied. Exiting...\n");
        return 0;
    }

    int choice;
    do {
        menu();
        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        getchar(); // clear buffer

        switch(choice) {
            case 1: addBook(); break;
            case 2: displayBooks(); break;
            case 3: searchBookBinary(); break;
            case 4: deleteBook(); break;
            case 5: issueBook(); break;
            case 6: returnBook(); break;
            case 7: showLastReturned(); break;
            case 8: dequeue(); break;
            case 0: printf("Logging out... Goodbye!\n"); break;
            default: printf("Invalid choice! Try again.\n");
        }
    } while(choice != 0);

    return 0;
}

// ----------------------------------------------
// ADMIN LOGIN
// ----------------------------------------------
int adminLogin() {
    char user[20], pass[20];
    int attempts = 3;

    while (attempts > 0) {
        printf("\n---------- ADMIN LOGIN ----------\n");
        printf("Username: ");
        scanf("%s", user);
        printf("Password: ");
        scanf("%s", pass);

        if (strcmp(user, admin.username) == 0 && strcmp(pass, admin.password) == 0) {
            printf("\nogin Successful! Welcome, %s\n", user);
            return 1;
        } else {
            attempts--;
            printf("Incorrect credentials! Attempts left: %d\n", attempts);
        }
    }
    return 0;
}

// ----------------------------------------------
// MENU DISPLAY
// ----------------------------------------------
void menu() {
    printf("\n========== LIBRARY MENU ==========\n");
    printf("1. Add Book (Linked List + Array)\n");
    printf("2. Display All Books (Linked List)\n");
    printf("3. Search Book (Binary Search)\n");
    printf("4. Delete Book (Linked List)\n");
    printf("5. Issue Book (Queue)\n");
    printf("6. Return Book (Stack)\n");
    printf("7. Show Recently Returned (Stack)\n");
    printf("8. Process Next Issue Request (Queue)\n");
    printf("0. Logout & Exit\n");
    printf("==================================\n");
}

// ----------------------------------------------
// ADD NEW BOOK
// ----------------------------------------------
void addBook() {
    struct Book *newBook = (struct Book *)malloc(sizeof(struct Book));
    if (!newBook) {
        printf("Memory allocation failed!\n");
        return;
    }

    printf("Enter Book ID: ");
    scanf("%d", &newBook->id);
    getchar();
    printf("Enter Book Title: ");
    fgets(newBook->title, sizeof(newBook->title), stdin);
    newBook->title[strcspn(newBook->title, "\n")] = '\0';
    printf("Enter Author Name: ");
    fgets(newBook->author, sizeof(newBook->author), stdin);
    newBook->author[strcspn(newBook->author, "\n")] = '\0';

    newBook->isIssued = 0;
    newBook->next = NULL;

    if (head == NULL)
        head = newBook;
    else {
        struct Book *temp = head;
        while (temp->next)
            temp = temp->next;
        temp->next = newBook;
    }

    storeTitle(newBook->title);
    bubbleSortTitles(); // keep array sorted

    printf(" Book added successfully!\n");
}

// ----------------------------------------------
// DISPLAY ALL BOOKS
// ----------------------------------------------
void displayBooks() {
    if (head == NULL) {
        printf("No books available.\n");
        return;
    }

    struct Book *temp = head;
    printf("\n%-5s %-30s %-20s %-10s\n", "ID", "Title", "Author", "Status");
    printf("---------------------------------------------------------------\n");
    while (temp) {
        printf("%-5d %-30s %-20s %-10s\n",
               temp->id, temp->title, temp->author,
               temp->isIssued ? "Issued" : "Available");
        temp = temp->next;
    }
}

// ----------------------------------------------
// DELETE BOOK
// ----------------------------------------------
void deleteBook() {
    if (head == NULL) {
        printf("Library is empty.\n");
        return;
    }

    int id;
    printf("Enter Book ID to delete: ");
    scanf("%d", &id);

    struct Book *temp = head, *prev = NULL;

    if (head->id == id) {
        head = head->next;
        free(temp);
        printf("Book deleted successfully.\n");
        return;
    }

    while (temp && temp->id != id) {
        prev = temp;
        temp = temp->next;
    }

    if (!temp) {
        printf("Book not found.\n");
        return;
    }

    prev->next = temp->next;
    free(temp);
    printf("Book deleted successfully.\n");
}

// ----------------------------------------------
// ISSUE BOOK (QUEUE ENQUEUE)
// ----------------------------------------------
void issueBook() {
    char title[50];
    getchar();
    printf("Enter Book Title to issue: ");
    fgets(title, sizeof(title), stdin);
    title[strcspn(title, "\n")] = '\0';

    enqueue(title);
    printf("📚 Issue request for '%s' added to queue.\n", title);
}

// ----------------------------------------------
// RETURN BOOK (STACK PUSH)
// ----------------------------------------------
void returnBook() {
    char title[50];
    getchar();
    printf("Enter Book Title to return: ");
    fgets(title, sizeof(title), stdin);
    title[strcspn(title, "\n")] = '\0';

    pushReturned(title);
    printf(" Book '%s' returned successfully!\n", title);
}

// ----------------------------------------------
// SHOW RECENTLY RETURNED BOOKS (STACK PEEK)
// ----------------------------------------------
void showLastReturned() {
    if (top == -1) {
        printf("No books have been returned yet.\n");
        return;
    }

    printf("\n Recently Returned Books:\n");
    for (int i = top; i >= 0; i--)
        printf("-> %s\n", returnedStack[i]);
}

// ----------------------------------------------
// STACK FUNCTIONS
// ----------------------------------------------
void pushReturned(char title[]) {
    if (top == MAX_STACK - 1) {
        printf("Stack full! Cannot store more.\n");
        return;
    }
    strcpy(returnedStack[++top], title);
}

// ----------------------------------------------
// QUEUE FUNCTIONS
// ----------------------------------------------
void enqueue(char title[]) {
    if (rear == MAX_QUEUE - 1) {
        printf("Queue full! Cannot accept new requests.\n");
        return;
    }
    if (front == -1) front = 0;
    strcpy(requestQueue[++rear], title);
}

void dequeue() {
    if (front == -1) {
        printf("Queue empty! No pending requests.\n");
        return;
    }
    printf(" Processed issue request for: %s\n", requestQueue[front]);
    if (front == rear)
        front = rear = -1;
    else
        front++;
}

// ----------------------------------------------
// STORE TITLE FOR ARRAY + BINARY SEARCH
// ----------------------------------------------
void storeTitle(char title[]) {
    strcpy(bookTitles[totalBooks++], title);
}

// Sort book titles alphabetically for binary search
void bubbleSortTitles() {
    for (int i = 0; i < totalBooks - 1; i++) {
        for (int j = 0; j < totalBooks - i - 1; j++) {
            if (strcmp(bookTitles[j], bookTitles[j + 1]) > 0) {
                char temp[50];
                strcpy(temp, bookTitles[j]);
                strcpy(bookTitles[j], bookTitles[j + 1]);
                strcpy(bookTitles[j + 1], temp);
            }
        }
    }
}

// ----------------------------------------------
// BINARY SEARCH IMPLEMENTATION
// ----------------------------------------------
int binarySearch(char arr[][50], int n, char key[]) {
    int low = 0, high = n - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        int cmp = strcmp(arr[mid], key);
        if (cmp == 0)
            return mid;
        else if (cmp < 0)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return -1;
}

// ----------------------------------------------
// SEARCH BOOK (BINARY SEARCH ON TITLES)
// ----------------------------------------------
void searchBookBinary() {
    if (totalBooks == 0) {
        printf("No books available for searching.\n");
        return;
    }

    char key[50];
    getchar();
    printf("Enter Book Title to search: ");
    fgets(key, sizeof(key), stdin);
    key[strcspn(key, "\n")] = '\0';

    int index = binarySearch(bookTitles, totalBooks, key);
    if (index != -1)
        printf("Book '%s' found in the library.\n", key);
    else
        printf(" Book not found.\n");
}