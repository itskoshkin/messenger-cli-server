/**
 * @authors Stanislav Stoianov, Daria Pavlova
 */

#include "ServerService.h"
#include "TimeService.h"
#include "AuthService.h"

#define PORT 8080
#define MAXCONN 50

#define FOREACH(type, item, array, size) \
    size_t X(keep), X(i); \
    type item; \
    for (X(keep) = 1, X(i) = 0 ; X(i) < (size); X(keep) = !X(keep), X(i)++) \
        for (item = (array)[X(i)]; X(keep); X(keep) = 0)

#define _foreach(item, array) FOREACH(__typeof__(array[0]), item, array, length(array))
#define foreach(item_in_array) _foreach(item_in_array)

#define in ,
#define length(array) (sizeof(array) / sizeof((array)[0]))
#define CAT(a, b) CAT_HELPER(a, b) /* Concatenate two symbols for macros! */
#define CAT_HELPER(a, b) a ## b
#define X(name) CAT(__##name, __LINE__) /* unique variable */

pthread_mutex_t mutex;

/**
 * TODO FIXME Make a linkedlist or smth to deal with foreach cycle @SergeyBoryaev
 */

SOCKET clientSockets[MAXCONN];
int CURRENT_CONN = 0;


void *clientHandler(void *param) {
    pthread_mutex_lock(&mutex);
    printf("[%s] INFO: Client %llu the client handler was successfully initialized\n",
           getCurrentTime(), (SOCKET) param);
    pthread_mutex_unlock(&mutex);

    SOCKET clientSocket = (SOCKET) param;

    char receive[1024], transmit[1024], login[64];
    int isOk, ret;

    do {
        ret = recv(clientSocket, receive, 1024, 0);

        if (!ret || ret == SOCKET_ERROR) {
            pthread_mutex_lock(&mutex);
            printf("[%s] ERROR: Client %llu error getting data\n", getCurrentTime(), clientSocket);
            pthread_mutex_unlock(&mutex);

            /**
             * TODO self termination of a thread and removal of a list of online sockets from a socket
             */

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
            pthread_mutex_unlock(&mutex);
            sprintf(transmit, "%s is online!", login);
        } else {
            sprintf(transmit, "%d%c", isOk, '\0');
            ret = send(clientSocket, transmit, 1024, 0);
        }

        if (ret == SOCKET_ERROR) {
            pthread_mutex_lock(&mutex);
            printf("[%s] ERROR: Client %llu error sending authorization success report\n",
                   getCurrentTime(), (SOCKET) param);
            pthread_mutex_unlock(&mutex);

            /**
             * TODO self termination of a thread and removal of a list of online sockets from a socket
             */

            return (void *) 2;
        }

    } while (!isOk);


    foreach(client in clientSockets) {
            ret = send(client, transmit, sizeof(transmit), 0);
            if (ret == SOCKET_ERROR) {
                pthread_mutex_lock(&mutex);
                printf("[%s] ERROR: Client %llu did not receive a message about new user\n",
                       getCurrentTime(), (SOCKET) param);
                pthread_mutex_unlock(&mutex);
            }
        }

    /**
     * FIXME Interrogates the current connection and notifies everyone else
     */

    while (1) {
        if (recv(clientSocket, receive, 1024, 0) == SOCKET_ERROR) break;
        foreach(client1 in clientSockets) send(client1, receive, 1024, 0);
    }

    /*
     * TODO FIXME Notify to all clients that this connection is leaving
     */

    sprintf(transmit, "%s leaving", login);
    foreach(client2 in clientSockets) send(client2, transmit, 1024, 0);

    /**
     * TODO self termination of a thread and removal of a list of online sockets from a socket
     */
    return (void *) 0;
}

void clientAcceptor(SOCKET server) {
    int size;
    struct sockaddr_in sockaddrIn;
    SOCKET client;

    while (1) {
        size = sizeof(sockaddrIn);
        client = accept(server, (struct sockaddr *) &sockaddrIn, &size);
        printf("[%s] INFO: Client %llu was accepted\n", getCurrentTime(), client);

        if (client == SOCKET_ERROR) {
            printf("[%s] WARN: Client %llu error accept client\n", getCurrentTime(), client);
            continue;
        }

        /*
         * TODO add connection information to clients_list
         */

        if (CURRENT_CONN == 50) exit(EXIT_FAILURE);
        clientSockets[CURRENT_CONN] = client;
        CURRENT_CONN++;

        pthread_t pthread;
        pthread_create(&pthread, NULL, clientHandler, (void *) client);
        pthread_detach(pthread);

        /*
         * TODO removing connection information from clients_list
         */
    }

    /*
    pthread_mutex_destroy(&mutex);
    printf("[%s] INFO: Server is stopped\n", getCurrentTime());
    closesocket(server);
     */
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