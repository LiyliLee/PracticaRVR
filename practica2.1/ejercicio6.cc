#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <string.h>
#include <time.h>

#include <thread>
#include <chrono>
#include <mutex>

int do_message(int sd)
{
    char buffer[80];
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    struct sockaddr cliente;
    socklen_t clientelen = sizeof(struct sockaddr);

    int bytes = recvfrom(sd, (void *)buffer, 80, 0, &cliente, &clientelen);
    buffer[bytes] = '\0';

    if (bytes == -1)
    {
        return -1;
    }

    for (int i = 0; i < 3; i++)
    {
        std::cout << i << std::endl;
        sleep(1);
    }

    getnameinfo(&cliente, clientelen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

    struct tm *tmp;
    time_t t;
    char outstr[200];

    t = time(NULL);
    tmp = localtime(&t);
    if (tmp == NULL)
    {
        std::cerr << "ERROR: localtime" << std::endl;
        return -1;
    }
    outstr[0] ='\0';

    if (buffer[0] == 't' && buffer[1] == '\n')
    {

        std::cout << " Mensaje["<<std::this_thread::get_id() << "] con " << bytes << " bytes de " << host << ":" << serv << std::endl;
        bytes = strftime(outstr, sizeof(outstr), "%r", tmp);
        sendto(sd, (void *)outstr,bytes, 0, &cliente, clientelen);
    }
    else if (buffer[0] == 'd' && buffer[1] == '\n')
    {
        std::cout << " Mensaje["  <<std::this_thread::get_id() << "] con " << bytes << " bytes de " << host << ":" << serv << std::endl;
        bytes = strftime(outstr, sizeof(outstr), "%F", tmp);
        sendto(sd, (void *)outstr,bytes, 0, &cliente, clientelen);
    }
    else if (buffer[0] == 'q' && buffer[1] == '\n')
    {
        std::cout << " Mensaje[ "  <<std::this_thread::get_id() << "] con " << bytes << " bytes de " << host << ":" << serv << std::endl;
        std::cout << "Saliendo..." << std::endl;
        sendto(sd, "\n", 1, 0, &cliente, clientelen);
        return -1;
    }
    else
    {
        std::cout << " Mensaje["  <<std::this_thread::get_id() << "] con comando no soportado " << buffer << std::endl;
        strncpy(outstr, "Comando no soportado", sizeof(outstr));
        sendto(sd, outstr, sizeof(outstr), 0, &cliente, clientelen);
    }

    return 0;
}

int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct addrinfo *res;
    int rc;

    memset((void *)&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if (rc != 0)
    {
        std::cerr << "ERROR: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    int sd = socket(res->ai_family, res->ai_socktype, 0);

    if (sd == -1)
    {
        std::cerr << "ERROR: creacion socket" << std::endl;
        return -1;
    }

    bind(sd, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);

    std::thread pool[20];

    bool salir =false;
    for (int i = 0; i < 20 && !salir; i++)
    {
        pool[i] = std::thread([&](){
            if(do_message(sd) < 1)
            { salir =true;    }
        });
    }

    for (int j = 0; j < 20 && !salir; j++)
    {
        pool[j].join();
    }

    close(sd);

    return 0;
}