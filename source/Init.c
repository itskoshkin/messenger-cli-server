/**
 * @authors Stanislav Stoianov, Daria Pavlova, Sergey Boryaev
 */

#include "Services/ServerService.h"
#include "Services/TimeService.h"

int main() {
    WSADATA wsd;
    if (WSAStartup(MAKEWORD(1, 1), &wsd) == 0) {
        printf("[%s] INFO: Connected to socket library was successful\n", getCurrentTime());
    } else {
        exit(EXIT_FAILURE);
    }
    initServer();
    return 0;
}

