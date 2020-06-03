//
// Created by Sergey on 6/3/2020.
//

#include "ClientStruct.h"

struct client *addUser(struct client *lastUser, SOCKET newUser){
    struct client* nextUser = (struct client*)malloc(sizeof(struct client));
    lastUser->next = nextUser;
    nextUser->prev = lastUser;
    nextUser->client = newUser;
    nextUser->next = NULL;
    return nextUser;
}

void deleteUser(struct client *targetUser){
    targetUser->prev->next = targetUser->next;
    targetUser->next->prev = targetUser->prev;
    free(targetUser);
}

struct client *makeFirstUser(SOCKET firstUser){
    struct client* firstUserStruct = (struct client*)malloc(sizeof(struct client));
    firstUserStruct->client = firstUser;
    firstUserStruct->next = NULL;
    firstUserStruct->prev = NULL;
    return firstUserStruct;
}