//Includes
#include <Annwvyn.h>

//Namespaces
using namespace std;
using namespace Annwvyn; //All Annwvyn components are here 


AnnMain()
{
	AnnEngine* GameEngine = new AnnEngine("My Game");

	GameEngine->initRessources();

	GameEngine->oculusInit(true);

	while(!GameEngine->requestStop())
	{
		GameEngine->refresh();
	}

	delete GameEngine;
	return 0;
}

