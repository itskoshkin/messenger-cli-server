/**
 * @authors Stanislav Stoianov, Daria Pavlova
 */

#include "AuthService.h"
#include "stdlib.h"
#include "string.h"
#include <stdio.h>
#define STR_LEN_MAX 100

bool signIn(char* login, char * password){
    //open file
    FILE* database = fopen("../data/users.txt", "rt");
    if (!database)
    {
        puts("Error open database");
        return -2;
    }
    fseek(database, 0, SEEK_END);
    long file_size = ftell(database);

    if (!stringFind(database, login, file_size, password))
    {
        printf("\nERROR: the login are already used");
        return false;
    }

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

int stringFind(FILE *database, char *str, long file_size) {
    //go to the begin of file
    fseek(database, 0, SEEK_SET);

    //buffer for checking
    char *string = (char *) calloc(STR_LEN_MAX, sizeof(char));
    if (!string) exit(EXIT_FAILURE);
    char *stringbuf = (char *) calloc(STR_LEN_MAX, sizeof(char));
    if (!stringbuf) exit(EXIT_FAILURE);

    //skip the strings while it is not a neccessary key
    while (strcmp(stringbuf, string)) {
        //read a string from file
        fgets(string, STR_LEN_MAX, database);
        stringbuf = strtok(string, ":");

        //if the end of file
        if (ftell(database) == file_size) {
            free(string);
            return 0;
        }
    }

    free(string);
    return 1;
}

// TEST function
int main(){
    signIn("nat", "1002");
}

