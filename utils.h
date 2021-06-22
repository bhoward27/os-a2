#ifndef _UTILS_H_
#define _UTILS_H_

#include <netdb.h>
#include <string.h>

void sleep_usec(long usec);
void sleep_msec(long msec);
void err(char*, char*, int);
int config_socket(struct sockaddr_in*, short);

#endif