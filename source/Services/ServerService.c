/**
 * @authors Stanislav Stoianov, Sergey Boryaev
 */

#include "ServerService.h"
#include "TimeService.h"
#include "AuthService.h"
#include "ClientStruct.h"

#define PORT 8080
#define MAXCONN 50

pthread_mutex_t mutex;


void *clientHandler(void *param) {
    Client* clientList;
    Client *temp;
    pthread_mutex_lock(&mutex);
    printf("[%s] INFO: Client %llu the Client handler was successfully initialized\n",
           getCurrentTime(), (SOCKET) param);
    pthread_mutex_unlock(&mutex);


    char receive[1024], transmit[1024], login[64];
    int isOk, ret;
    Client* currentClient;
    SOCKET clientSocket = (SOCKET) param;

    /**
     * AUTH
     */

    do {
        ret = recv(clientSocket, receive, 1024, 0);

        if (!ret || ret == SOCKET_ERROR) {
            pthread_mutex_lock(&mutex);
            printf("[%s] ERROR: Client %llu error getting auth data\n", getCurrentTime(), clientSocket);
            pthread_mutex_unlock(&mutex);

            pthread_mutex_lock(&mutex);
            if(!strcmp(currentClient->login, clientList->login))
                clientList = clientList->prev;
            deleteUser(currentClient);
            pthread_mutex_unlock(&mutex);

            return (void *) 1;
        }

        receive[ret] = '\0';

        pthread_mutex_lock(&mutex);
        printf("[%s] INFO: Client %llu was receive an auth request: %s\n", getCurrentTime(), clientSocket, receive);
        pthread_mutex_unlock(&mutex);

        char password[32];
        strcpy(login, receive + 2);
        int i;
        for (i = 0; i < strlen(login); i++) if (login[i] == ':') break;
        strcpy(password, login + i + 1);
        login[i] = '\0';

        isOk = (int) processingUser((bool) (receive[0] - '0'), login, password);

        if (isOk) {
            sprintf(transmit, "%d%c", isOk, '\0');
            ret = send(clientSocket, transmit, 1024, 0);
            pthread_mutex_lock(&mutex);
            printf("[%s] INFO: Client %llu login successful\n", getCurrentTime(), (SOCKET) param);
            currentClient = connectNewUser(clientList, clientSocket, login);
            if (clientList->next)
                clientList = clientList->next;
            printf("[%s] INFO: Client %llu successfully added to the mailing list\n", getCurrentTime(), (SOCKET) param);
            pthread_mutex_unlock(&mutex);

        } else {
            sprintf(transmit, "%d%c", isOk, '\0');
            ret = send(clientSocket, transmit, 1024, 0);
        }

        if (ret == SOCKET_ERROR) {
            pthread_mutex_lock(&mutex);
            printf("[%s] ERROR: Client %llu error sending authorization success report\n",
                   getCurrentTime(), (SOCKET) param);

            if(!strcmp(currentClient->login, clientList->login))
                clientList = clientList->prev;
            deleteUser(currentClient);

            printf("[%s] INFO: Client %llu successfully removed from the mailing list\n",
                   getCurrentTime(), (SOCKET) param);
            pthread_mutex_unlock(&mutex);

            return (void *) 2;
        }

    } while (!isOk);
    //уведомление о коннкекте
    sprintf(transmit, "%s is online!", currentClient->login);
    temp = clientList;
    while (temp->prev) {
        ret = send(clientList->client, transmit, 1024, 0);
        if (ret == SOCKET_ERROR) {
            pthread_mutex_lock(&mutex);
            printf("[%s] ERROR: Client %llu did not receive a message about new user\n",
                   getCurrentTime(), (SOCKET) param);
            if(!strcmp(currentClient->login, clientList->login))
                clientList = clientList->prev;
            deleteUser(currentClient);

            printf("[%s] INFO: Client %llu successfully removed from the mailing list\n",
                   getCurrentTime(), (SOCKET) param);
            pthread_mutex_unlock(&mutex);
        }
        temp = temp->prev;
    }

    /**
     * FIXME Опрашивает текущее соединение и уведомляет всех остальных
     */

    while (1) {
        //опросить текущее соединение
        ret = recv(clientSocket, receive, 1024, 0);

        //ошибка? удаление из списка активных соединений,
        if (ret == SOCKET_ERROR) {
            pthread_mutex_lock(&mutex);
            printf("[%s] ERROR: Client %llu did not receive a message about new user\n",
                   getCurrentTime(), (SOCKET) param);
            if(!strcmp(currentClient->login, clientList->login))
                clientList = clientList->prev;
            deleteUser(currentClient);

            printf("[%s] INFO: Client %llu successfully removed from the mailing list\n",
                   getCurrentTime(), (SOCKET) param);
            pthread_mutex_unlock(&mutex);
            break;
        }


        while (clientList->prev) {
            send(clientList->client, receive, 1024, 0);
        }
    }

    //уведомление о выходе
    sprintf(transmit, "%s leaving", login);
    temp = clientList;
    while (temp->prev) {
        ret = send(clientList->client, transmit, 1024, 0);
        if (ret == SOCKET_ERROR) {
            pthread_mutex_lock(&mutex);
            printf("[%s] ERROR: Client %llu did not receive a message about new user\n",
                   getCurrentTime(), (SOCKET) param);
            if(!strcmp(currentClient->login, clientList->login))
                clientList = clientList->prev;
            deleteUser(currentClient);

            printf("[%s] INFO: Client %llu successfully removed from the mailing list\n",
                   getCurrentTime(), (SOCKET) param);
            pthread_mutex_unlock(&mutex);
        }
        temp = temp->prev;
    }

    pthread_mutex_lock(&mutex);
    if(!strcmp(currentClient->login, clientList->login))
        clientList = clientList->prev;
    deleteUser(currentClient);
    printf("[%s] INFO: Client %llu disconnected from server\n",
           getCurrentTime(), (SOCKET) param);
    pthread_mutex_unlock(&mutex);

    return (void *) 0;
}

_Noreturn void clientAcceptor(SOCKET server) {
    int size;
    struct sockaddr_in sockaddrIn;
    SOCKET client;

    while (1) {
        size = sizeof(sockaddrIn);
        client = accept(server, (struct sockaddr *) &sockaddrIn, &size);
        printf("[%s] INFO: Client %llu was accepted\n", getCurrentTime(), client);

        if (client == SOCKET_ERROR) {
            printf("[%s] WARN: Client %llu error accept Client\n", getCurrentTime(), client);
            continue;
        }

        pthread_t pthread;
        pthread_create(&pthread, NULL, clientHandler, (void *) client);
        pthread_detach(pthread);

    }

    pthread_mutex_destroy(&mutex);
    printf("[%s] INFO: Server is stopped\n", getCurrentTime());
    closesocket(server);

}


void initServer() {
    SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    if (server == SOCKET_ERROR) {
        printf("[%s] ERROR: Error create server\n", getCurrentTime());
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in localaddr;
    localaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    localaddr.sin_family = AF_INET;
    localaddr.sin_port = htons(PORT);

    if (bind(server, (struct sockaddr *) &localaddr, sizeof(localaddr)) == SOCKET_ERROR) {
        printf("[%s] ERROR: Cannot start server\n", getCurrentTime());
        exit(EXIT_FAILURE);
    } else {
        printf("[%s] INFO: Server started\n", getCurrentTime());
        listen(server, MAXCONN);
        pthread_mutex_init(&mutex, NULL);
        clientAcceptor(server);
    }
}