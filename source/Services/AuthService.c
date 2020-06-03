/**
 * @authors Stanislav Stoianov
 */

#include "DatabaseService.h"
#include "AuthService.h"

bool processingUser(bool isRegistration, char *login, char *password) {
    if (isRegistration) return signUp(login, password);
    else return signIn(login, password);
}
