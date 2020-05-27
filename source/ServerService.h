//
// Created by Stanislav on 27.05.2020.
//

#ifndef SERVER_SERVERSERVICE_H
#define SERVER_SERVERSERVICE_H

#include <pthread.h>
#include <winsock2.h>
#include <stdio.h>

void *clientHandler(void *param);

void clientAcceptor(SOCKET server);

int initServer();

#endif //SERVER_SERVERSERVICE_H
