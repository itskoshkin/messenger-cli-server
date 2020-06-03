/**
 * @authors Sergey Boryaev
 */


#include "ClientStruct.h"

struct Client *addUser(struct Client *lastUser, SOCKET newUser, char *login) {
    struct Client *nextUser = (struct Client *) malloc(sizeof(struct Client));
    lastUser->next = nextUser;
    nextUser->prev = lastUser;
    nextUser->client = newUser;
    nextUser->next = NULL;
    nextUser->IsOnline = true;
    return nextUser;
}

void deleteUser(struct Client *targetUser) {
    targetUser->prev->next = targetUser->next;
    targetUser->next->prev = targetUser->prev;
    free(targetUser->login);
    free(targetUser);
}

struct Client *findByLogin(char *login, struct Client *lastUser) {
    struct Client *temp = lastUser;
    while (lastUser) {
        if (!strcmp(login, temp->login))
            return temp;
        temp = temp->prev;
    }
    return NULL;
}

struct Client *connectNewUser(struct Client *lastUser, SOCKET newUser, char *login) {
    struct Client *User;
    User = findByLogin(login, lastUser);
    if (User) {
        User->IsOnline = true;
        User->client = newUser;
    } else
        User = addUser(lastUser, newUser, login);
    return User;
}