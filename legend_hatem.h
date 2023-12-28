#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


#include "print.h"
#include "globals.h"
#include "str_utils.h"
#include "input_functions.h"
#include "save.h"
#include "load.h"
#include "validations.h"



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void query (){
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
        printf("Account number not found");
        free(account_number);
        return;
}

void advancedsearch(){
    int found = 0;
    char *keyword, *name = malloc(maxNameSize * sizeof(char));
    keyword = getOnHeapTillValid(maxNameSize , "Enter keyword: " , "keyword is very long\n" , "keyword is not a name\n" , isValidName);
    printf("\n");
    for (int i=0; i< accounts.number; i++){
        sprintf(name , "%s" , accounts.array[i].name);
        tolowercase(name);
        tolowercase(keyword);
        removeSpaces(keyword);
        if (strstr( name , keyword)){
            printAccount(i);
            printf("\n");
        }
    }

    if(!found)
        printf("No matches are found");
    
    free(name);
    free(keyword);
}

void addaccount(){
    char *name, *email , *account_number, *mobile_number;
    double balance;

    name = getOnHeapTillValid(maxNameSize , "Account Holder name: " , "name is very long\n" , "name can not have non alphabetic characters\n" , isValidName);
    
    email = getOnHeapTillValid( maxEmailSize , "Email: " , "email is very long\n" , "email is not valid\n" , isValidEmail);


    account_number = getOnHeapTillValid( maxAccountNumberSize , "please enter account number: " , "account number is very long\n" , "Please enter a valid account number\n" , isnum);


    do{
        
        int i;
        for (i = 0; i < accounts.number && strcmp(account_number, accounts.array[i].accountNumber) != 0; i++);
        
        if(i==accounts.number) break;

        free(account_number);
        printf("account number already exists\n");
        account_number = getOnHeapTillValid( maxAccountNumberSize , "please enter account number: " , "account number is very long\n" , "Please enter a valid account number\n" , isnum);

    }while(1);


    getDoubleTillValid(&balance , "balance: " , "enter a valid balance\n");

    mobile_number =  getOnHeapTillValid( maxMobileSize , "Mobile number: " , "Please enter a valid 11-digit mobile number\n" , "Please enter a valid 11-digit mobile number\n" , isMobile);
    




    // save 
    // end save

    int save_request = isSaveWanted();
    if(!save_request) return;

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
    if(!save()){
        accounts.number--;
        free(accounts.array[accounts.number].name);
        free(accounts.array[accounts.number].mobileNumber);
        free(accounts.array[accounts.number].accountNumber); 
        free(accounts.array[accounts.number].emailAddress); 
    }
}


