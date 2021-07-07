#ifndef _UTILS_H_
#define _UTILS_H_

#include <netdb.h>
#include <string.h>

void err(char*, char*, int);
int config_socket(struct sockaddr_in*, short);

#endif