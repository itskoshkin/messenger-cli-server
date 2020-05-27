/**
 * @authors Stanislav Stoianov, Daria Pavlova
 */


#include <pthread.h>
#include <winsock2.h>
#include <stdio.h>

#ifndef SERVER_SERVERSERVICE_H
void *clientHandler(void *param);

void clientAcceptor(SOCKET server);

int initServer();
#define SERVER_SERVERSERVICE_H

#endif //SERVER_SERVERSERVICE_H
