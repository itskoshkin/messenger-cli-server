/**
 * @authors Stanislav Stoianov, Daria Pavlova
 */

#include "../Services/ServerService.h"
#include "../Services/TimeService.h"

int main() {
    WSADATA wsd;
    if (WSAStartup(MAKEWORD(1, 1), &wsd) == 0) {
        printf("[%s] Connected to socket lib was successful\n", getCurrentTime());
    } else {
        return 1;
    }
    int err = 0;
    if (err = initServer()) {
        return err;
    }
    return 0;
}

