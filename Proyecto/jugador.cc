#include "jugador.h"
#include "Message.h"
#include "SDLTexture.h"

Jugador::Jugador(const char *n) : nick(n)
{
}

Jugador::~Jugador()
{
}
void Jugador::setPosition(const Vector2D &newPos)
{
    pos = newPos;
}

void Jugador::setDir(const Vector2D &newDir)
{
    dir = newDir;
}

void Jugador::setTexture(SDLTexture *newTexture)
{
    texture = newTexture;
}

SDLTexture *Jugador::getPlayerTexture()
{
    return texture;
}

Vector2D Jugador::getPlayerPos()
{
    return pos;
}

Vector2D Jugador::getPlayerDir()
{
    return dir;
}
