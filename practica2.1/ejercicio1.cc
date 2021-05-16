#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <string.h>

int main (int argc, char** argv)
{
    struct addrinfo hints;
    struct addrinfo *res ,*i;
    int rc;

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
    


    memset((void*)&hints,0, sizeof(struct addrinfo));



    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = 0;

    
    /* 
    int getaddrinfo(const char *node, const char *service,
                       const struct addrinfo *hints,
                       struct addrinfo **res);
    */
   
    rc = getaddrinfo(argv[1],argv[2],&hints, &res);


    if (rc!=0)
    {
        std::cerr <<"ERROR: " <<gai_strerror(rc) <<std::endl;
        return -1;
    }

    struct in_addr addr;

    if (inet_aton(argv[1], &addr) != 0) {
        getnameinfo(res->ai_addr, res->ai_addrlen, host, NI_MAXHOST, serv, NI_MAXSERV,  NI_NAMEREQD);

        std::cout<<host <<std::endl;
    }
    else 
    {
        for (i = res; i!= NULL; i = i->ai_next)
        {
         /* 
        int getnameinfo(const struct sockaddr *sa, socklen_t salen,
                    char *host, size_t hostlen,
                       char *serv, size_t servlen, int flags);
        */
            getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, serv, NI_MAXSERV,  NI_NUMERICHOST);

            std::cout<<host <<" "<<i->ai_family <<" " <<i->ai_socktype<<std::endl;
        }
    }

    freeaddrinfo(res);
       
    return 0;
}