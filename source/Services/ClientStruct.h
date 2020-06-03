/**
 * @authors Sergey Boryaev
 */

#include <stdbool.h>
#include <winsock2.h>
#include <malloc.h>
#include <string.h>

#ifndef SERVER_CLIENTSTRUCT_H

typedef struct Client {
    char *login;
    bool IsOnline;
    SOCKET client;
    struct Client *next;
    struct Client *prev;
} Client;

struct Client *addUser(struct Client *lastUser, SOCKET newUser, char *login);

void deleteUser(struct Client *targetUser);

struct Client *findByLogin(char *login, struct Client *lastUser);

struct Client *connectNewUser(struct Client *lastUser, SOCKET newUser, char *login);

#define SERVER_CLIENTSTRUCT_H

#endif
