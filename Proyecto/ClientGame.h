#include "SDLApp.h"
#include "jugador.h"
#include "ObjectInfo.h"
#include <map>
#include "Socket.h"
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400
class ClientGame
{
private:
    
    Jugador *PlayerSelf = nullptr;
    
    std::map<std::string, ObjectInfo> jugadores;
    
    std::map<std::string, ObjectInfo> balas;
    SDLApp *app = nullptr;
    SDLTexture *background = nullptr;
    Socket socket;

    bool isRunning = true;


public:
    ClientGame(const char *s, const char *p, const char *n);
    ~ClientGame();

    void render() const;
    void initGame();
    void net_thread();
    void input_thread();
    void run();
};