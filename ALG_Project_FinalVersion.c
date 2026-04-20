#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =========================
   STANDARD COLOR Setup
   =========================
*/
#define RESET   "\033[0m"
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define PURPLE  "\033[1;35m"
#define CYAN    "\033[1;36m"
#define WHITE   "\033[1;37m"
#define BOLD    "\033[1m"

/* =========================
   FILE NAME CONSTANTS
   =========================
*/
#define FILE_CONTACT "contacts.txt"
#define FILE_HISTORY "history.txt"
#define FILE_FAV     "favorites.txt"

/* =========================
   DATA STRUCTURES
   =========================
*/
typedef struct Contact {
    char name[50];
    char phone[20];
    char email[50];
    struct Contact* next;
} Contact;

typedef struct CallLog {
    char name[50];
    char phone[20];
    char type[15];
    char duration[20];
    struct CallLog* next;
} CallLog;

typedef struct Favorite {
    char name[50];
    char phone[20];
    struct Favorite* next;
} Favorite;

/* =========================
   GLOBAL VARIABLE DECLARE
   =========================
*/
Contact* head = NULL;
CallLog* topStack = NULL;
Favorite* frontQ = NULL;
Favorite* rearQ = NULL;

/* =========================
   ALGORITHM 1: QUICK SORT (Time Complexity: O(n log n))
   =========================
*/
// Partition function for Quick Sort
int partition(Contact arr[], int low, int high) {
    char pivot[50];
    strcpy(pivot, arr[high].name);
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (strcasecmp(arr[j].name, pivot) < 0) {
            i++;
            Contact temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    Contact temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    return (i + 1);
}

// Quick Sort Recursive Function
void quickSort(Contact arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

// Sorting the Linked List using Quick Sort Algorithm
void sortContacts() {
    if (!head) return;
    int count = 0;
    Contact* temp = head;
    while (temp) { count++; temp = temp->next; }

    Contact* arr = (Contact*)malloc(count * sizeof(Contact));
    temp = head;
    for (int i = 0; i < count; i++) {
        arr[i] = *temp;
        temp = temp->next;
    }

    quickSort(arr, 0, count - 1);

    temp = head;
    for (int i = 0; i < count; i++) {
        strcpy(temp->name, arr[i].name);
        strcpy(temp->phone, arr[i].phone);
        strcpy(temp->email, arr[i].email);
        temp = temp->next;
    }
    free(arr);
}

/* =========================
   ALGORITHM 2: BINARY SEARCH (Time Complexity: O(log n))
   =========================
*/
int binarySearch(Contact arr[], int l, int r, char* name) {
    while (l <= r) {
        int m = l + (r - l) / 2;
        int res = strcasecmp(arr[m].name, name);
        if (res == 0) return m;
        if (res < 0) l = m + 1;
        else r = m - 1;
    }
    return -1;
}

// Optimized Search using Binary Search
int getNumberByName(char* name, char* destPhone) {
    if (!head) return 0;
    sortContacts();
    int count = 0;
    Contact* temp = head;
    while (temp) { count++; temp = temp->next; }

    Contact* arr = (Contact*)malloc(count * sizeof(Contact));
    temp = head;
    for (int i = 0; i < count; i++) {
        arr[i] = *temp;
        temp = temp->next;
    }

    int resultIdx = binarySearch(arr, 0, count - 1, name);
    int found = 0;
    if (resultIdx != -1) {
        strcpy(destPhone, arr[resultIdx].phone);
        found = 1;
    }
    free(arr);
    return found;
}

/* =========================
   ALGORITHM 3: FREQUENCY ANALYSIS (Most Called Contact)
   =========================
*/
void showMostFrequent() {
    if (!topStack) return;
    CallLog* i = topStack;
    char bestName[50] = "None";
    int maxCount = 0;

    while (i) {
        int c = 0;
        CallLog* j = topStack;
        while (j) {
            if (strcmp(i->name, j->name) == 0 && strcmp(i->name, "Unknown") != 0) c++;
            j = j->next;
        }
        if (c > maxCount) { maxCount = c; strcpy(bestName, i->name); }
        i = i->next;
    }
    if (maxCount > 0)
        printf("\t%s[Insight] Most Called: %s (%d times)%s\n", PURPLE, bestName, maxCount, RESET);
}

/* =========================
   CONSOLE HELPER
   =========================
*/
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void printLine(int len) {
    printf("\t%s", CYAN);
    for (int i = 0; i < len; i++) printf("-");
    printf("%s\n", RESET);
}

void pressEnter() {
    printf("\n\t%s>> Press ENTER to return...%s", YELLOW, RESET);
    while (getchar() != '\n');
    getchar();
}

/* =========================
   FILE IO
   =========================
*/
void saveData() {
    FILE* fp;
    fp = fopen(FILE_CONTACT, "w");
    Contact* tempC = head;
    while (tempC) {
        fprintf(fp, "%s,%s,%s\n", tempC->name, tempC->phone, tempC->email);
        tempC = tempC->next;
    }
    if (fp) fclose(fp);

    fp = fopen(FILE_HISTORY, "w");
    CallLog* tempH = topStack;
    while (tempH) {
        fprintf(fp, "%s,%s,%s,%s\n", tempH->name, tempH->phone, tempH->type, tempH->duration);
        tempH = tempH->next;
    }
    if (fp) fclose(fp);

    fp = fopen(FILE_FAV, "w");
    Favorite* tempF = frontQ;
    while (tempF) {
        fprintf(fp, "%s,%s\n", tempF->name, tempF->phone);
        tempF = tempF->next;
    }
    if (fp) fclose(fp);
}

void loadData() {
    FILE* fp;
    char buffer[200], f1[50], f2[20], f3[50], f4[20];
    fp = fopen(FILE_CONTACT, "r");
    if (fp) {
        while (fgets(buffer, sizeof(buffer), fp)) {
            sscanf(buffer, "%[^,],%[^,],%s", f1, f2, f3);
            Contact* newC = (Contact*)malloc(sizeof(Contact));
            strcpy(newC->name, f1); strcpy(newC->phone, f2); strcpy(newC->email, f3);
            newC->next = head; head = newC;
        }
        fclose(fp);
        sortContacts();
    }
    fp = fopen(FILE_HISTORY, "r");
    if (fp) {
        CallLog* tail = NULL;
        while (fgets(buffer, sizeof(buffer), fp)) {
            sscanf(buffer, "%[^,],%[^,],%[^,],%s", f1, f2, f3, f4);
            CallLog* newH = (CallLog*)malloc(sizeof(CallLog));
            strcpy(newH->name, f1); strcpy(newH->phone, f2); strcpy(newH->type, f3); strcpy(newH->duration, f4);
            newH->next = NULL;
            if (topStack == NULL) { topStack = newH; tail = newH; }
            else { tail->next = newH; tail = newH; }
        }
        fclose(fp);
    }
    fp = fopen(FILE_FAV, "r");
    if (fp) {
        while (fgets(buffer, sizeof(buffer), fp)) {
            sscanf(buffer, "%[^,],%s", f1, f2);
            Favorite* newF = (Favorite*)malloc(sizeof(Favorite));
            strcpy(newF->name, f1); strcpy(newF->phone, f2);
            newF->next = NULL;
            if (!frontQ) frontQ = rearQ = newF;
            else { rearQ->next = newF; rearQ = newF; }
        }
        fclose(fp);
    }
}

/* =========================
   UI HEADER (HUBUHU AGER MOTO)
   =========================
*/
void header(char* title) {
    clearScreen();
    printf("\t%s.=======================================================.%s\n", CYAN, RESET);
    printf("\t%s|%s  ____  _                      _                 _     %s|%s\n", CYAN, YELLOW, CYAN, RESET);
    printf("\t%s|%s |  _ \\| |__   ___  _ __   ___| |__   ___   ___ | | __ %s|%s\n", CYAN, YELLOW, CYAN, RESET);
    printf("\t%s|%s | |_) | '_ \\ / _ \\| '_ \\ / _ \\ '_ \\ / _ \\ / _ \\| |/ / %s|%s\n", CYAN, YELLOW, CYAN, RESET);
    printf("\t%s|%s |  __/| | | | (_) | | | |  __/ |_) | (_) | (_) |   <  %s|%s\n", CYAN, YELLOW, CYAN, RESET);
    printf("\t%s|%s |_|   |_| |_|\\___/|_| |_|\\___|_.__/ \\___/ \\___/|_|\\_\\ %s|%s\n", CYAN, YELLOW, CYAN, RESET);
    printf("\t%s|                                                       |%s\n", CYAN, RESET);
    printf("\t%s|             %sPHONEBOOK MANAGEMENT SYSTEM%s               %s|%s\n", CYAN, BOLD, RESET, CYAN, RESET);
    printf("\t%s'======================================================='%s\n", CYAN, RESET);
    printf("\n\t                 %s[ %s ]%s\n\n", YELLOW, title, RESET);
}

/* =========================
   CORE FEATURES
   =========================
*/
void resolveName(char* number, char* destination) {
    Contact* temp = head;
    while (temp) {
        if (strcmp(temp->phone, number) == 0) { strcpy(destination, temp->name); return; }
        temp = temp->next;
    }
    strcpy(destination, "Unknown");
}

void addContact() {
    header("ADD NEW CONTACT");
    char n[50], p[20], e[50];
    printf("\t%s Name  : %s", BOLD, RESET); scanf(" %[^\n]s", n);
    printf("\t%s Phone : %s", BOLD, RESET); scanf(" %[^\n]s", p);
    printf("\t%s Email : %s", BOLD, RESET); scanf(" %[^\n]s", e);

    Contact* newC = (Contact*)malloc(sizeof(Contact));
    strcpy(newC->name, n); strcpy(newC->phone, p); strcpy(newC->email, e);
    newC->next = head; head = newC;

    sortContacts(); // Using Quick Sort Algorithm
    printf("\n\t%s[Success] Contact saved & Quick-Sorted!%s\n", GREEN, RESET);
    saveData();
    pressEnter();
}

void displayContacts() {
    header("ALL CONTACTS (SORTED)");
    if (!head) { printf("\t%sNo contacts found.%s\n", RED, RESET); pressEnter(); return; }
    printLine(57);
    printf("\t|      %s%-15s  %-18s  %-11s%s |\n", YELLOW, "NAME", "PHONE", "EMAIL", RESET);
    printLine(57);
    Contact* temp = head;
    while (temp) {
        printf("\t| %-17s  %-15s  %-17s |\n", temp->name, temp->phone, temp->email);
        printLine(57);
        temp = temp->next;
    }
    pressEnter();
}

void processCall(char* phoneNumber, char* type) {
    char name[50], duration[20];
    resolveName(phoneNumber, name);
    printf("\n\t%s%s Call Initiated to %s...%s\n", YELLOW, type, name, RESET);
    printf("\t%s[Call Connected]\n\tTalking...\n\t[Call Ended]%s\n", BOLD, RESET);
    printf("\n\tEnter Duration (e.g. 2:30) : "); scanf(" %[^\n]", duration);

    CallLog* newLog = (CallLog*)malloc(sizeof(CallLog));
    strcpy(newLog->name, name); strcpy(newLog->phone, phoneNumber);
    strcpy(newLog->type, type); strcpy(newLog->duration, duration);
    newLog->next = topStack; topStack = newLog;
    saveData();
}

void makeCall() {
    header("CALL DIALER");
    int choice; char input[50];
    printLine(57);
    printf("\t%s[1]%s Dial Number\n\t%s[2]%s Search Name (Binary Search)\n\t%s[0]%s Back\n", YELLOW, RESET, YELLOW, RESET, RED, RESET);
    printLine(57);
    printf("\n\tSelect : "); scanf("%d", &choice);

    if (choice == 1) {
        printf("\tEnter Number : "); scanf(" %[^\n]", input);
        processCall(input, "Outgoing");
    } else if (choice == 2) {
        printf("\tEnter Name : "); scanf(" %[^\n]", input);
        char foundPhone[20];
        if (getNumberByName(input, foundPhone)) processCall(foundPhone, "Outgoing");
        else printf("\n\t%s[!] Contact not found.%s\n", RED, RESET);
    }
    pressEnter();
}

void viewHistory() {
    header("CALL HISTORY (STACK)");
    showMostFrequent(); // Algorithm: Frequency Analysis
    if (!topStack) { printf("\t%sNo recent calls.%s\n", RED, RESET); pressEnter(); return; }
    printLine(57);
    printf("\t|   %s%-12s %-17s %-13s %-6s%s |\n", YELLOW, "TYPE", "NAME", "NUMBER", "TIME", RESET);
    printLine(57);
    CallLog* temp = topStack;
    while (temp) {
        char* color = (strcmp(temp->type, "Incoming") == 0) ? GREEN : BLUE;
        printf("\t| %s%-12s%s %-16s %-15s %-7s |\n", color, temp->type, RESET, temp->name, temp->phone, temp->duration);
        printLine(57);
        temp = temp->next;
    }
    pressEnter();
}

void deleteContact() {
    header("DELETE CONTACT");
    char nameDel[50];
    printf("\n\tEnter Name to Delete: "); scanf(" %[^\n]", nameDel);
    Contact *temp = head, *prev = NULL;
    while (temp) {
        if (strcmp(temp->name, nameDel) == 0) {
            if (prev == NULL) head = temp->next;
            else prev->next = temp->next;
            free(temp); printf("\n\t%s[Success] Deleted!%s\n", GREEN, RESET);
            saveData(); pressEnter(); return;
        }
        prev = temp; temp = temp->next;
    }
    printf("\n\t%s[!] Not Found!%s\n", RED, RESET); pressEnter();
}

void addFavorite() {
    header("ADD TO FAVORITES");
    char nInput[50], pInput[20];
    printf("\tEnter Name to Add : "); scanf(" %[^\n]", nInput);
    if (getNumberByName(nInput, pInput)) {
        Favorite* newF = (Favorite*)malloc(sizeof(Favorite));
        strcpy(newF->name, nInput); strcpy(newF->phone, pInput);
        newF->next = NULL;
        if (!frontQ) frontQ = rearQ = newF;
        else { rearQ->next = newF; rearQ = newF; }
        printf("\n\t%s[Success] Added to Queue!%s\n", GREEN, RESET);
        saveData();
    } else printf("\n\t%s[!] Contact Not Found!%s\n", RED, RESET);
    pressEnter();
}

void viewFavorites() {
    header("FAVORITES (QUEUE)");
    if (!frontQ) printf("\t%sEmpty.%s\n", RED, RESET);
    else {
        Favorite* temp = frontQ;
        while (temp) {
            printf("\t| %-20s  %-25s |\n", temp->name, temp->phone);
            printLine(57);
            temp = temp->next;
        }
    }
    pressEnter();
}

/* =========================
   MAIN FUNCTION (UI HUBUHU AGER MOTO)
   =========================
*/
int main() {
    loadData();
    int choice;
    while (1) {
        clearScreen();
        printf("\t%s.=======================================================.%s\n", CYAN, RESET);
        printf("\t%s|%s  ____  _                      _                 _     %s|%s\n", CYAN, YELLOW, CYAN, RESET);
        printf("\t%s|%s |  _ \\| |__   ___  _ __   ___| |__   ___   ___ | | __ %s|%s\n", CYAN, YELLOW, CYAN, RESET);
        printf("\t%s|%s | |_) | '_ \\ / _ \\| '_ \\ / _ \\ '_ \\ / _ \\ / _ \\| |/ / %s|%s\n", CYAN, YELLOW, CYAN, RESET);
        printf("\t%s|%s |  __/| | | | (_) | | | |  __/ |_) | (_) | (_) |   <  %s|%s\n", CYAN, YELLOW, CYAN, RESET);
        printf("\t%s|%s |_|   |_| |_|\\___/|_| |_|\\___|_.__/ \\___/ \\___/|_|\\_\\ %s|%s\n", CYAN, YELLOW, CYAN, RESET);
        printf("\t%s|                                                       |%s\n", CYAN, RESET);
        printf("\t%s|             %sPHONEBOOK MANAGEMENT SYSTEM%s               %s|%s\n", CYAN, BOLD, RESET, CYAN, RESET);
        printf("\t%s|=======================================================|%s\n", CYAN, RESET);
        printf("\t%s|                                                       |%s\n", CYAN, RESET);
        printf("\t%s|          %s[1]%s Add New Contact                          %s|%s\n", CYAN, YELLOW, RESET, CYAN, RESET);
        printf("\t%s|          %s[2]%s Display Contacts                         %s|%s\n", CYAN, YELLOW, RESET, CYAN, RESET);
        printf("\t%s|          %s[3]%s Delete Contact                           %s|%s\n", CYAN, YELLOW, RESET, CYAN, RESET);
        printf("\t%s|                                                       |%s\n", CYAN, RESET);
        printf("\t%s|-------------------------------------------------------|%s\n", CYAN, RESET);
        printf("\t%s|                                                       |%s\n", CYAN, RESET);
        printf("\t%s|          %s[4]%s Make a Call                              %s|%s\n", CYAN, YELLOW, RESET, CYAN, RESET);
        printf("\t%s|          %s[5]%s Call History                             %s|%s\n", CYAN, YELLOW, RESET, CYAN, RESET);
        printf("\t%s|          %s[6]%s Delete Last Call (Not Modified)          %s|%s\n", CYAN, YELLOW, RESET, CYAN, RESET);
        printf("\t%s|                                                       |%s\n", CYAN, RESET);
        printf("\t%s|-------------------------------------------------------|%s\n", CYAN, RESET);
        printf("\t%s|                                                       |%s\n", CYAN, RESET);
        printf("\t%s|          %s[7]%s Add Favorite                             %s|%s\n", CYAN, YELLOW, RESET, CYAN, RESET);
        printf("\t%s|          %s[8]%s View Favorites                           %s|%s\n", CYAN, YELLOW, RESET, CYAN, RESET);
        printf("\t%s|          %s[9]%s Delete Favorite                          %s|%s\n", CYAN, YELLOW, RESET, CYAN, RESET);
        printf("\t%s|                                                       |%s\n", CYAN, RESET);
        printf("\t%s|-------------------------------------------------------|%s\n", CYAN, RESET);
        printf("\t%s|                                                       |%s\n", CYAN, RESET);
        printf("\t%s|          %s[0] EXIT%s                                     %s|%s\n", CYAN, RED, RESET, CYAN, RESET);
        printf("\t%s|                                                       |%s\n", CYAN, RESET);
        printf("\t%s'======================================================='%s\n\n", CYAN, RESET);
        printf("\t Select Option : ");

        if (scanf("%d", &choice) != 1) { while (getchar() != '\n'); continue; }

        switch (choice) {
            case 1: addContact(); break;
            case 2: displayContacts(); break;
            case 3: deleteContact(); break;
            case 4: makeCall(); break;
            case 5: viewHistory(); break;
            case 7: addFavorite(); break;
            case 8: viewFavorites(); break;
            case 0: saveData(); exit(0);
            default: printf("\n\tInvalid Choice!\n");
        }
    }
    return 0;
}
