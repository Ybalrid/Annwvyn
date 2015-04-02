//Includes
#include <Annwvyn.h>

//Namespaces
using namespace std;
using namespace Annwvyn; //All Annwvyn components are here 


AnnMain()
{
	AnnEngine* GameEngine = new AnnEngine("My Game");

	//Load your ressources here

	GameEngine->initResources();
	GameEngine->oculusInit();

	//Do the other initialization here

	while(!GameEngine->requestStop())
	{
		//Put your gameplay code here
		GameEngine->refresh();
	}

	delete GameEngine;
	return 0;
}

