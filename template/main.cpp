//Includes
#include <Annwvyn.h>
using namespace Annwvyn; //All Annwvyn components are here 

AnnMain()
{
	//Initialize the engine
	new AnnEngine("Your game");
	//Load your ressources here

	AnnEngine::Instance()->initResources();
	AnnEngine::Instance()->oculusInit();

	//Do the other initialization here

	do
	{
		//Put your gameplay code here
	}while(AnnEngine::Instance()->refresh());

	//destroy the engine
	delete AnnEngine::Instance();
	return EXIT_SUCCESS;
}

