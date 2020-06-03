//
// Created by Sergey on 6/3/2020.
//

#ifndef SERVER_CLIENTSTRUCT_H
#define SERVER_CLIENTSTRUCT_H

#include <stdbool.h>
#include <winsock2.h>
#include <malloc.h>
#include <string.h>

typedef struct client
{
    char* login;
    SOCKET client;
    struct client* next;
    struct client* prev;
}CLIENT;

struct client *addUser(struct client *lastUser, SOCKET newUser, char *login);
void deleteUser(struct client *targetUser);
struct client *makeFirstUser(SOCKET firstUser, char *login);
struct client *findByLogin(char *login, struct client *lastUser);

#endif //SERVER_CLIENTSTRUCT_H
