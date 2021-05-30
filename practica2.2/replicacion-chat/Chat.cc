#include "Chat.h"
#include <memory>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
using namespace std;
void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data

    char * dest = _data;
    memcpy(dest,&type,sizeof(uint8_t));
    dest += sizeof(uint8_t);
    memcpy(dest,&nick , 8*sizeof(char));
    dest += sizeof(8*sizeof(char));
    memcpy(dest,&message , 80*sizeof(char));

}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char * from = _data;
    memcpy(&type,from,sizeof(uint8_t));
    from += sizeof(uint8_t);
    memcpy(&nick, from,  8*sizeof(char));
    from += 8*sizeof(char);
    memcpy(&message, from, 80*sizeof(char));

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */
        ChatMessage msg ;
        Socket * newClient = new Socket(socket);
        socket.recv(msg, newClient);

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)

        if (msg.type == msg.LOGIN)
        {
            std::unique_ptr<Socket> ptrClient =std::make_unique<Socket> (newClient);
            newClient= nullptr;
            clients.push_back(std::move(ptrClient));

        }
        else if (msg.type == msg.LOGOUT)
        {
            bool noEncontrado = true;
            auto it  = clients.begin();
            while(it !=clients.end && noEncontrado)
            {
                if (**it == *newClient) 
                {
                    clients.erase(it);
                    noEncontrado=false;

                }
                else it++;
            }
        }
        else if (msg.type ==msg.MESSAGE)
        {
            for (int i =0; i<clients.size();i++)
            {
                if (*clients[i] == *newClient)
                {
                }
                else
                {
                    socket.send(msg,*clients[i]);
                }
                

            }
        }

    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    // Completar
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        std::string s;
        std::getline(std::cin ,s);
        // Enviar al servidor usando socket
        ChatMessage msg = ChatMessage(nick,s);
        msg.type= ChatMessage::MESSAGE;
        socket.send(msg,socket);
    }
}

void ChatClient::net_thread()
{
    ChatMessage msg;
    Socket *servidor = new Socket(socket);
    while(true)
    {
        //Recibir Mensajes de red
        socket.recv(msg,servidor);
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        std::cout<< msg.nick<<": "<<msg.message<<std::endl;
    }
}

