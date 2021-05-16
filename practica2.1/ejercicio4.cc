#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <string.h>
#include <time.h>

int main (int argc, char** argv)
{
    struct addrinfo hints;
    struct addrinfo *res ;
    int rc;

    memset((void*)&hints,0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    rc = getaddrinfo(argv[1],argv[2],&hints, &res);

    if (rc!=0)
    {
        std::cerr <<"ERROR: " <<gai_strerror(rc) <<std::endl;
        return -1;
    }

    int sd = socket (res->ai_family, res->ai_socktype,0);

    if (sd ==-1)
    {
        std::cerr <<"ERROR: creacion socket" <<std::endl;
        return -1;
    }

    bind(sd,(struct sockaddr *) res->ai_addr, res->ai_addrlen);
    //int listen(int sockfd, int backlog);
    int cl = listen(sd,5); 
    if (cl == -1) return -1;
      freeaddrinfo(res);

    char buffer[80];
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
    struct sockaddr cliente;
    socklen_t       cliente_len = sizeof (struct sockaddr);
    int cliente_sd ;

    //int accept(int socket, struct sockaddr *restrict address,socklen_t *restrict address_len);
    cliente_sd = accept(sd,(struct sockaddr *) &cliente, &cliente_len);
        
    getnameinfo((struct sockaddr *) &cliente, cliente_len, host, NI_MAXHOST, serv, NI_MAXSERV,  NI_NUMERICHOST | NI_NUMERICSERV);
        
    std::cout <<"Conexión desde "<<host <<"  " <<serv <<std::endl;

    while (true)
    {
        int bytes= recv(cliente_sd, (void *) buffer, 80, 0);
         //int bytes = recvfrom(sd,(void *) buffer,80, 0, &cliente, &cliente_len);
       
        buffer[bytes]='\0';
        if (bytes == -1)  { return -1; }
        if (bytes == 0 || buffer[0] =='Q') {std::cout <<"Conexión terminada" <<std::endl; return 0;}

        send(cliente_sd, buffer, bytes, 0);       
    
    }
  
    close (sd);

    return 0;
       
}