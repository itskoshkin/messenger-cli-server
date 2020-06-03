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
Client* clientList;

void *clientHandler(void *param) {
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
            /*
             * TODO removing connection
             */
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

            /*
             * TODO add connection information to clients_list
             */

            currentClient = addUser(clientList, clientSocket, login);
            currentClient = connectNewUser(clientList, clientSocket, login);
            if (clientList->next) clientList = clientList->next;

            printf("[%s] INFO: Client %llu successfully added to the mailing list\n", getCurrentTime(), (SOCKET) param);
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

            /*
             * TODO removing connection information from clients_list
             */
            deleteUser(currentClient);

            printf("[%s] INFO: Client %llu successfully removed from the mailing list\n",
                   getCurrentTime(), (SOCKET) param);
            pthread_mutex_unlock(&mutex);

            return (void *) 2;
        }

    } while (!isOk);


    /**
     * TODO Notify all users about a new user
     */

    while (clientList.next) {
        ret = send(clientList.client, transmit, 1024, 0);
        if (ret == SOCKET_ERROR) {
            pthread_mutex_lock(&mutex);
            printf("[%s] ERROR: Client %llu did not receive a message about new user\n",
                   getCurrentTime(), (SOCKET) param);
            /*
             * TODO removing connection information from clients_list
             */
            deleteUser(currentClient);

            printf("[%s] INFO: Client %llu successfully removed from the mailing list\n",
                   getCurrentTime(), (SOCKET) param);
            pthread_mutex_unlock(&mutex);
        }
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
            /*
             * TODO removing connection information from clients_list
             */
            deleteUser(currentClient);

            printf("[%s] INFO: Client %llu successfully removed from the mailing list\n",
                   getCurrentTime(), (SOCKET) param);
            pthread_mutex_unlock(&mutex);
            break;
        }

        //прислать всем остальным соединениям сообщение полученное от пользователя
        while (clientList.next) {
            send(clientList.client, receive, 1024, 0);
        }
    }

    /*
     * FIXME Уведомить всех клиентов о том, что это пользователь отключился
     */

    sprintf(transmit, "%s leaving", login);
    while (clientList.next) {
        send(clientList.next, transmit, 1024, 0);
    }

    /**
     * FIXME Выход из потока...
     */

    pthread_mutex_lock(&mutex);
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