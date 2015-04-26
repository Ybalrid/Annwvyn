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

#include <Gorilla.h>

using namespace std;
using namespace Annwvyn;

class Sinbad : public AnnGameObject
{
public:
	void postInit()
	{
		setPos(0,0,-5);
		setScale(0.2,0.2,0.2);
		setAnimation("Dance");
		playAnimation(true);
		loopAnimation(true);
		setUpPhysics(40, phyShapeType::boxShape);
	}
};

AnnMain()
{
	//create Annwvyn engine
	AnnEngine* GameEngine(new AnnEngine("A Game"));
	//load ressources
	GameEngine->loadDir("media/dome");
	GameEngine->loadZip("media/Sinbad.zip");
	GameEngine->loadDir("media/plane");
	GameEngine->loadDir("media/body");
	GameEngine->loadDir("media/GUI");

	AnnEngine::Instance()->loadDir("media/environement");

	GameEngine->initResources();

	AnnEngine::Instance()->createGameObject("Island.mesh")->setUpBullet();
	AnnEngine::Instance()->createGameObject("Water.mesh");
	AnnGameObject* Sign(AnnEngine::Instance()->createGameObject("Sign.mesh"));

	Sign->setPos(0.5f,0,-2);
	Sign->setUpPhysics(0, phyShapeType::staticShape);

	//Add light
	//Light->setPosition(0,3,10);
	
	AnnLightObject* Sun = GameEngine->addLight();
	Sun->setType(Ogre::Light::LT_DIRECTIONAL);
	Sun->setDirection(Ogre::Vector3::NEGATIVE_UNIT_Y + 1.5* Ogre::Vector3::NEGATIVE_UNIT_Z);
	GameEngine->setAmbiantLight(Ogre::ColourValue(.6f,.6f,.6f));
	//GameEngine->setSkyDomeMaterial(true,"Sky/dome1");

	GameEngine->initPlayerPhysics();
	GameEngine->setDebugPhysicState(true);

	//setUp Oculus system
	GameEngine->oculusInit();
	GameEngine->setNearClippingDistance(0.20f);

	GameEngine->attachVisualBody("male_Body.mesh",-0.1 ,true);
	AnnGameObject* S = GameEngine->createGameObject("Sinbad.mesh", new Sinbad);

    GameEngine->useDefaultEventListener();
	GameEngine->resetOculusOrientation();
	do	
	{ 
		if(GameEngine->isKeyDown(OIS::KC_F12))
			GameEngine->resetOculusOrientation();	
	}
	while(GameEngine->refresh());

	//delete GameEngine;
	return 0;
}


/*
	//////////////////////////// INIT GUI TEST 
	Ogre::SceneNode* camera = GameEngine->getCamera();
	Ogre::SceneNode* GUI_root = camera->createChildSceneNode();
	GUI_root->translate(-0.6,0,-2);
	Gorilla::Silverback* svbk = new Gorilla::Silverback();
	svbk->loadAtlas("dejavu");

	Gorilla::ScreenRenderable* screen = svbk->createScreenRenderable(Ogre::Vector2(2.0f,1.0f),"dejavu");
	GUI_root->attachObject(screen);

	Gorilla::Layer* baseLayer = screen->createLayer(0);
	Gorilla::Rectangle* background = baseLayer->createRectangle(0,0,300,200);
	background->background_colour(Gorilla::rgb(255,0,0,0));

	Gorilla::Caption* testText = baseLayer->createCaption(9,20,20,"Test");
	testText->colour(Ogre::ColourValue::Black);
	testText->height(5);
	testText->_redraw();
*/