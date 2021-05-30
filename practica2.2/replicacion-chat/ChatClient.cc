#include <thread>
#include "Chat.h"

int main(int argc, char **argv)
{
    ChatClient ec(argv[1], argv[2], argv[3]);
    //std::cout << "creado cliente" <<argv[3]<<std::endl;
    std::thread net_thread([&ec](){ ec.net_thread(); });
    //std::cout << "cliente ejecuta net_thread" <<std::endl;
    ec.login();
    //std::cout << "cliente hace login" <<std::endl;
    ec.input_thread();
    std::cout << "cliente manda mensaje" <<std::endl;
}

