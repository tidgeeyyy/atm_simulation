#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <conio.h>

#define BLUE "\033[34m"
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define CYAN "\033[36m"
#define YELLOW "\033[33m"
#define WHITE "\033[97m"

using namespace std;

const int MAX_ACCOUNT_NAME_LENGTH = 100;
const int MIN_INITIAL_DEPOSIT = 5000;
const double MAX_TRANSACTION_AMOUNT = 1000000;

void SetBlueBackgroundWhiteText() {
    system("color 17");
}

void ResetConsoleColors() {
    system("color 07");
}

struct ATMAccount {
    int accountNumber;
    string accountName;
    string birthday;
    string contactNumber;
    double balance;
    int pinCode;
    ATMAccount* next;

    ATMAccount(int accNumber, string accName, string dob, string contact, double bal, int pin)
        : accountNumber(accNumber), accountName(accName), birthday(dob),
          contactNumber(contact), balance(bal), pinCode(pin), next(nullptr) {}

    ~ATMAccount() {
        if (next) {
            delete next;
        }
    }
};

struct ATMList {
    ATMAccount* head;

    ATMList() : head(nullptr) {}

    ~ATMList() {
        while (head) {
            ATMAccount* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void Insert(ATMAccount* newAccount) {
        if (!head) {
            head = newAccount;
        } else {
            newAccount->next = head;
            head = newAccount;
        }
    }

    ATMAccount* Find(int accountNumber, int pinCode) {
    ATMAccount* current = head;
    while (current) {
        if (current->accountNumber == accountNumber) {
            if (current->pinCode == pinCode) {
                return current;
            } else {
                return nullptr;
            }
        }
        current = current->next;
    }

    return nullptr;
    }

    ATMAccount* FindByAccountNumber(int accountNumber) {
    ATMAccount* current = head;
    while (current) {
        if (current->accountNumber == accountNumber) {
            return current;
        }
        current = current->next;
        }
    return nullptr;
    }

};

ATMList accountList;
string cardFileName = "accounts.txt";
string cardFile = "ATM_CARD.txt";

bool IsAccountNumberTaken(int accountNumber) {
    ATMAccount* current = accountList.head;
    while (current) {
        if (current->accountNumber == accountNumber) {
            return true;
        }
        current = current->next;
    }
    return false;
}

int GenerateUniqueAccountNumber() {
    int accountNumber;
    do {
        accountNumber = rand() % 90000 + 10000;
    } while (IsAccountNumberTaken(accountNumber));
    return accountNumber;
}

bool IsValidAmount(double amount) {
    return amount >= 0 && amount <= MAX_TRANSACTION_AMOUNT;
}

void LogTransaction(const string& message) {
    ofstream logFile("transaction.log", ios::app);
    if (logFile.is_open()) {
        logFile << message << endl;
        logFile.close();
    }
}

int GetPINInput() {
    string pin = "";
    char c;
    const int maxLength = 4;

    while (true) {
        c = _getch();
        if (c == 13)
            break;
        else if (c == 8 && !pin.empty()) {
            cout << "\b \b";
            pin.pop_back();
        }
        else if (isdigit(c) && pin.length() < maxLength) {
            cout << '*';
            pin += c;
        }
    }

    cout << endl;
    return stoi(pin);
}

void SaveAccountsToFile(const string& fileName) {
    ofstream file(fileName);
    ATMAccount* current = accountList.head;

    while (current) {
        file << current->accountNumber << " " << current->pinCode << " " << current->balance << " "
             << current->accountName << " " << current->birthday << " " << current->contactNumber << endl;
        current = current->next;
    }

    file.close();
}


void InsertNewAccount(ATMAccount* newAccount) {
    accountList.Insert(newAccount);
}


void WaitForEnter() {
    cout << "\n\n\t\t\t\t\t\t\t\tPress Enter to continue...";
    cin.ignore();
    cin.get();
}


ATMAccount* EnrollNewAccount() {
    ATMAccount* newAccount;
    int accountNumber = GenerateUniqueAccountNumber();
    string accountName, birthday, contactNumber;
    double balance;
    int pinCode;

    cout << WHITE <<"\n\t\t\t\t\t\t\tEnter Account Name: ";
    cin.ignore();
    getline(cin, accountName);

    int birthYear, birthMonth, birthDay;
    bool validDate = false;

    while (!validDate) {
        cout << "\n\t\t\t\t\t\t\tEnter Birth Year (1880-2023): ";
        cin >> birthYear;

        cout << "\n\t\t\t\t\t\t\tEnter Birth Month (1-12): ";
        cin >> birthMonth;

        cout << "\n\t\t\t\t\t\t\tEnter Birth Day (1-31): ";
        cin >> birthDay;

        validDate = (birthYear >= 1880 && birthYear <= 2023) &&
                    (birthMonth >= 1 && birthMonth <= 12) &&
                    (birthDay >= 1 && birthDay <= 31);

        if (!validDate) {
            cout << YELLOW << "\n\t\t\t\t\t\t\tError: Please enter a valid date (Year: 1880-2023, Month: 1-12, Day: 1-31)." << endl;
        }
    }

    birthday = to_string(birthYear) + '-' + to_string(birthMonth) + '-' + to_string(birthDay);

    do {
        cout << "\n\t\t\t\t\t\t\tEnter Contact Number (Max. 11 digits): ";
        cin.ignore();
        getline(cin, contactNumber);
        if (contactNumber.length() > 11) {
            cout << YELLOW << "\n\t\t\t\t\t\t\tError: Contact number can have a maximum of 11 digits." << endl;
        }
    } while (contactNumber.length() > 11);

    do {
        cout << "\n\t\t\t\t\t\t\tEnter Initial Deposit (Min. " << MIN_INITIAL_DEPOSIT << "): ";
        cin >> balance;
        if (balance < MIN_INITIAL_DEPOSIT) {
            cout << YELLOW << "\n\t\t\t\t\t\t\tError: Initial deposit must be at least " << MIN_INITIAL_DEPOSIT << "." << endl;
        }
    } while (balance < MIN_INITIAL_DEPOSIT);

    do {
        cout << WHITE << "\n\t\t\t\t\tEnter 4-digit PIN Code (Please do NOT share your Account PIN code): ";
        pinCode = GetPINInput();
    } while (pinCode < 1000 || pinCode > 9999);

    cout << "\n\t\t\t\t\tYour Account Number is: " << accountNumber << '\n';
    newAccount = new ATMAccount(accountNumber, accountName, birthday, contactNumber, balance, pinCode);
    InsertNewAccount(newAccount);
    return newAccount;
}


void BalanceInquiry(int accountNumber, int pinCode) {
    system("cls");
    ATMAccount* account = accountList.Find(accountNumber, pinCode);

    if (account) {
        cout << "\n\n\n\n\n\n\n\t\t\t\t\t\t\tAccount Balance: " << account->balance << endl;
        WaitForEnter();
    } else {
        cout << "\n\n\n\n\n\n\n\t\t\t\t\t\t\tInvalid Account Number or PIN Code" << endl;
        WaitForEnter();
    }
}

bool ConfirmTransaction() {
    cout << "\n\n\n\n\n\n\n\t\t\t\t\t\t\tConfirm transaction\n";
    cout << "\n\t\t\t\t\t\t\t       Press (Y) YES >\n";
    cout << "\n\t\t\t\t\t\t\t       Press (N) NO >\n\n";
    cout << "\n\t\t\t\t\t\t\t       Enter here: ";
    char choice;
    cin >> choice;
    return (choice == 'Y' || choice == 'y');
}

void Withdraw(int accountNumber, int pinCode, double amount) {
    ATMAccount* account = accountList.Find(accountNumber, pinCode);

    if (account) {
        if (account->balance >= amount && amount <= 20000) {
            if (ConfirmTransaction()) {
                account->balance -= amount;
                cout << GREEN << "\n\n\n\n\n\n\n\t\t\t\t\t\t\tWithdrawal successful. New Balance: " << account->balance << endl;
            } else {
                cout << YELLOW << "\n\n\n\n\n\n\n\t\t\t\t\t\t\tWithdrawal canceled." << endl;
            }
        } else {
            cout << RED << "\n\n\n\n\n\n\n\t\t\t\t\t\t\tInsufficient Balance" << endl;
        }
    } else {
        cout << RED << "\n\n\n\n\n\n\n\t\t\t\t\t\t\tInvalid Account Number or PIN Code" << endl;
    }
    WaitForEnter();
}

void Deposit(int accountNumber, int pinCode, double amount) {
    ATMAccount* account = accountList.Find(accountNumber, pinCode);

    if (account) {
        if (IsValidAmount(amount)) {
            if (ConfirmTransaction()) {
                account->balance += amount;
                cout << GREEN << "\n\n\n\n\n\n\n\t\t\t\t\t\t\tDeposit successful. New Balance: " << account->balance << endl;
                LogTransaction("\n\n\n\n\n\n\n\t\t\t\t\t\t\tDeposit: Account Number - " + to_string(accountNumber) + ", Amount - " + to_string(amount));
            } else {
                cout << YELLOW <<"\n\n\n\n\n\n\n\t\t\t\t\t\t\tDeposit canceled." << endl;
            }
        } else {
            cout << YELLOW <<"\n\n\n\n\n\n\n\t\t\t\t\t\t\tInvalid deposit amount." << endl;
            LogTransaction("\n\n\n\n\n\n\n\t\t\t\t\t\t\tInvalid Deposit: Account Number - " + to_string(accountNumber) + ", Amount - " + to_string(amount));
        }
    } else {
        cout << YELLOW <<"\n\n\n\n\n\n\n\t\t\t\t\t\t\tInvalid Account Number or PIN Code" << endl;
    }
    WaitForEnter();
}

bool LoadAccountsFromFile(const string& fileName);

void FundTransfer(int sourceAccountNumber, int pinCode, int targetAccountNumber, double amount) {

    if (LoadAccountsFromFile(cardFileName)) {
        ATMAccount* sourceAccount = accountList.Find(sourceAccountNumber, pinCode);
        ATMAccount* targetAccount = accountList.FindByAccountNumber(targetAccountNumber);

        if (!sourceAccount) {
            cout << YELLOW <<"\n\n\n\n\n\n\n\t\t\t\t\t\t\tInvalid Source Account Number or PIN Code" << endl;
            WaitForEnter();
            return;
        }

        if (!targetAccount) {
            cout << YELLOW <<"\n\n\n\n\n\n\n\t\t\t\t\t\t\tInvalid Target Account Number" << endl;
            WaitForEnter();
            return;
        }

        if (sourceAccount->balance < amount) {
            cout << RED <<"\n\n\n\n\n\n\n\t\t\t\t\t\t\tInsufficient Balance" << endl;
            WaitForEnter();
            return;
        }

        if (!ConfirmTransaction()) {
            cout << YELLOW <<"\n\n\n\n\n\n\n\t\t\t\t\t\t\tFund transfer canceled." << endl;
            WaitForEnter();
            return;
        }

        sourceAccount->balance -= amount;
        targetAccount->balance += amount;

        SaveAccountsToFile(cardFileName);

        LogTransaction("\n\n\n\n\n\n\n\t\t\t\t\t\t\tFund Transfer: From Account " + to_string(sourceAccount->accountNumber) +
                       " to Account " + to_string(targetAccount->accountNumber) +
                       " Amount: " + to_string(amount));

        cout << GREEN <<"\n\n\n\n\n\n\n\t\t\t\t\t\t\tFund transfer successful. New Balance: " << sourceAccount->balance << endl;
        WaitForEnter();
    } else {
        cout << RED <<"\n\n\n\n\n\n\n\t\t\t\t\t\t\tError loading account data from file." << endl;
        WaitForEnter();
    }
}


bool IsValidPIN(int pin) {
    return (pin >= 1000 && pin <= 9999);
}

void ChangePIN(int accountNumber, int currentPIN, int newPIN) {
    ATMAccount* account = accountList.Find(accountNumber, currentPIN);

    if (account) {
        if (IsValidPIN(newPIN)) {
            account->pinCode = newPIN;
            cout << GREEN <<"\n\n\n\n\n\n\n\t\t\t\t\t\t\tPIN Code changed successfully" << endl;
        } else {
            cout << YELLOW <<"\n\n\n\n\n\n\n\t\t\t\t\t\t\tInvalid new PIN Code" << endl;
        }
    } else {
        cout << YELLOW <<"\n\n\n\n\n\n\n\t\t\t\t\t\t\tInvalid Account Number or PIN Code" << endl;
    }
    WaitForEnter();
}

bool LoadAccountsFromFile(const string& fileName) {
    ifstream file(fileName);

    if (file) {
        int accountNumber, pinCode;
        double balance;
        string accountName, birthday, contactNumber;

        while (file >> accountNumber >> pinCode >> balance >> accountName >> birthday >> contactNumber) {
            ATMAccount* newAccount = new ATMAccount(accountNumber, accountName, birthday, contactNumber, balance, pinCode);
            accountList.Insert(newAccount);
        }

        file.close();
        return true;
    }

    return false;
}


bool ReadATMCardData(int& accountNumber, int& pinCode) {
    string cardFile = "ATM_CARD.txt";
    ifstream cardFileStream(cardFile);
    if (cardFileStream.is_open()) {
        if (cardFileStream >> accountNumber >> pinCode) {
            cardFileStream.close();
            return true;
        } else {
            cerr << RED <<"\n\n\n\n\n\n\n\t\t\t\t\t\t\tFailed to read data from the card file." << endl;
            cardFileStream.close();
            return false;
        }
    } else {
        cerr << RED <<"\n\n\n\n\n\n\n\t\t\t\t\t\t\tFailed to open the card file." << endl;
        return false;
    }
}


void AccountInformation(ATMAccount* account) {
    system("cls");
    cout << CYAN <<"\n\n\t\t\t\t\t\t\t\t===============================" << endl;
    cout << WHITE <<"\n\n\t\t\t\t\t\t\t\t      Account Information" << endl;
    cout << CYAN <<"\n\n\t\t\t\t\t\t\t\t===============================" << endl;
    cout << WHITE <<"\n\t\t\t\t\t\t\t\tAccount Number: " << account->accountNumber << endl;
    cout << WHITE <<"\n\t\t\t\t\t\t\t\tAccount Name: " << account->accountName << endl;
    cout << WHITE <<"\n\t\t\t\t\t\t\t\tDate of Birth: " << account->birthday << endl;
    cout << WHITE <<"\n\t\t\t\t\t\t\t\tContact Number: " << account->contactNumber << endl;
    cout << WHITE <<"\n\t\t\t\t\t\t\t\tAccount Balance: " << account->balance << endl;
    cout << CYAN <<"\t\t\t\t\t\t\t\t===============================" << endl;
    WaitForEnter();
}


void ATMOperations(ATMAccount* verifiedAccount) {
    if (!verifiedAccount) {
        cout << YELLOW <<"\n\n\n\n\n\n\n\t\t\t\t\t\t\tInvalid Account or PIN Code" << endl;
        WaitForEnter();
        return;
    }

    bool exitOperation = false;

    while (!exitOperation) {
        system("cls");
        cout << GREEN <<"\n\n\t\t\t\t\t\t\t\t===============================" << endl;
        cout << WHITE <<"\n\n\t\t\t\t\t\t\t\t         ACCOUNT MENU" << endl;
        cout << GREEN <<"\n\n\t\t\t\t\t\t\t\t===============================" << endl;
        cout << WHITE <<"\n\t\t\t\t\t\t\t\tAccount Owner: " << verifiedAccount->accountName << endl;
        cout << GREEN <<"\n\t\t\t\t\t\t\t\t===============================" << endl;
        cout << WHITE <<"\t\t\t\t\t\t\t\tSelect a Transaction:" << endl;
        cout << WHITE <<"\t\t\t\t\t\t\t\t[1] Check Account Balance" << endl;
        cout << WHITE <<"\t\t\t\t\t\t\t\t[2] Withdraw Money" << endl;
        cout << WHITE <<"\t\t\t\t\t\t\t\t[3] Deposit Money" << endl;
        cout << WHITE <<"\t\t\t\t\t\t\t\t[4] Transfer Funds" << endl;
        cout << WHITE <<"\t\t\t\t\t\t\t\t[5] Change PIN Code" << endl;
        cout << WHITE <<"\t\t\t\t\t\t\t\t[6] Account Information" << endl;
        cout << WHITE <<"\t\t\t\t\t\t\t\t[7] Exit" << endl;
        cout << GREEN <<"\t\t\t\t\t\t\t\t===============================" << endl;

        int choice;
        cout << WHITE <<"\n\t\t\t\t\t\t\t\tEnter here: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                system("cls");
                BalanceInquiry(verifiedAccount->accountNumber, verifiedAccount->pinCode);
                break;
            }
            case 2: {
                system("cls");
                double withdrawalAmount;
                cout << WHITE <<"\n\n\n\n\n\n\n\t\t\t\t\t\t\tEnter Amount to Withdraw: ";
                cin >> withdrawalAmount;
                Withdraw(verifiedAccount->accountNumber, verifiedAccount->pinCode, withdrawalAmount);
                break;
            }
            case 3: {
                system("cls");
                double depositAmount;
                cout << WHITE <<"\n\n\n\n\n\n\n\t\t\t\t\t\t\tEnter Amount to Deposit: ";
                cin >> depositAmount;
                Deposit(verifiedAccount->accountNumber, verifiedAccount->pinCode, depositAmount);
                break;
            }
            case 4: {
                system("cls");
                int targetAccountNumber;
                double transferAmount;
                cout << WHITE <<"\n\n\n\n\n\n\n\t\t\t\t\t\t\tEnter Target Account Number: ";
                cin >> targetAccountNumber;
                cout << WHITE <<"n\t\t\t\t\t\t\tEnter Amount to Transfer: ";
                cin >> transferAmount;
                FundTransfer(verifiedAccount->accountNumber, verifiedAccount->pinCode, targetAccountNumber, transferAmount);
                break;
            }
            case 5: {
                system("cls");
                int newPIN;
                cout << WHITE <<"\n\n\n\n\n\n\n\t\t\t\t\t\t\tEnter New PIN Code: ";
                newPIN = GetPINInput();
                ChangePIN(verifiedAccount->accountNumber, verifiedAccount->pinCode, newPIN);
                break;
            }
            case 6: {
                AccountInformation(verifiedAccount);
                break;
            }
            case 7: {
                system("cls");
                SaveAccountsToFile(cardFileName);
                cout << WHITE <<"\n\n\n\n\n\n\n\t\t\t\t\t\t\tGoodbye, Please come again!" << endl;
                exitOperation = true;
                break;
            }
            default: {
                cout << RED <<"\n\n\n\n\n\n\n\t\t\t\t\t\tInvalid Choice. Please try again." << endl;
                WaitForEnter();
                break;
            }
        }
    }
}


void SaveAccountToDrive(ATMAccount* newAccount, const string& usbDrivePath) {
    string filePath = usbDrivePath + cardFile;
    ofstream cardFile(filePath, ios::app);

    if (cardFile.is_open()) {
        cardFile << newAccount->accountNumber << " " << newAccount->pinCode << endl;
        cardFile.close();
    }
}

void ShowLoadingScreen() {
    int animationLength = 21;
    char* colors[] = {CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN, CYAN,};

    cout << "\n\n\n\n\n\n\n\n\n\n\t\t\t\t\t\t\t\tLoading = ";
    for (int i = 0; i < animationLength; ++i) {
        cout << colors[i % 21];
        cout << char(219);
        cout << RESET;
        _sleep(200);
    }

    cout << "\r";
    system("cls");
    SetBlueBackgroundWhiteText();
    cout << GREEN <<"\n\n\n\t\t\t\t\t\t\tLOADING COMPLETE!" << endl;
}

int main() {
    SetBlueBackgroundWhiteText();
    ShowLoadingScreen();
    ResetConsoleColors();
    srand(static_cast<unsigned>(time(0)));

    if (LoadAccountsFromFile(cardFileName)) {
        while (true) {
            SetBlueBackgroundWhiteText();
            system("cls");
            cout << GREEN <<"\n\n\n\t\t\t\t\t\t\t\t***===============================***" << endl;
            cout << WHITE <<"\n\t\t\t\t\t\t\t\t          WELCOME TO THE ATM" << endl;
            cout << GREEN <<"\n\n\t\t\t\t\t\t\t\t***===============================***" << endl;
            cout << WHITE <<"\n\t\t\t\t\t\t\t\t[1.] Log-In Account" << endl;
            cout << WHITE <<"\n\t\t\t\t\t\t\t\t[2.] Enroll New Account" << endl;
            cout << WHITE <<"\n\t\t\t\t\t\t\t\t[3.] Exit" << endl;
            cout << GREEN <<"\n\t\t\t\t\t\t\t\t***===============================***" << endl;
            int choice;
            cout << WHITE <<"\t\t\t\t\t\t\t\t\tEnter your choice: ";
            cin >> choice;

            switch (choice) {
                 case 1: {
                      system("cls");
                      ifstream file("D:/ATM_CARD.txt");

                     if (file) {
                         cout << WHITE << "\n\n\n\n\t\t\t\t\t\tATM card inserted. You can now enter your Account Number and PIN Code." << endl;
                         file.close();

                         int accountNumber, pinCode;
                         cout << WHITE << "\n\n\n\t\t\t\t\t\t\t\tEnter Account Number: ";
                         cin >> accountNumber;
                         cout << WHITE << "\n\n\n\t\t\t\t\t\t\t\tEnter PIN Code: ";
                         pinCode = GetPINInput();

                        ATMAccount* verifiedAccount = accountList.Find(accountNumber, pinCode);
                        if (verifiedAccount) {
                            ATMOperations(verifiedAccount);
                        } else {
                            cout << YELLOW << "\n\n\n\t\t\t\t\t\t\tInvalid Account Number or PIN Code" << endl;
                            WaitForEnter();
                        }
                     } else {
                         cout << WHITE << "Please insert your ATM card first." << endl;
                         WaitForEnter();
                     }
                     break;
               }
                case 2: {
                    system("cls");
                    ifstream file("D:/ATM_CARD.txt");

                    if (file) {
                        cout << WHITE << "\n\n\n\n\t\t\t\t\t\t\tATM card inserted. You can now enroll a new account." << endl;
                        file.close();
                        ATMAccount* newAccount = EnrollNewAccount();
                        string usbDrivePath = "D:/";
                        SaveAccountToDrive(newAccount, usbDrivePath);
                        WaitForEnter();
                    } else {
                        cout << YELLOW << "\n\n\n\t\t\t\t\t\t\tPlease insert your ATM card first." << endl;
                        WaitForEnter();
                    }
                    break;
                }
                case 3: {
                    system("cls");
                    SaveAccountsToFile(cardFileName);
                    cout << GREEN <<"\n\n\n\n\n\n\n\n\n\n\t\t\t\t\t\t\t\tThank you for using our service!" << endl;
                    return 0;
                }
                default: {
                    cout << YELLOW << "\n\n\n\n\n\t\t\t\t\t\t\t\tInvalid Choice. Please try again." << endl;
                    WaitForEnter();
                    break;
                }
            }
        }
        ResetConsoleColors();
    } else {
        cout << RED << "\n\n\n\n\n\t\t\t\t\t\t\t\tError loading account data from file." << endl;

        return 1;
    }
}
