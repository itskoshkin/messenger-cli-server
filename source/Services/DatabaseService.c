/**
 * @authors Stanislav Stoianov, Daria Pavlova
 */

#include "DatabaseService.h"
#include "TimeService.h"
#include "stdlib.h"
#include "string.h"
#include <stdio.h>

#define STR_LEN_MAX 100

/** TODO
 * обработка разных ошибок входа
 * обработка разных ошибок регистрации
 */

bool stringFind(FILE *database, char *login, long file_size, char *password);

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
    if (!stringFind(database, login, file_size, password)) {
        printf("[%s] ERROR: the login are not exist\n", getCurrentTime());
        return false;
    }

    return true;
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

bool stringFind(FILE *database, char *login, long file_size, char *password) {
    //go to the begin of file
    fseek(database, 0, SEEK_SET);

    //buffer for checking
    char *string = (char *) calloc(STR_LEN_MAX, sizeof(char));
    if (!string) exit(EXIT_FAILURE);
    char *stringbuf = (char *) calloc(STR_LEN_MAX, sizeof(char));
    if (!stringbuf) exit(EXIT_FAILURE);
    char *stringbuf2 = (char *) calloc(STR_LEN_MAX, sizeof(char));
    if (!stringbuf2) exit(EXIT_FAILURE);
    char *stringbuf3 = (char *) calloc(STR_LEN_MAX, sizeof(char));
    if (!stringbuf3) exit(EXIT_FAILURE);

    if (password == NULL)
        stringbuf2 = NULL;

    //skip the strings while it is not a neccessary key
    while (strcmp(stringbuf, login) && strcmp(stringbuf2, password)) {
        //read a string from file
        fgets(string, STR_LEN_MAX, database);
        stringbuf = strtok(string, ":");

        if (password != NULL) {
            stringbuf3 = stringbuf;
            stringbuf = strtok(NULL, ":");
        }

        stringbuf2 = stringbuf;
        stringbuf = stringbuf3;
        printf("%s - %s \n", stringbuf, stringbuf2);

        //if the end of file
        if (ftell(database) == file_size) {
            free(string);
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
    if (stringFind(database, login, file_size, NULL)) {
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

/*//TEST function
int main(){
   signIn("nat", "1002");
  return 0;

}*/