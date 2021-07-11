#include "ServerGame.h"
#include <thread>

int main(int argc, char **argv)
{
    ServerGame servidor(argv[1], argv[2]);

    std::thread net_thread([&servidor]() { servidor.do_messages(); });
    
    while (true)
    {
        servidor.update();
        servidor.checkCollisions();
        servidor.createBalas();
    }
    return 0;
}
