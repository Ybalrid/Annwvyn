/**
 * * \file main.cpp
 * \brief test/demo program
 * \author Arthur Brainville 
 * \version 0.5
 * \date 06/11/14
 *
 * Annwvyn test program http://annwvyn.org/
 *
 */

//C++ includes
#include <iostream>
#include <streambuf>
#include <fstream>
//Annwvyn
#include <Annwvyn.h>

using namespace std;


class Sinbad : public Annwvyn::AnnGameObject
{
    public:
    void postInit()
    {
        setPos(0,2,3);
		setScale(0.2,0.2,0.2);
        setAnimation("Dance");
        playAnimation(true);
        loopAnimation(true);

        setUpBullet(40, Annwvyn::boxShape);
    }

};

class MyTrigger : public Annwvyn::AnnTriggerObject
{
    void postInit()
    {
        //Radius in metter
        setThreshold(2);
    }

    void atContact()
    {
        //Print "Contact" to standard error stream
        std::cerr << "Contact" << std::endl;
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
	GameEngine->loadDir("media/body");

	GameEngine->initRessources();


	//Create Objects


    Sinbad* S = (Sinbad*) GameEngine->createGameObject("Sinbad.mesh", new Sinbad);

    MyTrigger* T = (MyTrigger*) GameEngine->createTriggerObject(new MyTrigger);
    T->setPosition(0,-1,3);

    Annwvyn::AnnGameObject* Grid = GameEngine->createGameObject("Plane.mesh");
	Grid->setPos(0,-3,0);
	Grid->setUpBullet();
	GameEngine->setGround(Grid); 

	//Add light
	Annwvyn::AnnLightObject* Light = GameEngine->addLight();	
	GameEngine->setAmbiantLight(Ogre::ColourValue(.1f,.1f,.1f));
	Light->setPosition(0,3,10);
    

	GameEngine->initPlayerPhysics();
	GameEngine->setDebugPhysicState(false);


	//setUp Oculus system
    GameEngine->oculusInit();

	GameEngine->setSkyDomeMaterial(true,"Sky/dome1");


	//play background music
	GameEngine->getAudioEngine()->playBGM("media/bgm/Blown_Away.ogg",0.2f); //volume 20%
	
	GameEngine->attachVisualBody("male_Body.mesh",-0.1 ,true);

	while(!GameEngine->requestStop())
	{
		GameEngine->refresh();
	}

    delete GameEngine;
	return 0;
}
