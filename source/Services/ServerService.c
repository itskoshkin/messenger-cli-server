/**
 * @authors Stanislav Stoianov, Daria Pavlova
 */

#include "ServerService.h"
#include "TimeService.h"
#include "AuthService.h"

#define PORT 8080
#define MAXCONN 50
#define foreach(item, array) \
    for(int keep = 1, \
            count = 0,\
            size = sizeof (array) / sizeof *(array); \
        keep && count != size; \
        keep = !keep, count++) \
      for(item = (array) + count; keep; keep = !keep)

pthread_mutex_t mutex;
SOCKET clientSockets[MAXCONN];

/*
 * Add the function of receiving messages from the user and sending them to all users at the same time
 */

void *clientMessageHandler(SOCKET *clientSocket) {
    while (1) {
        char receive[1024];
        foreach(SOCKET client, clientSockets) {
                //TODO
            }
    }
}

void *clientHandler(void *param) {
    printf("[%s] INFO: Client %llu the client handler was successfully initialized\n",
           getCurrentTime(), (SOCKET) param);
    SOCKET clientSocket = (SOCKET) param;

    /*
     * Registration processing
     */

    char receive[1024], transmit[1024];
    int isOk = 0, ret = 0;
    do {
        ret = recv(clientSocket, receive, 1024, 0);
        if (!ret || ret == SOCKET_ERROR) {
            pthread_mutex_lock(&mutex);
            printf("[%s] ERROR: Client %llu error getting data\n", getCurrentTime(), (SOCKET) param);
            pthread_mutex_unlock(&mutex);
            return (void *) 1;
        }
        receive[ret] = '\0';

        pthread_mutex_lock(&mutex);
        printf("[%s] INFO: Client %llu was receive an auth request: %s\n", getCurrentTime(), (SOCKET) param, receive);
        pthread_mutex_unlock(&mutex);

        char login[64], password[32];
        strcpy(login, receive + 2);
        int i;
        for (i = 0; i < strlen(login); i++) if (login[i] == ':') break;
        strcpy(password, login + i + 1);
        login[i] = '\0';

        isOk = (int) processingUser((bool) (receive[0] - '0'), login, password);

        if (isOk) {
            sprintf(transmit, "%d%c", isOk, '\0');
            ret = send(clientSocket, transmit, sizeof(transmit), 0);
            pthread_mutex_lock(&mutex);
            printf("[%s] INFO: Client %llu login successful\n", getCurrentTime(), (SOCKET) param);
            pthread_mutex_unlock(&mutex);
        } else {
            sprintf(transmit, "%d%c", isOk, '\0');
            ret = send(clientSocket, transmit, sizeof(transmit), 0);
        }

        if (ret == SOCKET_ERROR) {
            pthread_mutex_lock(&mutex);
            printf("[%s] ERROR: Client %llu error sending authorization success report\n",
                   getCurrentTime(), (SOCKET) param);
            pthread_mutex_unlock(&mutex);
            return (void *) 2;
        }

    } while (!isOk);

    clientMessageHandler(&clientSocket);

    return (void *) 0;
}

void clientAcceptor(SOCKET server) {
    int size;
    struct sockaddr_in clientaddr;
    SOCKET client;
    while (1) {
        size = sizeof(clientaddr);
        client = accept(server, (struct sockaddr *) &clientaddr, &size);
        printf("[%s] INFO: Client %llu was accepted\n", getCurrentTime(), client);

        if (client == INVALID_SOCKET) {
            printf("[%s] WARN: Client %llu error accept client\n", getCurrentTime(), client);
            continue;
        }
        pthread_t mythread;
        int status = pthread_create(&mythread, NULL, clientHandler, (void *) client);
        pthread_detach(mythread);
    }
    pthread_mutex_destroy(&mutex);
    printf("[%s] INFO: Server is stopped\n", getCurrentTime());
    closesocket(server);
}

int initServer() {
    SOCKET server;
    struct sockaddr_in localaddr;

    server = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (server == INVALID_SOCKET) {
        printf("[%s] ERROR: Error create server\n", getCurrentTime());
        return 1;
    }
    localaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    localaddr.sin_family = AF_INET;
    localaddr.sin_port = htons(PORT);//port number is for example, must be more than 1024
    if (bind(server, (struct sockaddr *) &localaddr, sizeof(localaddr)) == SOCKET_ERROR) {
        printf("[%s] ERROR: Cannot start server\n", getCurrentTime());
        return 2;
    } else {
        printf("[%s] INFO: Server started\n", getCurrentTime());
    }
    listen(server, 50);//50 клиентов в очереди могут стоять
    pthread_mutex_init(&mutex, NULL);
    clientAcceptor(server);
    return 0;
}