/**
 * @authors Sergey Boryaev, Darya Pavlova
 */

#include "ClientStruct.h"

struct Client *init(SOCKET newUser, char *login) {

}

struct Client *addUser(struct Client *lastUser, SOCKET newUser, char *login) {
    if (lastUser) {
        struct Client *nextUser = (struct Client *) malloc(sizeof(struct Client));
        nextUser->prev = lastUser;
        nextUser->client = newUser;
        nextUser->next = NULL;
        nextUser->IsOnline = true;
        nextUser->login = login;
        lastUser->next = nextUser;
        return nextUser;
    } else {
        lastUser = (struct Client *) malloc(sizeof(struct Client));
        lastUser->prev = NULL;
        lastUser->next = NULL;
        lastUser->IsOnline = true;
        lastUser->client = newUser;
        lastUser->login = login;
        return lastUser;
    }

}

void deleteUser(struct Client *targetUser) {
    struct Client *prev, *next;
    prev = targetUser->prev;
    next = targetUser->next;
    if (prev != NULL)
        targetUser->prev->next = targetUser->next;
    if (next != NULL)
        targetUser->next->prev = targetUser->prev;
    // free(targetUser->login);
    free(targetUser);
}

struct Client *findByLogin(char *login, struct Client *lastUser) {
    struct Client *temp = lastUser;
    while (temp) {
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