/**
 * @authors Stanislav Stoianov
 */

#ifndef SERVER_AUTHSERVICE_H
#define SERVER_AUTHSERVICE_H

#include <stdbool.h>
#include <stdio.h>

bool signIn(char *login, char *password);

bool signUp(char *login, char *password);

#endif
