/**
 * @authors Stanislav Stoianov, Daria Pavlova
 */

#include "DatabaseService.h"
//#include "TimeService.h"
#include "stdlib.h"
#include "string.h"
#include <stdio.h>

#define STR_LEN_MAX 100

/** TODO
 * обработка разных ошибок входа
 * обработка разных ошибок регистрации
 */

bool stringFind(FILE *database, char *login, long file_size);
bool checkPassword(char *password, FILE *database);

bool signIn(char *login, char *password) {
    //open file
    FILE *database = fopen("../data/users.txt", "rt");
    if (!database) {
        printf("[%s] Error open database\n", getCurrentTime());
        return -2;
    }
    fseek(database, 0, SEEK_END);
    long file_size = ftell(database);

    //check database
    if (!stringFind(database, login, file_size)) {
        printf("[%s] ERROR: wrong login\n", getCurrentTime());
        return false;
    }

    //check password
    if (checkPassword(password, database))
        return true;
    else{
         printf("[%s] ERROR: wrong password\n", getCurrentTime());
        return false;
    }

}

char *makeData(char *login, char *password) {
    int size = strlen(login) + strlen(password) + 2;
    char *temp = (char *) malloc(size);
    strcat(temp, login);
    strcat(temp, ":");
    strcat(temp, password);
    strcat(temp, "\n");

    return temp;
}

bool checkPassword(char *password, FILE *database){
    //pointer is already in the line with login

    char *string = (char *) calloc(STR_LEN_MAX, sizeof(char));
    if (!string) exit(EXIT_FAILURE);
    char *stringbuf = (char *) calloc(STR_LEN_MAX, sizeof(char));
    if (!stringbuf) exit(EXIT_FAILURE);

    //get password
    fgets(string, STR_LEN_MAX, database);
    stringbuf = strtok(string, ":");
    stringbuf = strtok(NULL, ":");
    stringbuf[strlen(stringbuf)-1] = '\0';

    if (!strcmp(password, stringbuf))
        return true;
    else
        return false;

}

bool stringFind(FILE *database, char *login, long file_size) {
    //go to the beginning of file
    fseek(database, 0, SEEK_SET);

    //buffer for checking
    char *string = (char *) calloc(STR_LEN_MAX, sizeof(char));
    if (!string) exit(EXIT_FAILURE);
    char *stringbuf = (char *) calloc(STR_LEN_MAX, sizeof(char));
    if (!stringbuf) exit(EXIT_FAILURE);


    //skip the strings while it is not a neccessary key
    while (strcmp(stringbuf, login)) {
        //read a string from file
        fgets(string, STR_LEN_MAX, database);
        stringbuf = strtok(string, ":");

        printf("%s - %s\n", stringbuf, login);

        //if the end of file
        if (ftell(database) == file_size) {
            return false;
        }
    }

    return true;
}

bool signUp(char *login, char *password) {
    //open file
    FILE *database = fopen("../data/users.txt", "rt");
    if (!database) {
        printf("[%s] ERROR: Error open database\n", getCurrentTime());
        return false;
    }
    fseek(database, 0, SEEK_END);
    long file_size = ftell(database);

    //check database
    if (stringFind(database, login, file_size)) {
        printf("[%s] ERROR: the login are already used\n", getCurrentTime());
        return false;
    }

    //add to database
    char *data = makeData(login, password);
    database = fopen("../data/users.txt", "a");
    if (!database) {
        printf("[%s] ERROR: Error open database\n", getCurrentTime());
        return false;
    }
    fprintf(database, "%s", data);

    return true;
}

//TEST function
int main(){
   signIn("natis", "100002");

  return 0;

}