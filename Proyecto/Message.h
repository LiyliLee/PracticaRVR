#pragma once
#include "Serializable.h"
#include  <string>
#include <SDL2/SDL.h>
#include "ObjectInfo.h"
class Jugador;


enum class MessageType: int8_t
{
    LOGIN   = 0,
    PLAYERINFO = 1,
    SIGNIN= 2,
    BALADESTROY = 3,
    BALAINFO = 4,
    PLAYERDEAD = 5,
    LOGOUT  = 6,
    SHOOT =7,
    UNDEFINED = 8
};

class Message: public Serializable{
protected:
    size_t messageSize = sizeof(MessageType);
    MessageType type;
    std::string nick;
    ObjectInfo objectInfo;
   

    //Metodos para la serializacion de nuestros mensajes
    void serializeTypeNick();
    void serializeObjectInfo();

    //Metodos para la construccion de los mensajes recibidos
    void constructTypeNick(char *bobj);
    void constructObjectInfo(char *bobj);

public:
    
    Message();
    Message(MessageType type_ ,Jugador* player_);
    
    virtual ~Message();

    virtual void to_bin();
    virtual int from_bin(char * bobj);

    size_t getMessageSize();
    MessageType getMessageType();
    std::string getNick();
    ObjectInfo getObjectInfo()const{
        return objectInfo;
    }
    
    void setNick(std::string newNick);
    void setObjectInfo(const ObjectInfo& info){
        objectInfo= info;
    }
    void setMsgType(MessageType type);

};