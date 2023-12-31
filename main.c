#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/*-------------------------------------------------cool-stuff-------------------------------------------------*/
#ifdef _WIN32
    #define CLEAR_COMMAND "cls"
#endif
#ifdef __APPLE__
    #define CLEAR_COMMAND "clear"
#endif
#ifdef __linux__
    #define CLEAR_COMMAND "clear"
#endif

#define LOG(...) do { system(CLEAR_COMMAND); printf(__VA_ARGS__); printf("\n"); } while(0)

/*-------------------------------------------------globals-------------------------------------------------*/
typedef struct
{
    int month;
    int year;
} Date;

typedef struct
{
    char * accountNumber;
    char * name;
    char * mobileNumber;
    char * emailAddress;
    double balance;
    Date openingDate;
} Account;


typedef struct  {
    char * username;
    char * password;
} Credential;

typedef struct{
    Credential *array;
    int number;
}Credentials;
Credentials credentials = {.number = 0};


typedef struct{
    Account *array;
    int number;
}Accounts;

Accounts accounts = {.number = 0};

int ascendingFactor;

#define accountsFilePath  "accounts.txt"
#define credentialsFilePath  "users.txt"
#define transactionsFolderPath  "transactions/"
#define readingFileTip  "check file exists and not used by other process\n"
#define writingFileTip  "check check that you have access to the file\n"

#define maxUsernameSize  51
#define maxPasswordSize 51
#define maxNameSize  256
#define maxAccountNumberSize  11
#define maxEmailSize  321
#define maxMobileSize  12

/*---------------------------------------------------strUtils-------------------------------------------------------*/

char *copyStrOnHeap(char *source){
char *destination = malloc(strlen(source) + 1);
strcpy(destination, source);
return destination;
}

void tolowercase(char *str) {

int i = 0;
while (str[i] != '\0') {
    str[i] = tolower(str[i]);
    i++;
}
}


void removeSpaces(char *inputString) {
int count = 0;
for (int i = 0; inputString[i]; i++) {
    if (inputString[i] != ' ') {
        inputString[count++] = inputString[i];
    }
}
inputString[count] = '\0';
}


/*---------------------------------------------------Validations--------------------------------------------------*/

int isPositiveDouble(void *doublePointer){
double* number = (double *) doublePointer; 
return *number > 0;
}


int isalphabets(char *str){
    for (int i = 0; str[i] != '\0'; i++) {
    if (!isalpha(str[i])) return  0;
}
return 1;
}

int isnum(char *str){
for (int i = 0; str[i] != '\0'; i++) {
    if (!isdigit(str[i])) return 0;
}
return 1;
}

int searchAccNum(char* account_number)
{

if(account_number == NULL)
{
    printf("Invalid account id.");
    return -1;
}

for (int i=0; i< accounts.number; i++)
{
    if (strcmp(account_number,accounts.array[i].accountNumber)==0)
    {
        return i;
    }
}
return -1;
}


int isValidAccNum(char * account_number){
if(isnum(account_number) && searchAccNum(account_number) != -1) return 1;
return 0;
}

int isValidNewAccNum(char * account_number){
if(isnum(account_number) && strlen(account_number) == 10) return 1;
return 0;
}


int isValidUnamePass(char * usernameOrPassword){
    for (int i = 0; usernameOrPassword[i] != '\0'; i++) {
        if(usernameOrPassword[i] == ' ') return 0;
    }
    return 1;
}


int isValidName(char *str){
int i;
int alphaExists = 0;
for (i = 0; str[i] != '\0'; i++) {
    if (!isalpha(str[i]) && str[i] != ' ') {
        return  0;
    }
    if(isalpha(str[i])) alphaExists = 1;
}
if(i==0 || !alphaExists) return 0; // the input is "" or no alphabets exists
return 1;
}

int isMobile(char * mobileNumber){
if(mobileNumber[0] != '0' || mobileNumber[1] != '1' || strlen(mobileNumber) != 11 || !isnum(mobileNumber))
    return 0;
return 1;
}




int isValidEmail(char *email) {
int at_position = -1 , dot_position = -1 , length = strlen(email) , i;
// find the position of @ and the last .
for (i = 0; i < length; i++) {
    if (email[i] == '@') {
        at_position = i;
    } else if (email[i] == '.') {
        dot_position = i;
    }
}
// @ is before .
// check the existance of charchters between @ and .

if(at_position == 0 || dot_position == length - 1) return 0;

if (dot_position - at_position <= 1) {
    return 0;
}
// valid characters before @
for (i = 0; i < at_position; i++) {
    if (!isalnum(email[i]) && email[i] != '.' && email[i] != '_' && email[i] != '-') {
        return 0;
    }
}
// valid characters after @
for (i = at_position + 1; i < dot_position; i++) {
    if (!isalnum(email[i]) && email[i] != '.' && email[i] != '_' && email[i] != '-') {
        return 0;
    }
}
//  valid characters after .
for (i = dot_position + 1; i < length; i++) {
    if (!isalpha(email[i])) {
        return 0;
    }
}
return 1;
}

/*-------------------------------------------------input functions-------------------------------------------------*/

void getnumberTillValid(char* format, void *ptr , char *entry_message ,char* notANumber_error_message ,
                        char *condition_error_message , int (*conditionForPointer)(void *)){
    int inputSuccess;
    while(1){
        printf( "%s" , entry_message);
        inputSuccess = scanf(format , ptr) == 1;
        if(!inputSuccess){
            while(getchar() != '\n');
            printf(notANumber_error_message);
        }
        if(inputSuccess){
            while(getchar() != '\n');
            if(conditionForPointer == NULL) break;
            if(conditionForPointer(ptr)){
                break;
            }
            printf("%s" , condition_error_message);
        }
    }
}

void getIntTillValid(int *ptr , char *entry_message ,char* notANumber_error_message){
    getnumberTillValid("%d" , ptr , entry_message , notANumber_error_message , "" , NULL);
}

void getDoubleTillValid(double *ptr , char *entry_message ,char* notANumber_error_message){
    getnumberTillValid("%lf" , ptr , entry_message , notANumber_error_message , "" , NULL);
}

int check_input_notCropped(char* string , int stringElementsNum , FILE *file ){

    if(string[stringElementsNum -2] == '\n' || string[stringElementsNum -2] == '\0'){
        for(int i = stringElementsNum -2 ; i  >= 0 ; i--){
            if(string[i] == '\n'){
                string[i] = '\0';
            }
        }
        return 1; //input is not too long to fit in the string
    }

    //no return happened then input is (too long) or is (exactly the size of the string)
    char temp;
    if((temp = fgetc(file))=='\n'  || temp == EOF ){
        return 1;
    }
    //otherwise input is too long
    else{
        while((temp = fgetc(file))!='\n' && temp != EOF );
        return 0;
    }
}

char* getlineOnHeapUntil(int stringElementsNum, FILE *file) {
    char *result = malloc(sizeof(char));
    if (result == NULL) {
        return NULL; // memory allocation failed
    }

    int size = 0;
    int capacity = 1;
    int c;

    while ((c = fgetc(file)) != '\n' && c != EOF) {
        result[size] = (char)c;
        size++; // size now is the current size of result

        
        // if stringElementsNum == -1 don't check for the size 
        if (size >= stringElementsNum && stringElementsNum != -1) {
            free(result);
            while ((c = fgetc(file)) != '\n' && c != EOF); // clear the input buffer
            return NULL; // input size exceeds the specified limit
        }

        if (size >= capacity ) {
            // capacity *= 2; not capacity += 1; as it is common strategy in dynamic array implementations 
            // to avoid frequent reallocations and improve performance
            capacity *= 2;
            char *temp = realloc(result, sizeof(char) * capacity);
            if (temp == NULL) {
                free(result);
                return NULL; // memory reallocation failed
            }
            result = temp;
        }
    }

    result[size] = '\0';

    if (size == 0 && c == EOF) {
        free(result);
        return NULL; // Empty line at end of file
    }

    return result;
}


char* getlineOnHeap(FILE *file){
    return getlineOnHeapUntil(-1 , file);
}


char *getOnHeapTillValid(int size , char * entry_message , char * length_error_message ,
                            char *condition_error_message , int (*condition)(char *)){
    char* result;
    do{
        printf(  "%s", entry_message);
        result = getlineOnHeapUntil( size , stdin);
        if(result == NULL){
            printf("%s" , length_error_message);
            continue;
        }

        // string is empty
        if(!strlen(result)) {
            printf("no input entered\n");
            free(result);
            result = NULL;
            continue;
        }

        if(condition == NULL) break;


        // if condition is a function != NULL

        // condition is true
        if(condition(result))break;



        // continues if  condition didn't pass or the string is empty or the string is cropped (result == NULL)
        printf("%s" , condition_error_message);
        result = realloc(result , strlen(result)+9);
        free(result);
        result = NULL;

    }while(result == NULL);
    return result;
}

int isYorN(char * str){
    return !strcmp(str , "y") || !strcmp(str , "n");
}

int binary_prombt(char * message){

    char * prompt_message;
    prompt_message = malloc(strlen(message) + strlen(" (y/n) : ") + 1);
    prompt_message[0] = '\0';
    strcpy(prompt_message , message);
    strcat(prompt_message , " (y/n) : ");
    char *answer;


    answer = getOnHeapTillValid( 2 ,  prompt_message  , "invalid input should be y/n (lower case)\n" , "invalid input should be y/n (lower case)\n"  ,  isYorN);

    free(prompt_message);

    if(strcmp(answer , "n") == 0){
        free(answer);
        return 0;
    }

    free(answer);

    return 1;
}

FILE* openFileTillVaild(const char *filepath , char * mode ,const char *tip){
    FILE *result;
    while( (result = fopen(filepath , mode)) == NULL){
        printf("Error opening file %s\n" , filepath);
        printf("%s" , tip);
        if(!binary_prombt("Do you want to try again ?")) return NULL;
    }
    return result;

}


int getExistingAccountIndex(){
    char * account_number;
    int index;
    while(1){
        account_number = getOnHeapTillValid( maxAccountNumberSize , "please enter account number: " , "account number is very long\n" , "invalid account number (less than 10 or contains non numeric values)\n" , isValidNewAccNum);
        index = searchAccNum(account_number);
        // no free if the sender account is found as it will be compared to the receiver account
        if(index != -1){
            free(account_number);
            return index;
        }
        free(account_number);
        printf("account not found\n");
    }
}


/*--------------------------------------------------Transactions---------------------------------------------------*/
int start_from_line(FILE *readingEnabledFile , int line){
    rewind(readingEnabledFile);
    int i;
    for(i = 1 ; i < line  && !feof(readingEnabledFile); i++){
        while(fgetc(readingEnabledFile)!='\n' && !feof(readingEnabledFile));
    }
    return i==line;
}


char *getTransactionFilePathOnHeap(char *accountNumber) {

    char *filepath = malloc(strlen(transactionsFolderPath) + strlen(accountNumber) + strlen(".txt") + 1);

    if (filepath != NULL) {
        strcpy(filepath, transactionsFolderPath);
        strcat(filepath, accountNumber);
        strcat(filepath, ".txt");
    }
    return filepath;
}


int deleteTransactionsFile(char * accountNumber){

    char *transactionsFilepath = getTransactionFilePathOnHeap(accountNumber);

    if(transactionsFilepath == NULL) {
        printf("no enough memory to allocate for transaction file path");
        return 0;
    }

    if( remove(transactionsFilepath) != 0 ){
        printf("error removing transactions file %s" , transactionsFilepath);
    }

    free(transactionsFilepath);

    return 1;
}

int appendTransaction(int index , char * transactionType ,  double amount ){

char *transaction_path = getTransactionFilePathOnHeap(accounts.array[index].accountNumber);

if(transaction_path == NULL) {
    printf("no enough memory to allocate for transaction file path");
    return 0;
}

FILE *transactionsFile = openFileTillVaild(transaction_path  , "a" , writingFileTip);


if(transactionsFile == NULL){
    printf("Error opening %s\n" , transaction_path);
    free(transaction_path);
    return 0;
}

free(transaction_path);

fprintf(transactionsFile , "%s %.2lf\n" , transactionType , amount);


fclose(transactionsFile);
return 1;

}

int makeTransactionFile(int index){
    char *transactionFilePath  = getTransactionFilePathOnHeap(accounts.array[index].accountNumber);

    if(transactionFilePath == NULL) {
        printf("no enough memory to allocate for transaction file path");
        return 0;
    }

    FILE *transactionFile = openFileTillVaild(transactionFilePath , "wx" , "new transactions file cannot be created(file already exists or permission denied)\n");
    if(transactionFile == NULL) {
        return 0;
    }
    return 1;
}




/*------------------------------------------------------load-------------------------------------------------------*/

int getReturnsNumber(FILE *file){
    rewind(file);
    int result;
    char temp;
    for(result = 0 ; !feof(file) ; result++){
        while((temp = fgetc(file)) != '\n' && temp != EOF);

    }
    // result = number of \n and EOF
    return result-1;
}

void load_credentials(){
    FILE *credentialsfile;
    char *templine;

    credentialsfile = openFileTillVaild(credentialsFilePath , "r" , readingFileTip);
    if(credentialsfile == NULL) exit(-1);

    credentials.number = getReturnsNumber(credentialsfile);


    // validating format


    // check return '\n' is at the end of the file
    fseek(credentialsfile , -1 , SEEK_END);

    if(fgetc(credentialsfile)!='\n'){
        printf("Error %s format is improper (check the file ends with a return ('\n'))" , credentialsFilePath);
        exit(-1);
    }

    rewind(credentialsfile);

    // every line must consists of two parts uname and password
    for(int i = 0 ; i < credentials.number ; i++){
        templine = getlineOnHeap(credentialsfile);
        if(strtok(templine , " ") != NULL && strtok(NULL , " ") != NULL) continue;
        printf("Error %s format is improper check the file exists in the format of \"username password\" pairs" , credentialsFilePath);
        exit(-1);
        free(templine);
    }


    credentials.array = malloc(sizeof(Credential) * credentials.number);

    // to start reading from top again
    fseek(credentialsfile , 0 , SEEK_SET);

    for(int i = 0 ; i < credentials.number ; i++){
        // templine wasn't freed because it would remove data pointed to by the credential string members
        templine = getlineOnHeap(credentialsfile);
        credentials.array[i].username = copyStrOnHeap(strtok(templine , " "));
        credentials.array[i].password = copyStrOnHeap(strtok(NULL , " "));
        free(templine);
    }


    fclose(credentialsfile);
}

void load (){
    FILE *accountsfile , *tempfile;
    char *templine , *tempfilepath ;

    accountsfile = openFileTillVaild(accountsFilePath ,  "r" , readingFileTip);


    if(accountsfile == NULL) exit(-1);

    accounts.number = getReturnsNumber(accountsfile);


    // validation of format xxx,xxx,xxx,...\n

    // check return '\n' is at the end of the file
    fseek(accountsfile , -1 , SEEK_END);

    if(fgetc(accountsfile)!='\n'){
        printf("Error %s format is improper (check the file ends with a return ('\n'))" , accountsFilePath);
        exit(-1);
    }

    // to start reading from top again
    fseek(accountsfile , 0 , SEEK_SET);

    for(int i = 0 ; i < accounts.number ; i++){
        double tempDouble;
        int tempInt;
        templine = getlineOnHeap(accountsfile);
        if(strtok(templine , ",") == NULL){
            printf("Error %s format is improper check the file exists in the format of \n" 
            "\"account_number,name,emailAddress,balance,mobile_number,date_opened\" comma separated values" , accountsFilePath);
            exit(-1);
        }
        // 6 is the number of members
        for(int s = 0 ; s < 6 ; s++){
            char *str;

            // members separated by ,
            if(s < 4 ) str = strtok(NULL , ",");
            // members separated by -
            else str = strtok(NULL , "-");

            if(str == NULL){
                printf("Error %s format is improper" , accountsFilePath);
                exit(-1);
            }

            // if s corresponds to float data
            if(s == 2){
                if(sscanf(str , "%lf" , &tempDouble))continue;
                printf("Error %s format is improper" , accountsFilePath);
                exit(-1);
            }

            // if s corresponds to integer data
            if( s == 4 || s == 5){
                if(sscanf(str , "%d" , &tempInt))continue;
                printf("Error %s format is improper" , accountsFilePath);
                exit(-1);
            }
        }
        free(templine);
    }

    accounts.array = malloc(sizeof(Account) * accounts.number);

    // to start reading from top again
    fseek(accountsfile , 0 , SEEK_SET);

    // scanf will crash if no , delimeter (file is edited)
    for(int i = 0 ; i < accounts.number ; i++){
        // templine wasn't freed because it would remove data pointed to by the account string members
        templine = getlineOnHeap(accountsfile);
        accounts.array[i].accountNumber = copyStrOnHeap(strtok(templine , ","));
        accounts.array[i].name = copyStrOnHeap(strtok(NULL , ","));
        accounts.array[i].emailAddress = copyStrOnHeap(strtok(NULL , ","));
        sscanf(strtok(NULL , ",") , "%lf" , &accounts.array[i].balance);
        accounts.array[i].mobileNumber =  copyStrOnHeap(strtok(NULL , ","));
        sscanf(strtok(NULL , "-") , "%d" , &accounts.array[i].openingDate.month);
        sscanf(strtok(NULL , "-") , "%d" , &accounts.array[i].openingDate.year);
        free(templine);

        tempfilepath = getTransactionFilePathOnHeap(accounts.array[i].accountNumber);
        if(tempfilepath == NULL) {
            printf("no enough memory to allocate for transaction file path");
            exit(-1);
        }
        if((tempfile = openFileTillVaild(tempfilepath , "r" , readingFileTip)) == NULL ) exit(-1);
        fclose(tempfile);
        free(tempfilepath);
    }



    fclose(accountsfile);
}


/*--------------------------------printingAllAcounts---------------------------------------------*/

const char *monthname[]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

void printAccount(int account_index){


    printf("Account Number : %s\n", accounts.array[account_index].accountNumber);
    printf("Name: %s\n", accounts.array[account_index].name);
    printf("E-mail: %s\n", accounts.array[account_index].emailAddress);
    printf("Balance: %.2f $\n", accounts.array[account_index].balance);
    printf("Mobile: %s\n", accounts.array[account_index].mobileNumber);
    printf("Opened: %s %d\n", monthname[accounts.array[account_index].openingDate.month-1], accounts.array[account_index].openingDate.year);


}


// Comparison functions for qsort
int compareByName(const void *a, const void *b) {
    Account *accountA = (Account *)a;
    Account *accountB = (Account *)b;
    // not dynamically allocated as the values are important when the function returns
    char name1[maxNameSize], name2[maxNameSize];
    strcpy(name1, accountA->name);
    strcpy(name2, accountB->name);
    tolowercase(name1);
    tolowercase(name2);
    return ascendingFactor * strcmp(name1, name2);
}

int compareByDate(const void *a, const void *b) {
    Account *accountA = (Account *)a;
    Account *accountB = (Account *)b;
    Date date1 = accountA->openingDate;
    Date date2 = accountB->openingDate;
    if (date1.year != date2.year) {
        return date1.year - date2.year;
    } else {
        return ascendingFactor * (date1.month - date2.month);
    }
}

int compareByBalance(const void *a, const void *b) {
    Account *accountA = (Account *)a;
    Account *accountB = (Account *)b;
    // it is not accountA->balance - accountB->balance 
    // as balance is double and may cause int to become -ve and therefore introduces a logical error
    return ascendingFactor * ((accountA->balance > accountB->balance) - (accountA->balance < accountB->balance));
}

// Sorting functions using qsort
void sortByName() {
    qsort(accounts.array, accounts.number, sizeof(Account), compareByName);
}

void sortByDate() {
    qsort(accounts.array, accounts.number, sizeof(Account), compareByDate);
}

void sortByBalance() {
    qsort(accounts.array, accounts.number, sizeof(Account), compareByBalance);
}


void printAllAccounts(){
    LOG("\n");

    if(accounts.number == 0){
        LOG("No accounts to print\n");
        return;
    }


    int selection;
    printf("1- by name\n");
    printf("2- by date\n");
    printf("3- by balance\n");

    do{
        getIntTillValid(&selection , "choose how to sort the accounts :" , "enter a valid choice\n");
    }while(selection > 3 || selection < 1);

    // ascendingFactor is multiplied by the output in each comparison function
    // if ascendingFactor = 1 then the sorting is ascending
    // if -1 descending
    ascendingFactor = -1;
    if(binary_prombt("do you want the sorting to be ascending ?")) ascendingFactor = 1;


    switch (selection)
    {
        case 1 :
            sortByName();
            break;
        case 2 :
            sortByDate();
            break;
        case 3 :
            sortByBalance();
            break;

    }


    printf("------------------------------\n");
    for(int i = 0 ; i<accounts.number ; i++ ){
        printAccount(i);
        printf("------------------------------\n");
    }

}

/*----------------------------------------save----------------------------------------------------*/

int isSaveWanted(){
    return binary_prombt("Save changes?");
}



int save(){


    //files checking
    FILE *accountsfile = openFileTillVaild(accountsFilePath , "w" , writingFileTip);
    if(accountsfile == NULL){
        return 0;
    }

    for (int i = 0 ; i < accounts.number; i++){
        Account temp = accounts.array[i];
        fprintf( accountsfile , "%s,%s,%s,%.2lf,%s,%d-%d\n" , temp.accountNumber , temp.name , temp.emailAddress , temp.balance , temp.mobileNumber , temp.openingDate.month , temp.openingDate.year);
    }

    fclose(accountsfile);

    return 1 ;
}



/*---------------------------------------delete---------------------------------------------------*/

void delete(){

    LOG("\n");

    int index = getExistingAccountIndex();


    if(accounts.array[index].balance!=0){
        LOG("can't delete this account (balance is not zero)\n");
        return;
    }

    // account number to be deleted is copied as it will be passed to deleteTransactionsFile function
    char *account_number = copyStrOnHeap(accounts.array[index].accountNumber);

    free(accounts.array[index].accountNumber);
    free(accounts.array[index].emailAddress);
    free(accounts.array[index].mobileNumber);
    free(accounts.array[index].name);


    // removing from the array
    for (int j = index ; j < accounts.number - 1 ; j++){
        accounts.array[j] = accounts.array[j+1];
    }
    accounts.number--;

    if(!isSaveWanted() || !save() || !deleteTransactionsFile(account_number)) {
        free(account_number);
        load();
        return;
    }

    free(account_number);

    LOG("Account Deleted Successfully\n");
    return;
}

/*--------------------------------------login-modifyAccount-transfer------------------------------------------------------*/

// Function to validate user credentials
int validateUser(char *username, char *password) {
    for(int i = 0 ; i < credentials.number ; i++ ){
        if(strcmp(credentials.array[i].username , username) == 0 && strcmp(credentials.array[i].password , password) == 0){
            return 1;
        }
    }
    return 0;
}


void login(){

    while(1){
        char * username = getOnHeapTillValid(maxUsernameSize , "Enter username: " , "too long username\n"  , "invalid username (space(s) detected)\n" , isValidUnamePass );
        char * password = getOnHeapTillValid(maxPasswordSize , "Enter password: " , "too long password\n"  , "invalid password (space(s) detected)\n" , isValidUnamePass );
        if(validateUser(username , password)){
            free(password);
            free(username);
            break;
        }
        printf("wrong username or password\n");
        free(password);
        free(username);

    }

}


// Function to modify account details
void modifyAccount() {
    LOG("\n");

    int index = getExistingAccountIndex();

    char *tempName = copyStrOnHeap(accounts.array[index].name), *tempEmail = copyStrOnHeap(accounts.array[index].emailAddress), *tempMobile = copyStrOnHeap(accounts.array[index].mobileNumber);

    // no more use of them (will be modified)
    free(accounts.array[index].name);free(accounts.array[index].emailAddress);free(accounts.array[index].mobileNumber);

    accounts.array[index].name = getOnHeapTillValid( maxNameSize , "Enter new name: " , "name is very long\n" , "invalid name\n" , isValidName );
    accounts.array[index].emailAddress = getOnHeapTillValid( maxEmailSize , "Enter new email: " , "email is very long\n" , "invalid email\n" , isValidEmail );
    accounts.array[index].mobileNumber = getOnHeapTillValid( maxMobileSize , "Enter new mobile number: " , "mobile number is very long\n" , "invalid mobile number\n" , isMobile);

        // save is after save_prompt to avoid saving if user didn't want to save (using short circuit evaluation)
        if(!isSaveWanted() || !save()){
            // no use of modification strings
            free(accounts.array[index].name);free(accounts.array[index].emailAddress);free(accounts.array[index].mobileNumber);

            accounts.array[index].name = tempName;
            accounts.array[index].emailAddress = tempEmail;
            accounts.array[index].mobileNumber = tempMobile;
            return;
        }

        LOG("Account details updated successfully.\n");
        return;
}

// Function to transfer money between accounts
void transfer() {
    LOG("\n");
    int senderIndex , receiverIndex;
    printf("Sender:-\n");
    senderIndex = getExistingAccountIndex();
    printf("Reciever:-\n");
    receiverIndex =  getExistingAccountIndex();

    double amount;

    if(accounts.array[senderIndex].balance == 0){
        LOG("Insufficient funds to withdraw you balance is 0.00");
        return;
    }

    do{
        getnumberTillValid( "%lf" , &amount , "enter amount to be transferred: " , "invalid input (not a number)\n" , "invalid input (negative number)\n" , isPositiveDouble);
        if(amount > 10000)
        {
            printf("The transfer limit is 10000$\n");
            continue;
        }
        if(amount > accounts.array[senderIndex].balance){
            printf("Insufficient balance in sender account.\n");
            continue;
        }
        break;
    }while(1);



    accounts.array[senderIndex].balance -= amount;
    accounts.array[receiverIndex].balance += amount;

    if(!isSaveWanted() || !save() || !appendTransaction(senderIndex , "sending" , amount) || !appendTransaction(receiverIndex , "receiving" , amount)){
        accounts.array[senderIndex].balance += amount;
        accounts.array[receiverIndex].balance -= amount;
        LOG("transfer not saved\n");
    }
}

/*---------------------------------------query-advancedSearch-addaccount----------------------------------------------------*/

void query (){
    LOG("\n");
    char *account_number;

    account_number = getOnHeapTillValid(maxAccountNumberSize , "please enter account number: " , "account number is very long\n" , "invalid account number\n" ,  isnum );

        for (int i=0; i< accounts.number; i++)
        {
            if (strcmp(account_number,accounts.array[i].accountNumber)==0)
            {
                free(account_number);
                printAccount(i);
                return;
            }
        }
        LOG("Account number not found");
        free(account_number);
        return;
}

void advancedsearch(){
    LOG("\n");
    int found = 0;
    char *keyword, *name = malloc(maxNameSize * sizeof(char));
    keyword = getOnHeapTillValid(maxNameSize , "Enter keyword: " , "keyword is very long\n" , "keyword is not a name\n" , isValidName);

    printf("------------------------------\n");
    for (int i=0; i< accounts.number; i++){
        sprintf(name , "%s" , accounts.array[i].name);
        tolowercase(name);
        tolowercase(keyword);
        removeSpaces(keyword);
        if (strstr( name , keyword)){
            found = 1;
            printAccount(i);
            printf("------------------------------\n");
        }
    }

    if(!found)
        LOG("No matches are found");

    free(name);
    free(keyword);
}

void addaccount(){
    LOG("\n");
    char *name, *email , *account_number, *mobile_number;
    double balance;

    name = getOnHeapTillValid(maxNameSize , "Account Holder name: " , "name is very long\n" , "Invalid name has (contains non alphabetic characters or starts with space)\n" , isValidName);

    email = getOnHeapTillValid( maxEmailSize , "Email: " , "email is very long\n" , "email is not valid\n" , isValidEmail);


    account_number = getOnHeapTillValid( maxAccountNumberSize , "please enter account number: " , "account number is very long\n" , "invalid account number (less than 10 or contains non numeric values)\n" , isValidNewAccNum);


    do{

        int i;
        for (i = 0; i < accounts.number && strcmp(account_number, accounts.array[i].accountNumber) != 0; i++);

        if(i==accounts.number) break;

        free(account_number);
        printf("account number already exists\n");
        account_number = getOnHeapTillValid( maxAccountNumberSize , "please enter account number: " , "account number is very long\n" , "Please enter a valid account number\n" , isnum);

    }while(1);

    do{
        getnumberTillValid( "%lf" , &balance , "balance: " , "invalid amount (not a number)\n" ,  "invalid amount (negative number)\n" , isPositiveDouble);
        if(balance > 10000)
        {
            printf("The deposit limit is 10000$\n");
            continue;
        }
        break;
    }while(1);

    mobile_number =  getOnHeapTillValid( maxMobileSize , "Mobile number: " , "Please enter a valid 11-digit mobile number\n" , "Please enter a valid 11-digit mobile number\n" , isMobile);

    accounts.array = realloc( accounts.array , (accounts.number + 1) * sizeof(Account));
    time_t timeSecSince1970 = time(NULL);
    struct tm *current_time = localtime(&timeSecSince1970);

    accounts.array[accounts.number].name = name;
    accounts.array[accounts.number].mobileNumber = mobile_number;
    accounts.array[accounts.number].emailAddress = email;
    accounts.array[accounts.number].accountNumber = account_number;
    accounts.array[accounts.number].balance = balance;
    accounts.array[accounts.number].openingDate.year = (current_time->tm_year +1900);
    accounts.array[accounts.number].openingDate.month = (current_time->tm_mon +1);
    accounts.number++;
    if( !isSaveWanted() || !save() || !makeTransactionFile(accounts.number-1)){
        accounts.number--;
        free(accounts.array[accounts.number].name);
        free(accounts.array[accounts.number].mobileNumber);
        free(accounts.array[accounts.number].accountNumber);
        free(accounts.array[accounts.number].emailAddress);
    }

    LOG("account was added successfully");
}



/*--------------------------------------startup-withdraw-deposit-report----------------------------------------------------*/

void quit()
{
    LOG("Thank you for choosing our services");
    exit(0);
}

void withdraw()
{

    LOG("\n");
    int index = getExistingAccountIndex();

    double amountDrawn;
    if(accounts.array[index].balance == 0){
        LOG("Insufficient funds to withdraw you balance is 0.00");
        return;
    }

    do{
        getnumberTillValid( "%lf" , &amountDrawn , "How much to withdraw: " , "invalid amount (not a number)\n" ,  "invalid amount (negative number)\n" , isPositiveDouble);
        if(amountDrawn > 10000)
        {
            printf("The withdraw limit is 10000$\n");
            continue;
        }
        if(amountDrawn > accounts.array[index].balance){
            printf("Insufficient balance in account.\n");
            continue;
        }
        break;
    }while(1);

    accounts.array[index].balance -= amountDrawn;

    if(!isSaveWanted() || !save() || !appendTransaction(index , "withdrawing" , amountDrawn)){
        accounts.array[index].balance += amountDrawn;
        LOG("withdrawing not saved\n");
    }


}

void deposit()
{
    LOG("\n");

    int Index;

    Index = getExistingAccountIndex();

    double amountDeposited ;

    do{
        getnumberTillValid( "%lf" , &amountDeposited , "How much to deposit: " , "invalid amount (not a number)\n" ,  "invalid amount (negative number)\n" , isPositiveDouble);
        if(amountDeposited > 10000)
        {
            printf("The deposit limit is 10000$\n");
            continue;
        }
        break;
    }while(1);

    accounts.array[Index].balance += amountDeposited;


    if(!isSaveWanted() || !save() || !appendTransaction(Index , "depositing" , amountDeposited)){
        accounts.array[Index].balance -= amountDeposited;
        LOG("depositing not saved\n");
    }

}

void report()
{

    LOG("\n");
    int index = getExistingAccountIndex();

    char* filePath = getTransactionFilePathOnHeap(accounts.array[index].accountNumber);

    if(filePath == NULL) {
        printf("no enough memory to allocate for transaction file path");
        return;
    }

    FILE* transactionFile = openFileTillVaild(filePath,"r" , readingFileTip);

    if(transactionFile == NULL){
        free(filePath);
        return;
    }

    free(filePath);


    int lines_num = getReturnsNumber(transactionFile);

    if(!lines_num){
        LOG("no transactions to be printed\n");
        return;
    }

    //if lines num = 6 reading starts from line 2
    start_from_line( transactionFile , lines_num  - 4);

    printf("\n-----------------------------------\n");

    char tempchar;
    while((tempchar = fgetc(transactionFile)) != EOF)
        printf("%c",tempchar);

    printf("-----------------------------------\n"); // \r to remove the effect of EOF

    fclose(transactionFile);

}



void uiStartup()
{
    int option;
    int startup = 1;
    while(startup){

         printf("1-LOGIN\n2-QUIT\n");

        do{
            getnumberTillValid( "%d" ,  &option, "Enter option: ", "Invalid option (non numeric values aren't allowed)\n" , "" , NULL);
            if(option >= 1 && option <= 2 )break;
            printf("Invalid option (number must be 1 or 2)\n");
        }while(1);

        switch(option)
        {
            case 1:
                load_credentials();
                login();
                load();
                LOG("\n");
                startup = 0;
                break;
            case 2:
                quit();
                break;
        }
    }
}

void ui()
{
    uiStartup();
    int option;
    while(1)
    {

        // system("cls");
        printf("1-ADD\n2-DELETE\n3-MODIFY\n4-SEARCH\n5-ADVANCED SEARCH\n6-WITHDRAW\n7-DEPOSIT\n8-TRANSFER\n9-REPORT\n10-PRINT\n11-QUIT\n");

        do{
            getnumberTillValid( "%d" ,  &option, "Enter option: ", "Invalid option (non numeric values aren't allowed)\n" , "" , NULL);
            if(option >= 1 && option <= 11 )break;
            printf("Invalid option (number must be from 1 to 11)\n");
        }while(1);
        switch(option)
        {
            case 1:addaccount();break;
            case 2:delete();break;///////////////////
            case 3:modifyAccount();break;//causes a system error don't know why probably changing the name will fix it
            case 4:query();break;
            case 5:advancedsearch();break;
            case 6:withdraw();break;
            case 7:deposit();break;
            case 8:transfer();break;
            case 9:report();break;
            case 10:printAllAccounts();break;//////////////////////////
            case 11:quit();break;
        }
    }
}


/*----------------------------------------main----------------------------------------------------*/



int main(){
    ui();
}
