DEPS = Vector2D.h  PlayerInfo.h Resources.h InputManager.h SDLTexture.h TextureManager.h Socket.h Message.h ServerGame.h SDLApp.h jugador.h ClientGame.h  ChatClient.h ChatServer.h

SERVER = Vector2D.o  Resources.o Socket.o jugador.o Message.o ServerGame.o
CLIENT = Vector2D.o  Resources.o InputManager.o SDLTexture.o TextureManager.o Socket.o jugador.o Message.o SDLApp.o  ClientGame.o 
#LINKER_FLAGS specifies the libraries we're linking against
LIBS =  -lpthread 
LIBSSDL=-lSDL2 -lSDL2_image -lSDL2_ttf
%.o: %.cc $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)
all: cs cc
cs: $(SERVER) ChatServer.o
	g++ -o $@ $^ $(CFLAGS) `pkg-config --cflags --libs sdl2` $(LIBS) $(LIBSSDL)
cc: $(CLIENT) ChatClient.o
	g++ -o $@ $^ $(CFLAGS) `pkg-config --cflags --libs sdl2` $(LIBS) $(LIBSSDL) 
.PHONY: clean
clean:
	rm -f *.o server client