#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_ACCOUNTS 100
#define MAX_TRANSACTIONS 1000
#define FILE_ACCOUNTS "accounts.dat"
#define FILE_TRANSACTIONS "transactions.dat"

// ================= STRUCTURES =================
typedef struct {
    int accNo;
    char name[50];
    char pin[5];
    double balance;
    int active;
} Account;

typedef struct {
    int accNo;
    char type[20];
    double amount;
    double newBalance;
    time_t time;
} Transaction;

// ================= GLOBAL =================
Account accounts[MAX_ACCOUNTS];
Transaction trans[MAX_TRANSACTIONS];
int accCount = 0, transCount = 0;

// ================= FUNCTION DECLARATIONS =================
void loadAccounts();
void saveAccounts();
void loadTransactions();
void saveTransactions();

int findAccount(int accNo);
void createAccount();
void displayAccount(int accNo);
void updateAccount(int accNo);
void deleteAccount(int accNo);

void deposit(int accNo);
void withdraw(int accNo);
void showTransactions(int accNo);

void adminMenu();
void userMenu(int accNo);
int login();

void displayAllAccounts();
void searchAccount();

void clearBuffer();
int getInt();
double getDouble();

// ================= MAIN =================
int main() {
    loadAccounts();
    loadTransactions();

    while (1) {
        printf("\n===== BANK SYSTEM =====\n");
        printf("1. Login\n2. Admin\n3. Search\n4. Show All\n5. Exit\n");
        int ch = getInt();

        if (ch == 1) {
            int acc = login();
            if (acc != -1) userMenu(acc);
        }
        else if (ch == 2) adminMenu();
        else if (ch == 3) searchAccount();
        else if (ch == 4) displayAllAccounts();
        else {
            saveAccounts();
            saveTransactions();
            printf("Exiting...\n");
            break;
        }
    }
    return 0;
}

// ================= FILE HANDLING =================
void loadAccounts() {
    FILE *f = fopen(FILE_ACCOUNTS, "rb");
    if (!f) return;
    accCount = fread(accounts, sizeof(Account), MAX_ACCOUNTS, f);
    fclose(f);
}

void saveAccounts() {
    FILE *f = fopen(FILE_ACCOUNTS, "wb");
    fwrite(accounts, sizeof(Account), accCount, f);
    fclose(f);
}

void loadTransactions() {
    FILE *f = fopen(FILE_TRANSACTIONS, "rb");
    if (!f) return;
    transCount = fread(trans, sizeof(Transaction), MAX_TRANSACTIONS, f);
    fclose(f);
}

void saveTransactions() {
    FILE *f = fopen(FILE_TRANSACTIONS, "wb");
    fwrite(trans, sizeof(Transaction), transCount, f);
    fclose(f);
}

// ================= UTILITY =================
int getInt() {
    int x;
    scanf("%d", &x);
    clearBuffer();
    return x;
}

double getDouble() {
    double x;
    scanf("%lf", &x);
    clearBuffer();
    return x;
}

void clearBuffer() {
    while (getchar() != '\n');
}

// ================= ACCOUNT =================
int findAccount(int accNo) {
    for (int i = 0; i < accCount; i++)
        if (accounts[i].accNo == accNo && accounts[i].active)
            return i;
    return -1;
}

void createAccount() {
    if (accCount >= MAX_ACCOUNTS) {
        printf("Limit reached\n");
        return;
    }

    Account *a = &accounts[accCount];

    a->accNo = 100000 + rand() % 900000;

    printf("Name: ");
    fgets(a->name, 50, stdin);
    a->name[strcspn(a->name, "\n")] = 0;

    printf("PIN (4 digits): ");
    scanf("%s", a->pin);
    clearBuffer();

    printf("Initial Deposit: ");
    a->balance = getDouble();

    a->active = 1;
    accCount++;

    printf("Account Created! Acc No: %d\n", a->accNo);
}

void displayAccount(int accNo) {
    int i = findAccount(accNo);
    if (i == -1) return;

    printf("\nAccount: %d\nName: %s\nBalance: %.2f\n",
           accounts[i].accNo, accounts[i].name, accounts[i].balance);
}

void updateAccount(int accNo) {
    int i = findAccount(accNo);
    if (i == -1) return;

    printf("New Name: ");
    fgets(accounts[i].name, 50, stdin);
}

void deleteAccount(int accNo) {
    int i = findAccount(accNo);
    if (i != -1) {
        accounts[i].active = 0;
        printf("Deleted\n");
    }
}

// ================= BANK =================
void deposit(int accNo) {
    int i = findAccount(accNo);
    double amt = getDouble();

    accounts[i].balance += amt;

    if (transCount < MAX_TRANSACTIONS) {
        Transaction *t = &trans[transCount++];
        t->accNo = accNo;
        strcpy(t->type, "DEPOSIT");
        t->amount = amt;
        t->newBalance = accounts[i].balance;
        t->time = time(NULL);
    }
}

void withdraw(int accNo) {
    int i = findAccount(accNo);
    double amt = getDouble();

    if (amt > accounts[i].balance) {
        printf("Insufficient\n");
        return;
    }

    accounts[i].balance -= amt;

    if (transCount < MAX_TRANSACTIONS) {
        Transaction *t = &trans[transCount++];
        strcpy(t->type, "WITHDRAW");
        t->accNo = accNo;
        t->amount = amt;
        t->newBalance = accounts[i].balance;
        t->time = time(NULL);
    }
}

void showTransactions(int accNo) {
    for (int i = 0; i < transCount; i++) {
        if (trans[i].accNo == accNo) {
            printf("%s %.2f Balance: %.2f %s",
                   trans[i].type,
                   trans[i].amount,
                   trans[i].newBalance,
                   ctime(&trans[i].time));
        }
    }
}

// ================= MENUS =================
void adminMenu() {
    printf("\n1.Create\n2.View\n3.Update\n4.Delete\n");
    int ch = getInt();
    int acc;

    if (ch == 1) createAccount();
    else {
        printf("Acc No: ");
        acc = getInt();
        if (ch == 2) displayAccount(acc);
        else if (ch == 3) updateAccount(acc);
        else if (ch == 4) deleteAccount(acc);
    }
}

int login() {
    int acc;
    char pin[5];

    printf("Acc No: ");
    acc = getInt();

    int i = findAccount(acc);
    if (i == -1) return -1;

    printf("PIN: ");
    scanf("%s", pin);

    if (strcmp(pin, accounts[i].pin) == 0) return acc;

    printf("Wrong PIN\n");
    return -1;
}

void userMenu(int accNo) {
    while (1) {
        printf("\n1.Balance\n2.Deposit\n3.Withdraw\n4.History\n5.Exit\n");
        int ch = getInt();

        if (ch == 1) displayAccount(accNo);
        else if (ch == 2) deposit(accNo);
        else if (ch == 3) withdraw(accNo);
        else if (ch == 4) showTransactions(accNo);
        else break;
    }
}

// ================= EXTRA =================
void searchAccount() {
    printf("Enter Acc No: ");
    int acc = getInt();
    displayAccount(acc);
}

void displayAllAccounts() {
    for (int i = 0; i < accCount; i++)
        if (accounts[i].active)
            printf("%d %s %.2f\n",
                   accounts[i].accNo,
                   accounts[i].name,
                   accounts[i].balance);
}