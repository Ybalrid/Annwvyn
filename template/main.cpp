//Includes
#include <Annwvyn.h>

//Namespaces
using namespace std;
using namespace Annwvyn; //All Annwvyn components are here 


AnnMain()
{
	AnnEngine* GameEngine = new AnnEngine("My Game");

	GameEngine->initResources();

	GameEngine->oculusInit();



	while(!GameEngine->requestStop())
	{
		GameEngine->refresh();
	}

	delete GameEngine;
	return 0;
}

