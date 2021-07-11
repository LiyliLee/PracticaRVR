#include <thread>
#include "ClientGame.h"

int main (int argc, char **argv)
{
    ClientGame game(argv[1],argv[2], argv[3]);

    
    std::thread([&game]() { game.net_thread(); }).detach();
    
    
    game.initGame();
    game.run();

    return 0;
}