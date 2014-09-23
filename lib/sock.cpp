#include "sock.h"

char* Sock_ntop(const sockaddr *sa, socklen_t salen)
{
    char portstr[7];
    static char str[128];

    switch(sa->sa_family)
    {
    case AF_INET:
    {
        struct sockaddr_in *sin = (struct sockaddr_in*) sa;

        if(inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
            return NULL;
        if(ntohs(sin->sin_port) != 0)
        {
            snprintf(portstr, sizeof(portstr), "%d", ntohs(sin->sin_port));
            strcat(str, portstr);
        }
    }
    }
    return str;
}
