#include "Socket.h"
#include <vector>
#include <map>
#include "ObjectInfo.h"
#include <memory>
#include <mutex>

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400

class ServerGame
{
public:
    ServerGame(const char * s, const char * p);

    /**
     *  Thread principal del servidor recive mensajes en el socket y
     *  lo distribuye a los clientes. Mantiene actualizada la lista de clientes
     */
    void do_messages();
    void checkCollisions();
    void createBalas();
    void update();

private:
    
    std::map<std::string,std::unique_ptr<Socket>> clients;
    std::map<std::string,ObjectInfo > players;
    std::map<std::string,ObjectInfo> balas;
    const float TimeTocreateBala = 5000;
    float initTime = 0;
    const float TimeToUpdate = 100;
    float updateTime = 0;

    int numBalas=0;

    Socket socket;

};