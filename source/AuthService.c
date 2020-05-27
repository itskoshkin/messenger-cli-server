/**
 * @authors Stanislav Stoianov, Daria Pavlova
 */

#include "AuthService.h"
#include "stdlib.h"
#include "string.h"
#include <stdio.h>
#define STR_LEN_MAX 100

bool signIn(char* login, char * password){

    return false;
}

char* makeData(char *log, char *pas){
    int size = strlen(log) + strlen(pas) + 2;
    char * temp = (char*)malloc(size);
    strcat(temp, log);
    strcat(temp, ":");
    strcat(temp, pas);
    strcat(temp, "\n");

    return temp;
}

int stringFind(FILE* fr, char* str, long file_size){
    //go to the begin of file
    fseek(fr, 0, SEEK_SET);

    //buffer for checking
    char* string = (char*)calloc(STR_LEN_MAX, sizeof(char));
    if (!string) exit(EXIT_FAILURE);
    char* stringbuf = (char*)calloc(STR_LEN_MAX, sizeof(char));
    if (!stringbuf) exit(EXIT_FAILURE);

    //skip the strings while it is not a neccessary key
    while (strcmp(stringbuf, str))
    {
        //read a string from file
        fgets(string, STR_LEN_MAX, fr);
        stringbuf = strtok (str, ":");

        //if the end of file
        if (ftell(fr) == file_size)
        {
            free(string);
            return 0;
        }
    }

    free(string);
    return 1;
}

bool signUp(char* login, char * password){
    //open file
    FILE* database = fopen("../data/users.txt", "rt");
    if (!database)
    {
        puts("Error open database");
        return -2;
    }
    fseek(database, 0, SEEK_END);
    long file_size = ftell(database);

    //check database
    if (stringFind(database, login, file_size))
    {
        printf("\nERROR: the login are already used");
        return false;
    }

    //add to database
    char *data = makeData(login, password);
    database = fopen("../data/users.txt", "a");
    if (!database)
    {
        puts("Error open database");
        return -2;
    }
    fprintf(database, "%s", data);

    return true;
}

/* TEST function
int main(){
    signUp("nat", "1002");

}*/