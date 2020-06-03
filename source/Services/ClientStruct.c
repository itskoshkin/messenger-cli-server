//
// Created by Sergey on 6/3/2020.
//

#include "ClientStruct.h"

struct client *addUser(struct client *lastUser, SOCKET newUser, char *login){
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
    free(targetUser->login);
    free(targetUser);
}

struct client *makeFirstUser(SOCKET firstUser, char *login){
    struct client* firstUserStruct = (struct client*)malloc(sizeof(struct client));
    firstUserStruct->client = firstUser;
    firstUserStruct->next = NULL;
    firstUserStruct->prev = NULL;
    firstUserStruct->login = (char *)calloc(strlen(login)+1, sizeof(char));
    strcpy(firstUserStruct->login, login);
    return firstUserStruct;
}

struct client *findByLogin(char *login, struct client *lastUser){
    struct client *temp = lastUser;
    while (lastUser){
        if(!strcmp(login,temp->login))
            return temp;
        temp=temp->prev;
    }
    return  NULL;
}