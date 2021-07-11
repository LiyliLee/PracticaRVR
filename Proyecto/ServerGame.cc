#include "ServerGame.h"
#include "Message.h"
#include <memory>
#include <ctime>
#include <cstdlib>
#include <SDL2/SDL.h>
#include <list>

ServerGame::ServerGame(const char *s, const char *p) : socket(s, p)
{
     
    srand(std::time(0));
    initTime = SDL_GetTicks();
    updateTime=SDL_GetTicks();
}

void ServerGame::do_messages()
{

    if (socket.bind() == -1)
    {
        std::cout << "Address Incorrecto\n";
    }

    while (true)
    {
        Message cm;
        Socket *s = nullptr;

        //Esperamos recibir un mensaje de cualquier socket
        if (socket.recv(cm, s) == -1)
        {
            std::cout << "Error al recibir el mensaje\n";
        }

        //Recibir Mensajes en y en función del tipo de mensaje
        switch (cm.getMessageType())
        {
        case MessageType::LOGIN:
        {
            
            //Lo añadimos a la lista de clientes convirtiendo el socket en un unique_ptr y usando move
            clients[cm.getNick()] = std::move(std::make_unique<Socket>(*s));

            //Informacion del jugador
            ObjectInfo n;
            n.dir= Vector2D(0,0);
            n.pos = Vector2D(rand() % (WINDOW_WIDTH - 50), rand() % (WINDOW_HEIGHT - 50));

    
            players[cm.getNick()] = n; 

            Message newConnection = Message();
            newConnection.setMsgType(MessageType::SIGNIN);
            newConnection.setNick(cm.getNick());
            newConnection.setObjectInfo(players[cm.getNick()]);


            for (auto it = clients.begin(); it != clients.end(); it++)
            {
                
                socket.send(newConnection, *((*it).second.get()));
            }
            for (auto it = players.begin(); it != players.end(); ++it)
            {
                if ((*it).first != cm.getNick())
                {
                    newConnection.setNick((*it).first);
                    newConnection.setObjectInfo((*it).second);
                    socket.send(newConnection, *s);
                }
            }

            for (auto it = balas.begin(); it != balas.end(); ++it)
            {
                newConnection.setMsgType(MessageType::SHOOT);
                newConnection.setNick((*it).first);
                newConnection.setObjectInfo((*it).second);
                socket.send(newConnection, *s);
            }

            break;
        }

        case MessageType::LOGOUT:
        {
            auto it = clients.begin();

            while (it != clients.end() && (*((*it).second.get()) != *s))
                ++it;

            if (it == clients.end())
                std::cout << "El jugador ya se había desconectado previamente\n";
            else
            {
                std::cout << "Jugador desconectado: " << cm.getNick() << "\n";
                clients.erase((*it).first);               
                Socket *delSock = (*it).second.release(); 
                delete delSock;                           
            }
            break;
        }

        case MessageType::PLAYERINFO:
        {
            //Actualizamos la posición en la que se encuentra dicho jugador en la memoria del servidor
            players[cm.getNick()] = cm.getObjectInfo();

            //Avisar a todos los jugadores conectados que alguien se ha movido
            for (auto it = clients.begin(); it != clients.end(); it++)
            {
                if (*((*it).second.get()) != *s) //Excepto a la persona que ha enviado el mensaje
                {
                    socket.send(cm, (*((*it).second.get())));
                }
            }

            break;
        }
        default:
            break;
        }

        //En el metodo recieve si recibimos como parametro un puntero hacemos un new con el socket creado
        //A excecpción del que recibamos un mensaje de login, siempre querremos borrar ese objeto
        //puesto que ya lo almacenamos en el diccionario clients y no borrarlo dejaria basura
        if(cm.getMessageType() != MessageType::LOGIN){
            delete s;
        }

    }
}

void ServerGame::checkCollisions()
{

    std::list<std::map<std::string, ObjectInfo>::iterator> playersToErase;
    std::list<std::map<std::string, ObjectInfo>::iterator> balasToErase;

 

    for (auto it = players.begin(); it != players.end(); ++it)
    {
        for (auto it2 = balas.begin(); it2 != balas.end(); ++it2)
        {
            SDL_Rect a, b;
            ObjectInfo ap = (*it).second, bp = (*it2).second;
            a = {(int)ap.pos.getX(), (int)ap.pos.getY(),50,50};
            b = {(int)bp.pos.getX(), (int)bp.pos.getY(), 20,20};

            //Si se solapan y el tamano de l objeto es menor que el del jugador nos lo comemos
            if (SDL_HasIntersection(&a, &b))
            {
                //Lo metemos en una lista para borrarlo posteriormente
                balasToErase.push_back(it2);
                playersToErase.push_back(it);
            }
        }
    }

    for(auto it3 = balas.begin(); it3 != balas.end(); ++it3)
    {
        ObjectInfo bala = (*it3).second;
        if (bala.pos.getX() >WINDOW_WIDTH-20 || bala.pos.getX() <0
        ||bala.pos.getY()> WINDOW_HEIGHT-20|| bala.pos.getY() <0)
        {
            balasToErase.push_back(it3);
        }
    }

    for (auto player : playersToErase)
    {
        Message cm;
        cm.setMsgType(MessageType::PLAYERDEAD);
        cm.setObjectInfo((*player).second);
        cm.setNick((*player).first);


        //Avisamos a todos los clientes que un jugador va a ser borrado
        for (auto i = clients.begin(); i != clients.end(); ++i)
        {
            socket.send(cm, (*((*i).second.get())));
        }

        players.erase((*player).first);
    }

    for (auto bala : balasToErase)
    {
        Message cm;
        cm.setMsgType(MessageType::BALADESTROY);
        cm.setObjectInfo((*bala).second);
        cm.setNick((*bala).first);
        
        //Avisamos a todos los clientes que un jugador va a ser borrado
        for (auto i = clients.begin(); i != clients.end(); ++i)
        {
            socket.send(cm, (*((*i).second.get())));
        }

        balas.erase((*bala).first);
    }
}

void ServerGame::createBalas()
{
    if (SDL_GetTicks() - initTime > TimeTocreateBala)
    {
        for (int i =0; i<players.size()*2; i++)
        {

        
        //creo el objeto
        ObjectInfo bala;
        
        int type = rand()%2;
        if (type ==0)
        {
            bala.dir = Vector2D(-1,0);
            bala.pos = Vector2D(WINDOW_WIDTH-20, rand() % (WINDOW_HEIGHT - 20));
        }
        else if (type ==1)
        {
            bala.dir = Vector2D(1,0);
            bala.pos = Vector2D(0, rand() % (WINDOW_HEIGHT - 20));

        }
        std::string num = std::to_string(numBalas);
        num.resize(12);
        balas[num] = bala;

        numBalas++;
        
            
        //mandar mensaje de objeto nuevo
        Message cm;
        cm.setMsgType(MessageType::SHOOT);
        cm.setObjectInfo((bala));
        cm.setNick((num));
        //Avisamos a todos los clientes que un objeto ha sido creado
        for (auto i = clients.begin(); i != clients.end(); ++i)
        {
            socket.send(cm, (*((*i).second.get())));
        }
        }

        initTime = SDL_GetTicks();
    }
}
void ServerGame::update()
{
    if (SDL_GetTicks() - updateTime > TimeToUpdate)
    {
        for(auto it = balas.begin(); it != balas.end(); ++it)
        {
            ObjectInfo bala = (*it).second;
            
            

            bala.pos.setX(bala.pos.getX()+bala.dir.getX()*5);
            bala.pos.setY(bala.pos.getY()+bala.dir.getY()*5);


            (*it).second = bala;
        

        //mandar mensaje de objeto nuevo
        Message cm;
        cm.setMsgType(MessageType::BALAINFO);
        cm.setObjectInfo((bala));
        cm.setNick((*it).first);
        //Avisamos a todos los clientes que un objeto ha sido creado
        for (auto i = clients.begin(); i != clients.end(); ++i)
        {
            socket.send(cm, (*((*i).second.get())));
        }
        }

        updateTime= SDL_GetTicks();
    }

}