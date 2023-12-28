#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "input_functions.h"
#include "save.h"
#include "print.h"
#include "validations.h"
#include "load.h"


int getExistingAccountIndex(){
    char * account_number;
    int index;
    while(1){
        account_number = getOnHeapTillValid(maxAccountNumberSize , "enter sender account number : " , "account number is very long\n" , "invalid account number\n" ,  isnum);
        index = searchAccNum(account_number);
        
        // no free if the sender account is found as it will be compared to the receiver account 
        if(index != -1){
            free(account_number);
            return index;
        }
        printf("account not found\n");
    }
}


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
            printf("welcome %s :)\n", username);
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
    char * accountNumber = getOnHeapTillValid(maxAccountNumberSize , "enter account number to be modified : " , "account number is very long\n" , "invalid account number\n" ,  isnum );


    // if file doesn't exist what to do
    // end if
    for (int i = 0; i < accounts.number; i++) {
        
        if (strcmp(accountNumber , accounts.array[i].accountNumber) != 0) continue;
        free(accountNumber); // no more use of account number


        char *tempName = copyStrOnHeap(accounts.array[i].name), *tempEmail = copyStrOnHeap(accounts.array[i].emailAddress), *tempMobile = copyStrOnHeap(accounts.array[i].mobileNumber);

        // no more use of them (will be modified)
        free(accounts.array[i].name);free(accounts.array[i].emailAddress);free(accounts.array[i].mobileNumber);
        printf("here\n");

        accounts.array[i].name = getOnHeapTillValid( maxNameSize , "Enter new name: " , "name is very long\n" , "invalid name\n" , isValidName );
        accounts.array[i].emailAddress = getOnHeapTillValid( maxEmailSize , "Enter new email: " , "email is very long\n" , "invalid email\n" , isValidEmail );
        accounts.array[i].mobileNumber = getOnHeapTillValid( maxMobileSize , "Enter new mobile number: " , "mobile number is very long\n" , "invalid mobile number\n" , isMobile);

        // save is after save_prompt to avoid saving if user didn't want to save (using short circuit evaluation) 
        if(!isSaveWanted() || !save()){
            // no use of modification strings 
            free(accounts.array[i].name);free(accounts.array[i].emailAddress);free(accounts.array[i].mobileNumber);
            
            accounts.array[i].name = tempName;
            accounts.array[i].emailAddress = tempEmail;
            accounts.array[i].mobileNumber = tempMobile;

            return;
        }
        
        printf("Account details updated successfully.\n");
        return;

    }

    free(accountNumber);
    printf("Account not found.\n");
}

// Function to transfer money between accounts
void transfer() {
    int senderIndex = getExistingAccountIndex() ,
    receiverIndex =  getExistingAccountIndex();

    double amount;
    getnumberTillValid( "%lf" , &amount , "enter amount to be transferred: " , "invalid input (not a number)\n" , "invalid input (not a positive number)\n" , isPositiveDouble);

    if(accounts.array[senderIndex].balance < amount){
        printf("Insufficient balance in the sender's account.\n");
        return;
    }

    printf("%.2lf\n" , accounts.array[senderIndex].balance);
    printf("%.2lf\n" , accounts.array[receiverIndex].balance);

    accounts.array[senderIndex].balance -= amount;
    printf("%.2lf\n" , accounts.array[senderIndex].balance);
    accounts.array[receiverIndex].balance += amount;
    printf("%.2lf\n" , accounts.array[receiverIndex].balance);
    printf("%.2lf $ transferred from account number %s to account number %s successfully.\n", amount, accounts.array[senderIndex].accountNumber, accounts.array[receiverIndex].accountNumber);

    if(!isSaveWanted() || !save()){
        accounts.array[senderIndex].balance += amount;
        accounts.array[receiverIndex].balance -= amount;
        printf("transfer not saved\n");
    }


}

