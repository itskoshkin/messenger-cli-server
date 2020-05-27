#ifndef SERVER_SERVERSERVICE_H
#define SERVER_SERVERSERVICE_H

#endif //SERVER_SERVERSERVICE_H

#include <pthread.h>
#include <winsock2.h>
#include <stdio.h>

/**
 * @authors Stanislav Stoianov, Daria Pavlova
 */

void *clientHandler(void *param);

void clientAcceptor(SOCKET server);

int initServer();