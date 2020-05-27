/**
 * @authors Stanislav Stoianov, Daria Pavlova
 */

#include <stdbool.h>

#ifndef SERVER_AUTHSERVICE_H

bool signIn(char* login, char * password);

bool signUp(char* login, char * password);

#define SERVER_AUTHSERVICE_H

#endif //SERVER_AUTHSERVICE_H
