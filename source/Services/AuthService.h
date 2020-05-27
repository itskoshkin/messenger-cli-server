/**
 * @authors Stanislav Stoianov, Daria Pavlova
 */

#ifndef SERVER_AUTHSERVICE_H

#include <stdbool.h>
#include <stdio.h>

bool signIn(char *login, char *password);

bool signUp(char *login, char *password);

char* makeData(char *log, char *pas);
bool stringFind(FILE *database, char *login, long file_size, char *password);


#define SERVER_AUTHSERVICE_H

#endif
