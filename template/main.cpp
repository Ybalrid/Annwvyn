//Includes
#include <Annwvyn.h>

//Namespaces
using namespace std;
using namespace Annwvyn; //All Annwvyn components are here 


AnnMain()
{
	//Initialize the engine
	AnnEngine::initialize("MyGame");

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
	return 0;
}

