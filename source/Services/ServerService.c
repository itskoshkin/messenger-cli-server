/**
 * @authors Stanislav Stoianov, Sergey Boryaev, Darya Pavlova
 */

#include "ServerService.h"
#include "TimeService.h"
#include "AuthService.h"
#include "ClientStruct.h"

#define bzero(b, len) (memset((b), '\0', (len)), (void) 0)

#define PORT 8080
#define MAXCONN 50

pthread_mutex_t mutex;

Client *clientList;

void *clientHandler(void *param) {
    Client *temp;
    pthread_mutex_lock(&mutex);
    printf("[%s] INFO: Client %llu the Client handler was successfully initialized\n",
           getCurrentTime(), (SOCKET) param);
    pthread_mutex_unlock(&mutex);


    char receive[1024], transmit[1024], login[64];
    int isOk, ret;
    Client *currentClient;
    SOCKET clientSocket = (SOCKET) param;

    do {
        ret = recv(clientSocket, receive, 1024, 0);

        if (!ret || ret == SOCKET_ERROR) {
            pthread_mutex_lock(&mutex);
            printf("[%s] ERROR: Client %llu error getting auth data\n", getCurrentTime(), clientSocket);
            printf("[%s] ERROR: Client %llu will be disconnected from server\n", getCurrentTime(), clientSocket);
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
            printf("[%s] INFO: Client %llu is login successful\n", getCurrentTime(), (SOCKET) param);
            currentClient = addUser(clientList, clientSocket, login);
            clientList = currentClient;
            printf("[%s] INFO: Client %llu successfully added to the mailing list\n", getCurrentTime(), (SOCKET) param);
            pthread_mutex_unlock(&mutex);
        } else {
            sprintf(transmit, "%d%c", isOk, '\0');
            printf("[%s] INFO: Client %llu is not login successful\n", getCurrentTime(), (SOCKET) param);
            ret = send(clientSocket, transmit, 1024, 0);
        }

        if (ret == SOCKET_ERROR) {
            pthread_mutex_lock(&mutex);
            printf("[%s] ERROR: Client %llu error sending authorization success report\n",
                   getCurrentTime(), (SOCKET) param);
            if (!strcmp(currentClient->login, clientList->login) && currentClient->prev) {
                clientList = clientList->prev;
                deleteUser(currentClient);
            } else {
                deleteUser(currentClient);
            }
            printf("[%s] INFO: Client %llu successfully removed from the mailing list\n",
                   getCurrentTime(), (SOCKET) param);
            printf("[%s] ERROR: Client %llu will be disconnected from server\n",
                   getCurrentTime(), clientSocket);
            pthread_mutex_unlock(&mutex);
            return (void *) 2;
        }

    } while (!isOk);

    sprintf(transmit, "%s is online!\n", currentClient->login);
    temp = clientList;
    while (temp) {
        ret = send(temp->client, transmit, 1024, 0);
        if (ret == SOCKET_ERROR) {
            pthread_mutex_lock(&mutex);
            printf("[%s] ERROR: Client %llu did not receive a message about new user\n",
                   getCurrentTime(), temp->client);
            if (!strcmp(temp->login, clientList->login) && (temp->prev)) {
                clientList = clientList->prev;
                temp = temp->prev;
                deleteUser(temp->next);
            } else {
                deleteUser(temp);
            }
            printf("[%s] INFO: Client %llu successfully removed from the mailing list\n",
                   getCurrentTime(), temp->client);
            printf("[%s] ERROR: Client %llu will be disconnected from server\n",
                   getCurrentTime(), temp->client);
            pthread_mutex_unlock(&mutex);
            continue;
        }
        temp = temp->prev;
    }

    Client *tempup;
    Client *tempdown;
    while (1) {
        ret = recv(clientSocket, receive, 1024, 0);
        if (ret == SOCKET_ERROR) goto exit;
        pthread_mutex_lock(&mutex);
        tempdown = clientList->prev;
        while (tempdown) {
            send(tempdown->client, receive, 1024, 0);
            printf("[%s] INFO: Client %llu send a message %s to client %llu\n",
                   getCurrentTime(), clientSocket, receive, tempdown->client);
            tempdown = tempdown->prev;
        }
        tempup = clientList->next;
        while (tempup) {
            send(tempup->client, receive, 1024, 0);
            printf("[%s] INFO: Client %llu send a message %s to client %llu\n",
                   getCurrentTime(), clientSocket, receive, tempup->client);
            tempup = tempup->next;
        }
        send(clientList->client, receive, 1024, 0);
        printf("[%s] INFO: Client %llu send a message %s to client %llu\n",
               getCurrentTime(), clientSocket, receive, clientSocket);
        pthread_mutex_unlock(&mutex);
        bzero(receive, sizeof(receive));
    }

    exit:
    sprintf(transmit, "%s leaving\n", login);
    temp = clientList;
    while (temp) {
        ret = send(temp->client, transmit, 1024, 0);
        if (ret == SOCKET_ERROR) {
            pthread_mutex_lock(&mutex);
            printf("[%s] ERROR: Client %llu did not receive a message about disconnected client %llu\n",
                   getCurrentTime(), temp->client, clientSocket);
            if (!strcmp(temp->login, clientList->login) && (temp->prev)) {
                clientList = clientList->prev;
                temp = temp->prev;
                deleteUser(temp->next);
            } else
                deleteUser(temp);
            printf("[%s] INFO: Client %llu successfully removed from the mailing list\n",
                   getCurrentTime(), temp->client);
            printf("[%s] ERROR: Client %llu will be disconnected from server\n",
                   getCurrentTime(), temp->client);
            pthread_mutex_unlock(&mutex);
            continue;
        }
        temp = temp->prev;
    }

    pthread_mutex_lock(&mutex);
    if (!strcmp(currentClient->login, clientList->login) && currentClient->prev) {
        clientList = clientList->prev;
        deleteUser(currentClient);
    } else {
        deleteUser(currentClient);
    }
    printf("[%s] INFO: Client %llu disconnected from server\n",
           getCurrentTime(), clientSocket);
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
            printf("[%s] WARN: Client %llu - error accept the client\n", getCurrentTime(), client);
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