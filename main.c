#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>


/*-------------------------------------------------globals-------------------------------------------------*/
typedef enum{
    FALSE,
    TRUE,
    ERROR,
    SUCCESS
} error_message ;


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
    int addedAccountsStartingIndex;
    int addedAccountsNum;
}Accounts;

Accounts accounts = {.addedAccountsNum = 0 , .number = 0};


const char accountsFilePath [] = "../../accounts.txt";
const char credentialsFilePath [] = "../../users.txt";
const char transactionsFolderPath [] = "../../transactions/";
const char tempFilePath [] = "../../temp.txt";
const char filesRelativePath[] = "../../";
const char readingFileTip [] = "check file exists and not used by other process\n";
const char writingFileTip [] = "check check that you have access to the file\n";

int maxUsernameSize = 51;
int maxPasswordSize = 51;
int maxNameSize = 256;
int maxAccountNumberSize = 21;
int maxEmailSize = 321;
int maxMobileSize = 12;

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

int isPositiveDouble(double *num){
return *num > 0;
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


char* getlineOnHeapUntil(int stringElementsNum , FILE *file ){
    char temp;
    char *result = malloc(sizeof(char));
    
    if((temp = fgetc(file)) == '\n' || temp == EOF){
        result[0] = '\0';
        return result;
    }

    
    result[0] = temp;

    
    int size , skipBoundaryChecking = 0 , cropped;
    
    if(stringElementsNum == -1){
        skipBoundaryChecking = 1;
    }

    #define CROPPED (temp = fgetc(file)) != '\n' && temp != EOF )

    // if no '\n' or EOF is found then the input is cropped
    // CROPPED must come first to avoid short circuit evaluation of &&
    for( 
        size =2 ; 
        ( cropped = CROPPED && (skipBoundaryChecking || size  < stringElementsNum) ;
        size++
    ){
        result = realloc(result , sizeof(char) * size);
        result[size-1] = temp;
    }

    result[size - 1] = '\0';

    if(cropped){
        free(result);
        while((temp = (fgetc(file))) != '\n' && temp != EOF); // clear the input buffer
        return NULL;
    }

    return result;

}

char* getlineOnHeap(FILE *file){
    return getlineOnHeapUntil(-1 , file);
}


// char *str = malloc(8);
//  char str[];

int finput(char* preAllocatedString , int stringElementsNum, FILE *file){
    // clear the string from any old value or garbage 
    for(int i = 0; i < stringElementsNum ; i++){
        preAllocatedString[i] = '\0';
    }

    fgets(preAllocatedString , stringElementsNum , file);

    return check_input_notCropped(preAllocatedString , stringElementsNum , file);

}

int input(char* preAllocatedString , int stringSize){
    return finput(preAllocatedString , stringSize , stdin);
}


void inputTillValid(char *preAllocatedStr , int size , char * entry_message , char * length_error_message ,
                    char *condition_error_message , int (*condition)(char *)){
    int inputSuccess;
    do{
        printf( "%s" , entry_message);
        inputSuccess = finput(preAllocatedStr,size,stdin);
        inputSuccess? :printf("%s" , length_error_message);
        if (condition == NULL) continue;
        if(!condition(preAllocatedStr)){
            printf("%s" , condition_error_message);
            inputSuccess = 0;
        }
    }while(!inputSuccess ||  !strlen(preAllocatedStr));   
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
        
        if(!strlen(result)) {
            free(result);
            continue;
        }
        
        if(condition == NULL) continue;
        
        // if condition is a function != NULL
        if(!condition(result)){
            printf("%s" , condition_error_message);
            free(result);
            result = NULL;
        }
    // continues if  condition didn't pass or the string is empty or the string is cropped (result == NULL)
    }while(result == NULL || !strlen(result)); 
    return result;
}

int binary_prombt(char * message){
    char answer [2]; 
    int inputSuccess; 
    do{
        printf("%s (y/n) : " , message);
        inputSuccess = finput(answer , 2, stdin);// to not continue if user entered "yay" for example
    }while( !inputSuccess || ( strcmp(answer , "y") != 0 && strcmp(answer , "n") != 0 ) );

    if(strcmp(answer , "n") == 0){
        return 0;
    }

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

/*--------------------------------------------------Transactions---------------------------------------------------*/
void start_from_line(FILE *readingEnabledFile , int line){
rewind(readingEnabledFile);
for(int i = 1 ; i < line  && !feof(readingEnabledFile); i++){
    while(fgetc(readingEnabledFile)!='\n' && !feof(readingEnabledFile));
}
}


char *getTransactionFilePathOnHeap(char *accountNumber){
char *filepath = calloc(strlen(transactionsFolderPath)+strlen(accountNumber) + strlen(".txt") + 1 , sizeof(char));
strcat(strcat(strcat(filepath , transactionsFolderPath) , accountNumber) , ".txt");
return filepath;
}


int appendTransaction(char* accountNumber , char * transactionType ,  double amount ){

char *transaction_path = getTransactionFilePathOnHeap(accountNumber);
FILE *transactionsFile = openFileTillVaild(transaction_path  , "a" , writingFileTip);
free(transaction_path);


if(transactionsFile == NULL){
    return 0;
}

fprintf(transactionsFile , "%s %.lf\n" , transactionType , amount);


fclose(transactionsFile);
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

void load (){
    FILE *accountsfile , *credentialsfile , *tempfile;
    char *templine , *tempfilepath ;

    accountsfile = openFileTillVaild(accountsFilePath ,  "r" , readingFileTip);
    credentialsfile = openFileTillVaild(credentialsFilePath , "r" , readingFileTip);
    
    if(credentialsfile == NULL || accountsfile == NULL) exit(-1);
    
    // addition to be tested
    accounts.number = getReturnsNumber(accountsfile);
    


    // addition to be tested
    credentials.number = getReturnsNumber(credentialsfile);


    accounts.array = malloc(sizeof(Account) * accounts.number);

    credentials.array = malloc(sizeof(Credential) * credentials.number);


    // to start reading from top again
    fseek(accountsfile , 0 , SEEK_SET);
    fseek(credentialsfile , 0 , SEEK_SET);
    
    // scanf will crash if no , delimeter (file is edited)
    for(int i = 0 ; i < accounts.number ; i++){
        // templine wasn't freed because it would remove data pointed to by the account string members
        templine = getlineOnHeap(accountsfile);
        accounts.array[i].accountNumber = copyStrOnHeap(strtok(templine , ","));
        accounts.array[i].name = copyStrOnHeap(strtok(NULL , ","));
        accounts.array[i].emailAddress = copyStrOnHeap(strtok(NULL , ","));
        sscanf(strtok(NULL , ",") , "%lf" , &accounts.array[i].balance);
        accounts.array[i].mobileNumber =  copyStrOnHeap(strtok(NULL , ","));
        sscanf(strtok(NULL , ",") , "%d" , &accounts.array[i].openingDate.month);
        sscanf(strtok(NULL , ",") , "%d" , &accounts.array[i].openingDate.year);
        free(templine);

        tempfilepath = getTransactionFilePathOnHeap(accounts.array[i].accountNumber);
        if((tempfile = openFileTillVaild(tempfilepath , "r" , readingFileTip)) == NULL ) exit(-1); 
        fclose(tempfile);
        free(tempfilepath);
    }

    for(int i = 0 ; i < credentials.number ; i++){
        // templine wasn't freed because it would remove data pointed to by the credential string members
        templine = getlineOnHeap(credentialsfile);
        credentials.array[i].username = copyStrOnHeap(strtok(templine , " "));
        credentials.array[i].password = copyStrOnHeap(strtok(NULL , " "));
        free(templine);
    }
    


    fclose(accountsfile);
    fclose(credentialsfile);
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


void swap(Account *a , Account *b){
    // multiplication method will not work when any number is zero so we can use
    // the xor method
    // the addition and subtraction
    
    //if a and b have the same location then do nothing 
    if (a == b){
        return; 
    }
    Account temp = *a;
    *a = *b;
    *b = temp; 
}

// void sortbyname(){}

void sortbyname(){

    for(int i = 0 ; i<accounts.number-1 ; i++ ){
        int moved = 0;
        for(int j = 0; j < accounts.number - i - 1 ; j++){
            char* name1 = copyStrOnHeap(accounts.array[j].name);    
            char* name2 = copyStrOnHeap(accounts.array[j+1].name);  
            // lowering the case of the names to compare them correctly without bias to names with lowered alphabets (have higher ascii value)
            tolowercase(name1);
            tolowercase(name2);
            if(strcmp(name1 , name2) == 1){
                swap(&accounts.array[j] , &accounts.array[j+1]);
                moved = 1;
            }
            free(name1);
            free(name2);
        }

        if(!moved) return;
    }

}


void sortbydate(){
    for(int i = 0 ; i<accounts.number-1 ; i++ ){
        int moved = 0;
        for(int j = 0; j < accounts.number - i - 1 ; j++){
            Date date1 = accounts.array[j].openingDate;
            Date date2 = accounts.array[j+1].openingDate;

            if(date1.month*0.01 + date1.year > date2.month*0.01 + date2.year){
                swap(&accounts.array[j] , &accounts.array[j+1]);
                moved = 1;
            }
        }

        if(!moved) return;
    }
}

void sortbybalance(){
    for(int i = 0 ; i<accounts.number-1 ; i++ ){
        int moved = 0;
        for(int j = 0; j < accounts.number - i - 1 ; j++){
            if( accounts.array[j].balance > accounts.array[j+1].balance ){
                swap(&accounts.array[j] , &accounts.array[j+1]);
                moved = 1;
            }
        }

        if(!moved) return;
    }
}



void printAllAccounts(){

    if(accounts.number == 0){
        printf("No accounts to print\n");
        return;
    }

    // styled menu preferred
    int selection;
    printf("1- by name\n");
    printf("2- by date\n");
    printf("3- by balance\n");

    // endmenu
    do{
        getIntTillValid(&selection , "choose how to sort the accounts :" , "enter a valid choice\n");
    }while(selection > 3 || selection < 1);

    switch (selection)
    {
        case 1 :
            sortbyname();
            break;
        case 2 :
            sortbydate();
            break;
        case 3 :
            sortbybalance();
            break;
    
    }
    
    printf("\n------------------------------\n");
    for(int i = 0 ; i<accounts.number ; i++ ){
        printAccount(i);
        printf("------------------------------\n");
    }
    printf("\n");

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
        printf("%s,%s,%s,%.2lf,%s,%d-%d\n" , temp.accountNumber , temp.name , temp.emailAddress , temp.balance , temp.mobileNumber , temp.openingDate.month , temp.openingDate.year); 
    }

    fclose(accountsfile);
    
    return 1 ;
}


/*---------------------------------------delete---------------------------------------------------*/

void delete(){
char*account_number = getOnHeapTillValid(maxAccountNumberSize , "Enter Account Number: " , "I nvalid Account Number (too long)\n" , "Invalid Account Number (contain non-digit characters)\n" ,isnum);


for (int i = 0 ; i <accounts.number ; i++ ){
    if(strcmp(account_number , accounts.array[i].accountNumber) == 0){
        
        //no more use of it 
        free(account_number);

        if(accounts.array[i].balance!=0){
            printf("can't delete this account (balance is not zero)\n");
            return;
        }
        
        free(accounts.array[i].accountNumber);
        free(accounts.array[i].emailAddress);
        free(accounts.array[i].mobileNumber);
        free(accounts.array[i].name);
        
        // removing from the array
        for (int j = i ; j < accounts.number - 1 ; j++){
            accounts.array[j] = accounts.array[j+1];
        }
        accounts.number--;


        printAllAccounts();

        if(!isSaveWanted() || !save()) {
            load();
            return;
        }

        printf("Account Deleted Successfully\n");
        return;
    }
}
free(account_number);
printf("Account Not Found\n");
}




/*----------------------------------------main----------------------------------------------------*/

int main(){


    load();
    delete();
    printAllAccounts();


    

}
