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
    hints.ai_socktype = SOCK_DGRAM;

    
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

    /* int socket(int domain, int type, int protocol); */
    int sd = socket (res->ai_family, res->ai_socktype,0);

    if (sd ==-1)
    {
        std::cerr <<"ERROR: creacion socket" <<std::endl;
        return -1;
    }

    bind(sd,res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);

    while (true)
    {
        char buffer[80];
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        struct sockaddr cliente;
        socklen_t       clientelen = sizeof (struct sockaddr);

        /*
        ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                        struct sockaddr *src_addr, socklen_t *addrlen);
                        */
        
        int bytes = recvfrom(sd,(void *) buffer,80, 0, &cliente, &clientelen);
        buffer[bytes]='\0';

        if (bytes == -1)  {  return -1; }
        
        getnameinfo(&cliente, clientelen, host, NI_MAXHOST, serv, NI_MAXSERV,  NI_NUMERICHOST | NI_NUMERICSERV);

       /*
        ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
                      const struct sockaddr *dest_addr, socklen_t addrlen);
        */
        
        struct tm *tmp;
        time_t t;
        char outstr[200];

        t= time(NULL);
        tmp = localtime(&t);
        if (tmp == NULL )
        {
            std::cerr <<"ERROR: localtime" <<std::endl;
            return -1;
        }


        if (buffer[0] == 't'  )
        {
            
            std::cout<<bytes<<" bytes de "<< host <<":" << serv <<std::endl;
            bytes = strftime(outstr, sizeof(outstr), "%r", tmp);
            sendto(sd,(void *)outstr,bytes,0,&cliente,clientelen);
        }
        else if (buffer[0] == 'd' )
        {
            std::cout<<bytes<<" bytes de "<< host <<":" << serv <<std::endl;
            bytes = strftime(outstr, sizeof(outstr), "%F", tmp);
            sendto(sd,(void *)outstr,bytes,0,&cliente,clientelen);
        
        }
        else if (buffer[0] == 'q')
        {
             std::cout<<bytes<<" bytes de "<< host <<":" << serv <<std::endl;
             std::cout<<"Saliendo..."<<std::endl;
            sendto(sd,"\n",1,0,&cliente,clientelen);
             return 0;
        }
        else
        {
            std::cout<<"Comando no soportado "<< buffer ;
            sendto(sd,"\n",1,0,&cliente,clientelen);
        }
    
    }

    close (sd);

    return 0;
       
}