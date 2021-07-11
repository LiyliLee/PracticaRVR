#include "Resources.h"

//Para añadir nueva ruta: {TextureId, "./Sources/images/name.png/jpg..."}
vector<Resources::ImageInfo> Resources::imageRoutes {

	
	{ Background, "./Imagenes/background.jpg" },
	{ PlayerSelf, "./Imagenes/airplanes.png" },
	{ PlayerOther, "./Imagenes/airplanes0.png" },
	{ Gun, "./Imagenes/asteroid.png" },

	
};
/*
//Para añadir nueva ruta: {AudioId, "./Sources/audio/name.mp3/wav..."}
vector<Resources::AudioInfo> Resources::audioRoutes{
	//Musica principal

};

vector<Resources::AudioVolume> Resources::audioVolume{
	
};

vector <Resources::FontInfo> Resources::fontsRoutes{
	{// RETRO, "../Sources/fonts/RetroGaming.ttf", GameManager::instance()->getFontSize() },
	//Para añadir nueva ruta: {FontId, "./Sources/fonts/name.ttf", size}
};

vector <Resources::TextMsgInfo> Resources::messages{
	//Para añadir nuevo texto: {TextureId , string "mensaje", { COLOR(0xaaffffff), FontId  }
};*/

