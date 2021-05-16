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
    struct addrinfo *servidor;
    char buffer[80];
    int rc;

    memset((void*)&hints,0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;


    rc = getaddrinfo(argv[1],argv[2],&hints, &servidor);


    if (rc!=0)
    {
        std::cerr <<"ERROR: " <<gai_strerror(rc) <<std::endl;
        return -1;
    }

    struct sockaddr cliente ;
    socklen_t       clientelen = sizeof (struct sockaddr);


    int sd = socket (servidor->ai_family, servidor->ai_socktype,0);

    if (sd ==-1)
    {
        std::cerr <<"ERROR: creacion socket" <<std::endl;
        return -1;
    }


    bind(sd,&cliente, clientelen);

    //Connect to remote server
	if (connect(sd, servidor->ai_addr,servidor->ai_addrlen) ==-1)
	{
		std::cerr <<"ERROR: coneccion" <<std::endl;
		return -1;
	} 
 
    int bytes ;
    while (true)
    {  
        buffer[0]='\0';
        std::cin>>buffer;
        bytes = sendto(sd,buffer,sizeof(buffer),0,servidor->ai_addr,servidor->ai_addrlen);
        buffer[0]='\0';
        bytes = recvfrom(sd,(void *) buffer,80, 0, servidor->ai_addr, &servidor->ai_addrlen);
        
        if (bytes ==-1) return -1;

        buffer[bytes]='\0';

        std::cout<<buffer<<std::endl;
    }
    freeaddrinfo(servidor);
    close (sd);

    return 0;
       
}