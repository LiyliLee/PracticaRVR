#pragma once
#include "Vector2D.h"
#include <SDL2/SDL.h>
#include <string>
class SDLTexture;
 
class Jugador{
private:

    std::string nick;
    SDLTexture* texture = nullptr;
    Vector2D pos;
    Vector2D dir;

public: 
    Jugador(const char * n);
    ~Jugador();

    std::string getNick(){return nick;}

    SDLTexture* getPlayerTexture();
    Vector2D getPlayerPos();
    Vector2D getPlayerDir();

    void setPosition(const Vector2D& newPos);
    void setDir(const Vector2D& newDir);
    void setTexture(SDLTexture* newTexture);

};