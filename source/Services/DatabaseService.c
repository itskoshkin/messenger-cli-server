/**
 * @authors Daria Pavlova
 */

#include "DatabaseService.h"
#include "TimeService.h"
#include "stdlib.h"
#include "string.h"
#include <stdio.h>

#define STR_LEN_MAX 100

bool stringFind(FILE *database, char *login, long file_size, int passwordcheck, char *password);

bool openData(FILE *database);

char *makeData(char *login, char *password) {
    int size = strlen(login) + strlen(password) + 2;
    char *temp = (char *)calloc(size, sizeof(char));
    strcat(temp, login);
    strcat(temp, ":");
    strcat(temp, password);
    strcat(temp, "\n");

    return temp;
}

bool openData(FILE *database) {
    if (!database) {
       fprintf(stderr, "[%s] ERROR: Error open database\n", getCurrentTime());
        return false;
    }
    return true;
}

bool stringFind(FILE *database, char *login, long file_size, int passwordcheck, char *password) {
    //go to the beginning of file
    fseek(database, 0, SEEK_SET);

    //buffer for checking
    char* string = (char *) calloc(STR_LEN_MAX, sizeof(char));
    if (!string) exit(EXIT_FAILURE);
    char *stringbuf = (char *) calloc(STR_LEN_MAX, sizeof(char));
    if (!stringbuf) exit(EXIT_FAILURE);

    //skip the strings while it is not a neccessary key
    while (strcmp(stringbuf, login)) {
        //read a string from file
        fgets(string, STR_LEN_MAX, database);
        stringbuf = strtok(string, ":");


        //if the end of file
        if (ftell(database) == file_size && strcmp(stringbuf, login)) {
            return false;
        }
    }

    if (passwordcheck){
        stringbuf = strtok(NULL, ":");
        stringbuf[strlen(stringbuf) - 1] = '\0';
        if (!strcmp(password, stringbuf))
            return true;
        else
            return false;
    }

    return true;
}

bool signIn(char *login, char *password) {
    //open file
    FILE *database = fopen("../data/users.txt", "rt");
    openData(database);
    fseek(database, 0, SEEK_END);
    long file_size = ftell(database);

    //check database
    if (!stringFind(database, login, file_size, 1, password)) {
        fclose(database);
        return false;
    }

    return true;
}

bool signUp(char *login, char *password) {
    //open file
    FILE *database = fopen("../data/users.txt", "rt");
    openData(database);

    fseek(database, 0, SEEK_END);
    long file_size = ftell(database);
    //check database
    if (stringFind(database, login, file_size, 0, 0)) {
        return false;
    }

    //add to database
    char *data = makeData(login, password);
    database = fopen("../data/users.txt", "a");
    openData(database);

    fprintf(database, "%s", data);
    fclose(database);

    return true;
}

//TEST function
/*
int main() {
    if (!signIn("Daria", "mypassword"))
        printf("wrong");
    signUp("STASS", "PASSWORD");
    signUp("SERYOZHENKA", "TUTPAROL");
    if (!signIn("SERYOZHENKA", "TUTPAROL"))
        printf("wrong");
    if (!signIn("STASS", "PASSWORD"))
        printf("wrong");
    return 0;
}*/