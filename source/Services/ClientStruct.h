//
// Created by Sergey on 6/3/2020.
//

#ifndef SERVER_CLIENTSTRUCT_H
#define SERVER_CLIENTSTRUCT_H

#include <stdbool.h>
#include <winsock2.h>
#include <malloc.h>

typedef struct client
{
    SOCKET client;
    struct client* next;
    struct client* prev;
}CLIENT;

struct client *addUser(struct client *lastUser, SOCKET newUser);
void deleteUser(struct client *targetUser);
struct client *makeFirstUser();

#endif //SERVER_CLIENTSTRUCT_H
