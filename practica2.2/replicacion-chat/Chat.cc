#include "Chat.h"
#include "Socket.h"
#include <memory>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>


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
            //std::cout << "recibir cliente login " <<std::endl;
            std::unique_ptr<Socket> ptrClient (newClient);
            //std::cout << "convertir newClient en puntero unico" <<std::endl;

            clients.push_back(std::move(ptrClient));
            //std::cout << "meter al vector el puntero unico " <<std::endl;

        }
        else if (msg.type == msg.LOGOUT)
        {
            std::cout << "recibir cliente logout " << *newClient <<std::endl;
            bool noEncontrado = true;
            auto it  = clients.begin();
            while(it !=clients.end() && noEncontrado)
            {
                if (**it == *newClient) 
                {
                    clients.erase(it);
                    noEncontrado=false;

                }
                else it++;
            }

            if(it == clients.end()){
		        std::cout << "no existe este cliente" <<std::endl;
		        return;
            }
        }
        else if (msg.type ==msg.MESSAGE)
        {
            std::cout << "recibir clientes mensaje " << *newClient <<std::endl;
            for (int i =0; i<clients.size();i++)
            {
                if (*clients[i] != *newClient)
                { 
                    socket.send(msg,*clients[i]);
                    std::cout<<"mandar mensaje a todo el mundo"<<std::endl;
                }
                

            }
        }
        else 
        {
            std::cerr << "mensaje raro" << std::endl;
        }

        newClient = nullptr;

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
    //std::cout<<"mandar login"<<std::endl;
    
}

void ChatClient::logout()
{
    // Completar
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;

    socket.send(em, socket);
    //std::cout<<"mandar logout"<<std::endl;
}

void ChatClient::input_thread()
{
    
    while (true)
    {
        // Leer stdin con std::getline
       
	   
        char *line = NULL;
        size_t len = 0;
        ssize_t read;

        read = getline(&line, &len, stdin);
       
        std::string msg(line);

         free(line);
        // Enviar al servidor usando socket
        ChatMessage em(nick,msg);
        //std::cout <<"input_thread"<<std::endl;
        em.type= ChatMessage::MESSAGE;
        socket.send(em,socket);
        std::cout <<"sendMenssage" <<std::endl;
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

