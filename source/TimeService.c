//
// Created by Stanislav on 27.05.2020.
//

#include "TimeService.h"
#include "time.h"
#include "string.h"

/**
 * @author Stanislav Stoianov
 * @brief service for some logging info
 * @return current time
 */

char * getCurrentTime(){
    time_t mytime = time(NULL);
    char * time_str = ctime(&mytime);
    time_str[strlen(time_str)-1] = '\0';
    return time_str;
}