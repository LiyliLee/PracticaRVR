#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "Serializable.h"
#include "Socket.h"


Socket::Socket(const char * address, const char * port):sd(-1)
{
    //Construir un socket de tipo AF_INET y SOCK_DGRAM usando getaddrinfo.
    //Con el resultado inicializar los miembros sd, sa y sa_len de la clase
    struct addrinfo hints;
    struct addrinfo *servidor;
    char buffer[80];
    int rc;

    memset((void*)&hints,0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;


    rc = getaddrinfo(address,port,&hints, &servidor);

    if (rc!=0)      std::cerr <<"ERROR: getaddrinfo " <<gai_strerror(rc) <<std::endl;

    sd = socket (servidor->ai_family, servidor->ai_socktype,0);
    if (sd ==-1)     std::cerr <<"ERROR: creacion socket" <<std::endl; 

    //sa_len= sizeof(sockaddr);
    sa =*servidor->ai_addr;
    sa_len =servidor->ai_addrlen;

}

int Socket::recv(Serializable &obj, Socket * &sock)
{
    struct sockaddr sa;
    socklen_t sa_len = sizeof(struct sockaddr);

    char buffer[MAX_MESSAGE_SIZE];

    ssize_t bytes = ::recvfrom(sd, buffer, MAX_MESSAGE_SIZE, 0, &sa, &sa_len);

    if ( bytes <= 0 )
    {
        return -1;
    }

    if ( sock != 0 )
    {
        sock = new Socket(&sa, sa_len);
    }

    obj.from_bin(buffer);

    return 0;
}

int Socket::send(Serializable& obj, const Socket& sock)
{
    //Serializar el objeto
    obj.to_bin();
    //Enviar el objeto binario a sock usando el socket sd
    int rc = sendto(sd, obj.data() , obj.size() , 0 ,  &sock.sa, sock.sa_len);
    if (rc == -1)   
    {   std::cerr <<"ERROR: sendoto " <<gai_strerror(rc) <<std::endl;
        return -1;
    }
    //std::cout<<rc<<std::endl;
    return 0;
}

bool operator== (const Socket &s1, const Socket &s2)
{
    bool igual = true;

    sockaddr_in *in1 = (sockaddr_in*) &(s1.sa);
    sockaddr_in *in2 = (sockaddr_in*) &(s2.sa);

    if (in1->sin_family != in2->sin_family ) igual=false;
    if (in1->sin_addr.s_addr != in2->sin_addr.s_addr) igual=false;
    if (in1->sin_port != in2->sin_port ) igual=false;
    //Comparar los campos sin_family, sin_addr.s_addr y sin_port
    //de la estructura sockaddr_in de los Sockets s1 y s2
    //Retornar false si alguno difiere
    return igual;

};

bool operator != (const Socket &s1, const Socket &s2)
{
    return !(s1==s2);
}

std::ostream& operator<<(std::ostream& os, const Socket& s)
{
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    getnameinfo((struct sockaddr *) &(s.sa), s.sa_len, host, NI_MAXHOST, serv,
                NI_MAXSERV, NI_NUMERICHOST);

    os << host << ":" << serv;

    return os;
};

