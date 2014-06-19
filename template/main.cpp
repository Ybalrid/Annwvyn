//Includes
#include <Annwvyn.h>

//Namespaces
using namespace std;
using namespace Annwvyn; //All Annwvyn components are here 

//Main definition : 
#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
//On windows : 
#define WIN32_LEAN_AND_MEAN 
#include "windows.h"
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)

#else
//On others :
int main(int argc, char** argv)
#endif
{
	AnnEngine* GameEngine = new AnnEngine("My Game");

	GameEngine->loadZip("media/OgreOculus.zip");
	GameEngine->initRessources();

	GameEngine->oculusInit();

	while(!GameEngine->requestStop())
	{
		GameEngine->refresh();
	}

	delete GameEngine;
	return 0;
}

