#include "DatabaseService.h"
#include "AuthService.h"

bool processingUser(bool isRegistration, char *login, char *password) {
    if (isRegistration) return signIn(login, password);
    else return signUp(login, password);
}
