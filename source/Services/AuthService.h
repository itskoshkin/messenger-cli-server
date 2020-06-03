/**
 * @authors Stanislav Stoianov
 */

#ifndef SERVER_AUTHSERVICE_H
#define SERVER_AUTHSERVICE_H

#include <stdbool.h>
#include <stdio.h>

bool processingUser(bool isRegistration, char *login, char *password);

#endif
