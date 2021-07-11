#include "ClientGame.h"
#include "Message.h"
#include "Resources.h"
#include "InputManager.h"

void ClientGame::initGame()
{
    //Mandamos mensaje de login
    Message logMsg = Message(MessageType::LOGIN, PlayerSelf);

    if (socket.send(logMsg, socket) == -1)
    {
        std::cout << "Error al enviar el mensaje de login\n";
    }
}

ClientGame::ClientGame(const char *s, const char *p, const char *n) : socket(s, p)
{
    app = SDLApp::GetInstance();

    //Creacion de las texturas
    for (auto &image : Resources::imageRoutes)
    {
        app->getTextureManager()->loadFromImg(image.textureId, app->getRenderer(), image.filename);
    }

    PlayerSelf = new Jugador(n);
    PlayerSelf->setTexture(app->getTextureManager()->getTexture(Resources::TextureId::PlayerSelf));

    background = app->getTextureManager()->getTexture(Resources::TextureId::Background);
}

ClientGame::~ClientGame()
{
    //Destruir al jugador
    delete PlayerSelf;

    //Destruir tb la ventana de SDL
    delete app;
}

void ClientGame::net_thread()
{
    while (isRunning)
    {
        
        Message em;

        socket.recv(em);

        
        std::cout << "Recibido mensaje de: " << em.getNick() << " de tipo " << (int)em.getMessageType() << "\n";
        
        switch (em.getMessageType())
        {
        case MessageType::SIGNIN:
        {
            ObjectInfo p = em.getObjectInfo();
            if (em.getNick() != PlayerSelf->getNick())
                jugadores[em.getNick()] = p;
            else
            {
                PlayerSelf->setPosition(p.pos);
                PlayerSelf->setDir(p.dir);
            }

            break;
        }
        case MessageType::PLAYERINFO:
        {
            ObjectInfo p = em.getObjectInfo();
            jugadores[em.getNick()] = p;
            break;
        }
        case MessageType::BALAINFO:
        {
            ObjectInfo p = em.getObjectInfo();
            balas[em.getNick()] = p;
            

            break;
        }
        case MessageType::PLAYERDEAD:
        {
            
            if (em.getNick() == PlayerSelf->getNick())
            {
                
                em.setMsgType(MessageType::LOGOUT);
                socket.send(em, socket);
                std::cout << "HAS PERDIDO\n";
                isRunning = false;

            }
            
            else
            {
                std::cout << "Ha muerto un jugador: " << em.getNick() << "\n";
                
                jugadores.erase(em.getNick());
            }
            break;
        }

        case MessageType::BALADESTROY:
        {

            balas.erase(em.getNick());

            break;
        }
        }
    }

}

void ClientGame::input_thread()
{

    //Updateamos la instancia del input
    HandleEvents::instance()->update();

    Vector2D playerPos = PlayerSelf->getPlayerPos();
    bool sendMessage = false;
    int velocidad=5;
    
    if (HandleEvents::instance()->isKeyDown(SDL_SCANCODE_W) && playerPos.getY() - velocidad >= 0)
    {
        PlayerSelf->setPosition(Vector2D(playerPos.getX(), playerPos.getY() - velocidad));
        sendMessage = true;
    }
    if (HandleEvents::instance()->isKeyDown(SDL_SCANCODE_S) && playerPos.getY() + 50+ velocidad<= WINDOW_HEIGHT)
    {
        PlayerSelf->setPosition(Vector2D(playerPos.getX(), playerPos.getY() + velocidad));
        sendMessage = true;
    }
    if (HandleEvents::instance()->isKeyDown(SDL_SCANCODE_A) && playerPos.getX() - velocidad >= 0)
    {
        PlayerSelf->setPosition(Vector2D(playerPos.getX() - velocidad, playerPos.getY()));
        sendMessage = true;
    }
    if (HandleEvents::instance()->isKeyDown(SDL_SCANCODE_D) && playerPos.getX() + 50+ velocidad <= WINDOW_WIDTH)
    {
        PlayerSelf->setPosition(Vector2D(playerPos.getX() + velocidad, playerPos.getY()));
        sendMessage = true;
    }

    if (sendMessage && isRunning)
    {
        Message m(MessageType::PLAYERINFO, PlayerSelf);
        socket.send(m, socket);
    }
}

void ClientGame::render() const
{

    //Limpiamos el renderer
    SDL_RenderClear(app->getRenderer());


    background->render({0, 0, app->winWidth_, app->winHeight_}, SDL_FLIP_NONE);
    
    //balas
    SDLTexture* o = app->getTextureManager()->getTexture(Resources::TextureId::Gun);
    for (auto it = balas.begin(); it != balas.end(); ++it)
    {
        ObjectInfo p = (*it).second;
        o->render({(int)p.pos.getX(), (int)p.pos.getY(), 20, 20});
    }

    //jugadores
    PlayerSelf->getPlayerTexture()->render({(int)PlayerSelf->getPlayerPos().getX(),
                                            (int)PlayerSelf->getPlayerPos().getY(),
                                            50,50
                                            });


    SDLTexture *t = app->getTextureManager()->getTexture(Resources::TextureId::PlayerOther);
    for (auto it = jugadores.begin(); it != jugadores.end(); ++it)
    {
        ObjectInfo p = (*it).second;
        t->render({(int)p.pos.getX(), (int)p.pos.getY(),50,50});
    }



    SDL_RenderPresent(app->getRenderer());
}

void ClientGame::run()
{
    while (isRunning)
    {
        input_thread();
        render();
    }
}