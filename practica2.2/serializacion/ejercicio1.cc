#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, 80);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        //
        int32_t dataSize = 80*sizeof(char)+2*sizeof(int16_t);
        alloc_data(dataSize);

        char * dest =_data;
        memcpy(dest,name,80*sizeof(char)); //copiar name a destino (_data), ocupando los primeros 80 puestos de char
        dest += 80*sizeof(char);
        memcpy(dest, &x, sizeof(int16_t));
        dest += sizeof(int16_t);
        memcpy(dest, &y, sizeof(int16_t));
        dest += sizeof(int16_t);
    }

    int from_bin(char * data)
    {
        //
        char * from = data;
        memcpy(name,from,80*sizeof(char));
        from += 80*sizeof(char);
        memcpy(&x, from, sizeof(int16_t));
        from += sizeof(int16_t);
        memcpy(&y, from, sizeof(int16_t));
        return 0;
    }


public:
    char name[80];

    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    int fd = open("./data_jugador", O_CREAT || O_TRUNC || O_RDWR, 0666 );

    // 1. Serializar el objeto one_w
    one_w.to_bin();
    // 2. Escribir la serialización en un fichero
    write(fd, one_w.data(),one_w.size());
    close(fd);
    // 3. Leer el fichero
    fd = open("./data_jugador",O_RDONLY);
    read(fd, one_w.data(),one_w.size());
    // 4. "Deserializar" en one_r
    one_r.from_bin(one_w.data());
    // 5. Mostrar el contenido de one_r
    std::cout<<one_r.name <<"  [ "<<one_r.x<<" , "<<one_r.y <<" ]"<<std::endl;

    return 0;
}

