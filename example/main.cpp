/**
* * \file main.cpp
* \brief test/demo program
* \author Arthur Brainville 
* \version 0.6
* \date 09/03/15
*
* Annwvyn test program http://annwvyn.org/
*
*/
#include "stdafx.h"

//Annwvyn
#include <Annwvyn.h>
#include "TestLevel.hpp"
#include <AnnSplashLevel.hpp>

using namespace std;
using namespace Annwvyn;

timerID demoTimer;

class MySaveTest : public AnnSaveDataInterpretor
{
public:
	MySaveTest(AnnSaveFileData* data) : AnnSaveDataInterpretor(data)
	{
	}

	AnnVect3 getPosition()
	{
		return pos;
	}

	AnnQuaternion getOrientation()
	{
		return orient;
	}

	float getPi()
	{
		return pi;
	}

	int getLives()
	{
		return lives;
	}

	virtual void extract()
	{
		pos = keyStringToVect3("pos");
		orient = keyStringToQuaternion("orient");
		pi = keyStringToFloat("PI");
		lives = keyStringToInt("lives");
	}
private:
	AnnVect3 pos;
	AnnQuaternion orient;
	float pi; 
	int lives;
};

class DebugListener : LISTENER
{
public:
	DebugListener() : constructListener()
	{
	}

	void TimeEvent(AnnTimeEvent e)
	{
		AnnDebug() << "TimeEvent id : " << e.getID();
		if(e.getID() == demoTimer)
			AnnDebug() << "This is the demoTimer that was launched on the main function !"; 
	}
	void TriggerEvent(AnnTriggerEvent e)
	{
		AnnDebug() << "TriggerEvent contact status : " << e.getContactStatus() << " from " << e.getSender();
	}
};

AnnMain()
{
	//Only usefull on windows : Open a debug console to get stdout/stderr
	AnnEngine::openConsole();	
	//Init game engine
	AnnEngine* GameEngine(new AnnEngine("AnnTest"));

	//load ressources
	GameEngine->loadDir("media/environement");
	GameEngine->loadDir("media/debug");
	GameEngine->initResources();

	//SetUp Oculus system	
	GameEngine->oculusInit();
	GameEngine->setNearClippingDistance(0.20f);
	
	//Init some player body parameters
	GameEngine->initPlayerPhysics();	 

	//Register a level
	AnnXmlLevel* level = new AnnXmlLevel("./level/test.xml");
	AnnSplashLevel* splash = new AnnSplashLevel("splash.png", level, 7.1);
	splash->setBGM("media/AnnSplash.ogg");

	GameEngine->getLevelManager()->addLevel(splash);
	GameEngine->getLevelManager()->addLevel(level);
	GameEngine->getLevelManager()->jumpToFirstLevel();		//Jump to that level 
	
	GameEngine->useDefaultEventListener();
	GameEngine->resetOculusOrientation();
	GameEngine->getEventManager()->addListener(new DebugListener);
	demoTimer = GameEngine->getEventManager()->fireTimer(10);
	
	GameEngine->setDebugPhysicState(false);

	AnnLightObject* light = GameEngine->addLight(); 
	GameEngine->destroyLight(light);

	auto testFile = AnnEngine::Instance()->getFileSystemManager()->crateSaveFileDataObject("test");
	testFile->setValue("KEY0", "Thing");
	testFile->setValue("KEY1", "otherThing");
	testFile->setValue("lives", 10);
	testFile->setValue("PI", 3.14f);
	testFile->setValue("pos", AnnVect3(2.5, 4.8, Ogre::Math::HALF_PI));
	testFile->setValue("orient", AnnQuaternion(Ogre::Radian(Ogre::Math::HALF_PI), AnnVect3(.5,.5,.5)));


	AnnFileWriter* writer(AnnEngine::Instance()->getFileSystemManager()->getFileWriter());
	writer->write(testFile);

	AnnFileReader* reader(AnnEngine::Instance()->getFileSystemManager()->getFileReader());
	auto data = reader->read("test");
	
	AnnDebug() << "KEY0 val : " << data->getValue("KEY0");
	AnnDebug() << "KEY1 val : " << data->getValue("KEY1");

	MySaveTest tester(data);
	tester.extract();
	AnnDebug() << "stored vector value : " << tester.getPosition();
	AnnDebug() << "stored quaternion value : " << tester.getOrientation();
	AnnDebug() << "stored pi value : " << tester.getPi();
	AnnDebug() << "stored lives value : " << tester.getLives();

	AnnDebug() << "Starting the render loop";
	do	
	{
		if(GameEngine->isKeyDown(OIS::KC_Q))
			GameEngine->getLevelManager()->unloadCurrentLevel();
		if(GameEngine->isKeyDown(OIS::KC_E))
			GameEngine->getLevelManager()->jumpToFirstLevel();	
	}
	while(GameEngine->refresh());

	delete GameEngine;
	exit(0);
}
