#ifndef SOCK_H
#define SOCK_H

#include "unp.h"

char* Sock_ntop(const struct sockaddr *sa, socklen_t salen);

#endif // SOCK_H
