/**
 * * \file main.cpp
 * \brief test/demo program
 * \author Arthur Brainville 
 * \version 0.1
 * \date 08/03/13
 *
 * Annwvyn test program http://annwvyn.ybalrid.info/
 *
 */

//C++ includes
#include <iostream>
#include <streambuf>
#include <fstream>
#include <cmath>
//Annwvyn
#include <Annwvyn.h>

using namespace std;


class Sinbad : public Annwvyn::AnnGameObject
{
    public:
    void postInit()
    {
        setPos(0,2,3);
        setAnimation("Dance");
        playAnimation(true);
        loopAnimation(true);

        setUpBullet(40, Annwvyn::boxShape);
    }

};


//If you want to redirect cout & cerr to cout.txt and cerr.txt, uncomment the folowing line : 
//#define OUTSTREAM_TO_FILE

#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
int main(int argc, char **argv)
#endif
{
	#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	#ifdef OUTSTREAM_TO_FILE
	//If we are running a win32 application we cannot acces stdout & stderr on a console
	//so all output stream are redirected into files. It's ugly but usefull for debuging
	std::ofstream file;
	file.open ("cout.txt");
	std::streambuf* sbuf = std::cout.rdbuf();
	std::cout.rdbuf(file.rdbuf());

	std::ofstream file2;
	file2.open ("cerr.txt");
	std::streambuf* sbuf2 = std::cerr.rdbuf();
	std::cerr.rdbuf(file2.rdbuf());
	#endif //files
	#endif //windows


	//create Annwvyn engine
	Annwvyn::AnnEngine* GameEngine = new Annwvyn::AnnEngine;	

	//load ressources
	GameEngine->loadDir("media");
	GameEngine->loadDir("media/dome");
	GameEngine->loadZip("media/Sinbad.zip");
    GameEngine->loadDir("media/plane");

	GameEngine->initRessources();


	//Create Objects


    Sinbad* S = (Sinbad*) GameEngine->createGameObject("Sinbad.mesh", new Sinbad);
    
    Annwvyn::AnnGameObject* Grid = GameEngine->createGameObject("Plane.mesh");
	Grid->setPos(0,-3,0);
	Grid->setUpBullet();
	GameEngine->setGround(Grid); 

	//Add light
	Annwvyn::AnnLightObject* Light = GameEngine->addLight();	
	GameEngine->setAmbiantLight(Ogre::ColourValue(.1f,.1f,.1f));
	Light->setPosition(0,3,10);


	GameEngine->initPlayerPhysics();
	GameEngine->setDebugPhysicState(true);


	//setUp Oculus system
    GameEngine->oculusInit();

	GameEngine->setSkyDomeMaterial(true,"Sky/dome1");


	//play background music
	GameEngine->getAudioEngine()->playBGM("media/bgm/Blown_Away.ogg",0.2f); //volume 20%
	
    
    
	while(!GameEngine->requestStop())
	{
		GameEngine->refresh();
	}

    delete GameEngine;
	return 0;
}
