#include "wrap_func.h"

#include "unp.h"
#include "error.h"
#include "sock.h"

#define	SCTP_MAXLINE	800

#define SERV_MAX_SCTP_STRM      7

#include <iostream>

using namespace std;

//#include <pthread.h>

int Socket(int family, int type, int protocol)
{
    int n;

    if( (n=socket(family, type, protocol)) < 0 )
        err_sys("socket error");
    return n;
}

void Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
    if (bind(fd, sa, salen) < 0)
        err_sys("bind error");
}

void Listen(int fd, int backlog)
{
    char	*ptr;

        /*4can override 2nd argument with environment variable */
    if ( (ptr = getenv("LISTENQ")) != NULL)
        backlog = atoi(ptr);

    if (listen(fd, backlog) < 0)
        err_sys("listen error");
}

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
    int		n;

again:
    if ( (n = accept(fd, sa, salenptr)) < 0) {
#ifdef	EPROTO
        if (errno == EPROTO || errno == ECONNABORTED)
#else
        if (errno == ECONNABORTED)
#endif
            goto again;
        else
            err_sys("accept error");
    }
    return(n);
}

void Close(int fd)
{
    if (close(fd) == -1)
        err_sys("close error");
}

void Write(int fd, void *ptr, size_t nbytes)
{
    if (write(fd, ptr, nbytes) != nbytes)
        err_sys("write error");
}

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
       struct timeval *timeout)
{
    int		n;

    if ( (n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0)
        err_sys("select error");
    return(n);		/* can return 0 on timeout */
}

ssize_t Read(int fd, void *ptr, size_t nbytes)
{
    ssize_t		n;

    if ( (n = read(fd, ptr, nbytes)) == -1)
        err_sys("read error");
    return(n);
}

int Poll(struct pollfd *fdarray, unsigned long nfds, int timeout)
{
    int		n;

    if ( (n = poll(fdarray, nfds, timeout)) < 0)
        err_sys("poll error");

    return(n);
}

void Getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlenptr)
{
    if (getsockopt(fd, level, optname, optval, optlenptr) < 0)
        err_sys("getsockopt error");
}

struct addrinfo * host_serv(const char *host, const char *serv, int family, int socktype)
{
    int				n;
    struct addrinfo	hints, *res;

    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_flags = AI_CANONNAME;	/* always return canonical name */
    hints.ai_family = family;		/* AF_UNSPEC, AF_INET, AF_INET6, etc. */
    hints.ai_socktype = socktype;	/* 0, SOCK_STREAM, SOCK_DGRAM, etc. */

    if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
        return(NULL);

    return(res);	/* return pointer to first on linked list */
}

int tcp_connect(const char *host, const char *serv)
{
    int				sockfd, n;
    struct addrinfo	hints, *res, *ressave;

    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
        err_quit("tcp_connect error for %s, %s: %s",
                 host, serv, gai_strerror(n));
    ressave = res;

    do {
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd < 0)
            continue;	/* ignore this one */

        if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
            break;		/* success */

        Close(sockfd);	/* ignore this one */
    } while ( (res = res->ai_next) != NULL);

    if (res == NULL)	/* errno set from final connect() */
        err_sys("tcp_connect error for %s, %s", host, serv);

    freeaddrinfo(ressave);

    return(sockfd);
}

int Tcp_connect(const char *host, const char *serv)
{
    return(tcp_connect(host, serv));
}

void Getpeername(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
    if (getpeername(fd, sa, salenptr) < 0)
        err_sys("getpeername error");
}

char * sock_ntop_host(const struct sockaddr *sa, socklen_t salen)
{
    static char str[128];		/* Unix domain is largest */

    switch (sa->sa_family) {
    case AF_INET: {
        struct sockaddr_in	*sin = (struct sockaddr_in *) sa;

        if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
            return(NULL);
        return(str);
    }

#ifdef	IPV6
    case AF_INET6: {
        struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *) sa;

        if (inet_ntop(AF_INET6, &sin6->sin6_addr, str, sizeof(str)) == NULL)
            return(NULL);
        return(str);
    }
#endif

#ifdef	AF_UNIX
    case AF_UNIX: {
        struct sockaddr_un	*unp = (struct sockaddr_un *) sa;

            /* OK to have no pathname bound to the socket: happens on
               every connect() unless client calls bind() first. */
        if (unp->sun_path[0] == 0)
            strcpy(str, "(no pathname bound)");
        else
            snprintf(str, sizeof(str), "%s", unp->sun_path);
        return(str);
    }
#endif

#ifdef	HAVE_SOCKADDR_DL_STRUCT
    case AF_LINK: {
        struct sockaddr_dl	*sdl = (struct sockaddr_dl *) sa;

        if (sdl->sdl_nlen > 0)
            snprintf(str, sizeof(str), "%*s",
                     sdl->sdl_nlen, &sdl->sdl_data[0]);
        else
            snprintf(str, sizeof(str), "AF_LINK, index=%d", sdl->sdl_index);
        return(str);
    }
#endif
    default:
        snprintf(str, sizeof(str), "sock_ntop_host: unknown AF_xxx: %d, len %d",
                 sa->sa_family, salen);
        return(str);
    }
    return (NULL);
}

char * Sock_ntop_host(const struct sockaddr *sa, socklen_t salen)
{
    char	*ptr;

    if ( (ptr = sock_ntop_host(sa, salen)) == NULL)
        err_sys("sock_ntop_host error");	/* inet_ntop() sets errno */
    return(ptr);
}

struct addrinfo * Host_serv(const char *host, const char *serv, int family, int socktype)
{
    int				n;
    struct addrinfo	hints, *res;

    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_flags = AI_CANONNAME;	/* always return canonical name */
    hints.ai_family = family;		/* 0, AF_INET, AF_INET6, etc. */
    hints.ai_socktype = socktype;	/* 0, SOCK_STREAM, SOCK_DGRAM, etc. */

    if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
        err_quit("host_serv error for %s, %s: %s",
                 (host == NULL) ? "(no hostname)" : host,
                 (serv == NULL) ? "(no service name)" : serv,
                 gai_strerror(n));

    return(res);	/* return pointer to first on linked list */
}

int tcp_listen(const char *host, const char *serv, socklen_t *addrlenp)
{
    int				listenfd, n;
    const int		on = 1;
    struct addrinfo	hints, *res, *ressave;

    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
        err_quit("tcp_listen error for %s, %s: %s",
                 host, serv, gai_strerror(n));
    ressave = res;

    do {
        listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (listenfd < 0)
            continue;		/* error, try next one */

        Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
            break;			/* success */

        Close(listenfd);	/* bind error, close and try next one */
    } while ( (res = res->ai_next) != NULL);

    if (res == NULL)	/* errno from final socket() or bind() */
        err_sys("tcp_listen error for %s, %s", host, serv);

    Listen(listenfd, LISTENQ);

    if (addrlenp)
        *addrlenp = res->ai_addrlen;	/* return size of protocol address */

    freeaddrinfo(ressave);

    return(listenfd);
}

int Tcp_listen(const char *host, const char *serv, socklen_t *addrlenp)
{
    return(tcp_listen(host, serv, addrlenp));
}

void Setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
    if (setsockopt(fd, level, optname, optval, optlen) < 0)
        err_sys("setsockopt error");
}

void * Malloc(size_t size)
{
    void	*ptr;

    if ( (ptr = malloc(size)) == NULL)
        err_sys("malloc error");
    return(ptr);
}

