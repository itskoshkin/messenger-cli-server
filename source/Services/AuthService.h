/**
 * @authors Stanislav Stoianov, Daria Pavlova
 */

#ifndef SERVER_AUTHSERVICE_H

#include <stdbool.h>
#include <stdio.h>

bool signIn(char *login, char *password);

bool signUp(char *login, char *password);

char *makeData(char *login, char *password);

int stringFind(FILE *database, char *string, long file_size);

#define SERVER_AUTHSERVICE_H

#endif
