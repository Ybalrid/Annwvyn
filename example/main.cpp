/**
 * \file main.cpp
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
	Annwvyn::AnnGameObject* Sinbad = GameEngine->createGameObject("Sinbad.mesh");
	Sinbad->node()->scale(.40,.40,.40);
	Sinbad->setPos(0,2,3);

	Sinbad->setAnimation("Dance");
	Sinbad->playAnimation(true);
	Sinbad->loopAnimation(true);

	Sinbad->setUpBullet(140, Annwvyn::boxShape);

	Annwvyn::AnnGameObject* Sinbad2 = GameEngine->createGameObject("Sinbad.mesh");
	Sinbad2->node()->scale(.35,.35,.35);
	Sinbad2->setPos(1,2,4);
	
	Sinbad2->setAnimation("IdleTop");
	Sinbad2->playAnimation(true);
	Sinbad2->loopAnimation(true);
	
	Sinbad2->setUpBullet(140, Annwvyn::boxShape);

	Sinbad2->testCollisionWith(Sinbad);


	Annwvyn::AnnGameObject* Grid = GameEngine->createGameObject("Plane.mesh");
	Grid->setPos(0,0,0);
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
	
	//sinbad make sound
	Sinbad->playSound("media/monster.wav",true); //true = in loop, false by default
    
    Annwvyn::AnnMap* map = new Annwvyn::AnnMap(GameEngine, "test.map");
    
	//Render loop
    int count = 0;
	while(!GameEngine->requestStop())
	{
		GameEngine->refresh();
	}

    delete GameEngine;
    delete map;
	return 0;
}
