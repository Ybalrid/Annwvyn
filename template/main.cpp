//Includes
#include <Annwvyn.h>
using namespace Annwvyn; //All Annwvyn components are here 

AnnMain()
{
	//Initialize the engine
	new AnnEngine("Your game");
	//Load your ressources here

	AnnEngine::getSingletonPtr()->initResources();
	AnnEngine::getSingletonPtr()->oculusInit();

	//Do the other initialization here

	do
	{
		//Put your gameplay code here
	}while(AnnEngine::getSingletonPtr()->refresh());
	//destroy the engine
	delete AnnEngine::getSingletonPtr();
	return EXIT_SUCCESS;
}

