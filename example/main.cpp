/**
* * \file main.cpp
* \brief test/demo program
* \author Arthur Brainville 
*
* Annwvyn test program http://annwvyn.org/
*
*/
#include "stdafx.h"

//Annwvyn
#include <Annwvyn.h>
#include <Ann3DTextPlane.hpp>
#include "TestLevel.hpp"
#include "DemoLevel.hpp"

using namespace std;
using namespace Annwvyn;

timerID demoTimer;

class MySaveTest : public AnnSaveDataInterpretor
{
public:
	MySaveTest(std::shared_ptr<AnnSaveFileData> data) : AnnSaveDataInterpretor(data),
	pi(0),
	lives(0)
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

	void testFileIO()
	{
		auto testFile = AnnGetFileSystemManager()->crateSaveFileDataObject("test");
		testFile->setValue("KEY0", "Thing");
		testFile->setValue("KEY1", "otherThing");
		testFile->setValue("lives", 10);
		testFile->setValue("PI", 3.14f);
		testFile->setValue("pos", AnnVect3(2.5, 4.8, Ogre::Math::HALF_PI));
		testFile->setValue("orient", AnnQuaternion(AnnDegree(90), AnnVect3(.5, .5, .5)));

		auto writer(AnnGetFileSystemManager()->getFileWriter());
		writer->write(testFile);
		
		auto reader(AnnGetFileSystemManager()->getFileReader());
		auto data = reader->read("test");

		AnnDebug() << "KEY0 val : " << data->getValue("KEY0");
		AnnDebug() << "KEY1 val : " << data->getValue("KEY1");

		MySaveTest tester(data);
		tester.extract();
		AnnDebug() << "stored vector value : " << tester.getPosition();
		AnnDebug() << "stored quaternion value : " << tester.getOrientation();
		AnnDebug() << "stored pi value : " << tester.getPi();
		AnnDebug() << "stored lives value : " << tester.getLives();

	}

	void KeyEvent(AnnKeyEvent e)
	{
		if (e.isPressed() && e.getKey() == KeyCode::h)
			testFileIO();
	}

	void StickEvent(AnnStickEvent e)
	{
		if (e.getStickID() == 0)
		{
			AnnStickPov pov = e.getPov(0);
		//	AnnDebug() << pov.getNorth() << pov.getSouth() << pov.getEast() << pov.getWest();
		}
	}
};

class DummySubsystem : public AnnUserSubSystem
{
public:
	DummySubsystem(std::string name = "Dummy") : AnnUserSubSystem(name),
		call(0)
	{

	}

	virtual ~DummySubsystem()
	{
		AnnDebug() << "Dummy is no more. ";
	}

	void dummyMethod() 
	{
		AnnDebug() << "access from smart poiner casted through the stupid macro ACTUALLY WORKS!";
	}

protected:
	bool needUpdate()
	{
		return true;
	}

	void update()
	{
		AnnDebug() << "Dummy debug " << ++call;;
	}
	unsigned long call; 
};

AnnMain()
{
	//Only usefull on windows : Open a debug console to get stdout/stderr
	AnnEngine::openConsole();

	AnnInit("AnnTest");
	
	//Init some player body parameters
	AnnGetEngine()->initPlayerPhysics();	
	AnnGetPhysicsEngine()->setDebugPhysics(false);
	AnnGetEventManager()->useDefaultEventListener();
	AnnGetVRRenderer()->recenter();

	//load ressources
	AnnGetResourceManager()->loadDir("media/environement");
	AnnGetResourceManager()->loadDir("media/debug");
	AnnGetResourceManager()->initResources();

	//AnnGetLevelManager()->addLevel(make_shared<TestLevel>());
	AnnGetLevelManager()->addLevel(make_shared<DemoHub>());
	AnnGetLevelManager()->addLevel(make_shared<Demo0>());
	AnnGetLevelManager()->addLevel(make_shared<TestLevel>());


	AnnGetLevelManager()->jumpToFirstLevel();

	AnnDebug() << "Starting the render loop";
	do	
	{
		//This is just for debugging stuff with the level manager
		if(AnnGetEngine()->isKeyDown(OIS::KC_Q))
			AnnGetEngine()->getLevelManager()->unloadCurrentLevel();
		if(AnnGetEngine()->isKeyDown(OIS::KC_E))
			AnnGetEngine()->getLevelManager()->jumpToFirstLevel();	
	}
	while(AnnGetEngine()->refresh());

	AnnQuit();

	return EXIT_SUCCESS;
}
