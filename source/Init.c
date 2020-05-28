/**
 * @authors Stanislav Stoianov, Daria Pavlova
 */

#include "Services/ServerService.h"
#include "Services/TimeService.h"

int main() {
    WSADATA wsd;
    if (WSAStartup(MAKEWORD(1, 1), &wsd) == 0) {
        printf("[%s] INFO: Connected to socket lib was successful\n", getCurrentTime());
    } else {
        return 1;
    }
    int err = 0;
    err = initServer();
    if (err) {
        return err;
    }
    return 0;
}

