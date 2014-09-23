#ifndef WRAP_FUNC_H
#define WRAP_FUNC_H

#include "unp.h"

#define	MAXFD	64

int Socket(int family, int type, int protocol);                             //

void Bind(int fd, const struct sockaddr *sa, socklen_t salen);              //

void Listen(int fd, int backlog);                                           //

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);               //

void Close(int fd);                                                         //

void Write(int fd, void *ptr, size_t nbytes);                               //

ssize_t Read(int fd, void *ptr, size_t nbytes);                             //

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
       struct timeval *timeout);                                            //

int tcp_connect(const char *host, const char *serv);

int Tcp_connect(const char *host, const char *serv);

void Getpeername(int fd, struct sockaddr *sa, socklen_t *salenptr);

struct addrinfo * Host_serv(const char *host, const char *serv, int family, int socktype);

struct addrinfo * host_serv(const char *host, const char *serv, int family, int socktype);

int tcp_listen(const char *host, const char *serv, socklen_t *addrlenp);

int Tcp_listen(const char *host, const char *serv, socklen_t *addrlenp);

void Setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen);

void * Malloc(size_t size);

#endif // WRAP_FUNC_H
