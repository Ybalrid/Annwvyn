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

float toMeters(float millimeters)
{
	return millimeters/1000.0f * 1.3f;
}

//stream problem on windows :
#define OUTSTREAM_TO_FILE

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
	Annwvyn::AnnEngine GameEngine;	

	//load ressources
	GameEngine.loadDir("media");
	GameEngine.loadDir("media/dome");
	GameEngine.loadZip("media/Sinbad.zip");
	GameEngine.loadDir("media/plane");
	GameEngine.loadDir("media/hand");

	GameEngine.initRessources();

	//Create Objects
	Annwvyn::AnnGameObject* Sinbad = GameEngine.createGameObject("Sinbad.mesh");
	Sinbad->node()->scale(.40,.40,.40);
	Sinbad->setPos(0,2,3);

	Sinbad->setAnimation("Dance");
	Sinbad->playAnimation(true);
	Sinbad->loopAnimation(true);

	Sinbad->setUpBullet(140, Annwvyn::boxShape);

	Annwvyn::AnnGameObject* Sinbad2 = GameEngine.createGameObject("Sinbad.mesh");
	Sinbad2->node()->scale(.35,.35,.35);
	Sinbad2->setPos(1,2,4);
	
	Sinbad2->setAnimation("IdleTop");
	Sinbad2->playAnimation(true);
	Sinbad2->loopAnimation(true);
	
	Sinbad2->setUpBullet(140, Annwvyn::boxShape);

	Sinbad2->testCollisionWith(Sinbad);


	Annwvyn::AnnGameObject* Grid = GameEngine.createGameObject("Plane.mesh");
	Grid->setPos(0,0,0);
	Grid->setUpBullet();
	GameEngine.setGround(Grid); 

	//Add light
	Annwvyn::AnnLightObject* Light = GameEngine.addLight();	
	GameEngine.setAmbiantLight(Ogre::ColourValue(.1f,.1f,.1f));
	Light->setPosition(0,3,10);


	GameEngine.initPlayerPhysics();
	GameEngine.setDebugPhysicState(true);


	//setUp Oculus system
    	GameEngine.oculusInit();

	GameEngine.setSkyDomeMaterial(true,"Sky/dome1");

	//play background music
	GameEngine.getAudioEngine()->playBGM("media/bgm/Blown_Away.ogg",0.2f); //volume 20%
	
	//sinbad make sound
	Sinbad->playSound("media/monster.wav",true); //true = in loop, false by default

	Annwvyn::AnnLeap Leap;

	Annwvyn::AnnGameObject *lHand = GameEngine.createGameObject("Palm.mesh"), *rHand = GameEngine.createGameObject("Palm.mesh");
	Annwvyn::AnnGameObject *lFingers[5];

	for(size_t i = 0; i < 5; i++)
	{
		lFingers[i] = GameEngine.createGameObject("Finger.mesh");
	}

	Annwvyn::AnnGameObject *rFingers[5];

	for(size_t i = 0; i < 5; i++)
	{
		rFingers[i] = GameEngine.createGameObject("Finger.mesh");
	}

	Ogre::Vector3 decal (0.0f, -0.4f, -0.2f);


	//Render loop
	while(!GameEngine.requestStop())
	{
		//if no new hands
		if(!(Leap.getNbHands() > 0))
		{
			//mask all
			lHand->setPos(0,-10,0);
			rHand->setPos(0,-10,0);
			for(int i = 0; i < 5; i++)
			{
				lFingers[i]->setPos(0,-10,0);
				rFingers[i]->setPos(0,-10,0);
			}
		}
		//make 
		else
		{
					//get hands positions
		Ogre::Vector3 lHandPos (toMeters(Leap.getLeftHand().palmPosition().x), toMeters(Leap.getLeftHand().palmPosition().y), toMeters(Leap.getLeftHand().palmPosition().z));
		Ogre::Vector3 rHandPos (toMeters(Leap.getRightHand().palmPosition().x), toMeters(Leap.getRightHand().palmPosition().y), toMeters(Leap.getRightHand().palmPosition().z));

		//set Y base vector
		Ogre::Vector3 Y(0,1,0);

		//get player's body basic parameters
		Annwvyn::bodyParams* vBody = GameEngine.getBodyParams();
		
		//get body orientation angle
		double theta =  vBody->Orientation.getYaw().valueRadians();
		
		//calculate the quaternion for this transform
		Ogre::Quaternion rotateTransform((Ogre::Radian)theta, Y);

		//aply it to an orthogonal vector of Y to get Z
		Ogre::Vector3 Z(-1*(rotateTransform*Ogre::Vector3::NEGATIVE_UNIT_Z));
		//do a vectorial product to get the 3rd base vector
		Ogre::Vector3 X(Y.crossProduct(Z));

		//get hands coordinates from leap base to world base
		Ogre::Vector3 lNewHandPos(lHandPos.x*X + lHandPos.y*Y + lHandPos.z*Z);
		Ogre::Vector3 rNewHandPos(rHandPos.x*X + rHandPos.y*Y + rHandPos.z*Z);
		
		//rotate the decal vector
		Ogre::Vector3 newDecal(rotateTransform*decal);

		//translate hands in front of player body
		lNewHandPos += newDecal + vBody->Position;
		rNewHandPos += newDecal + vBody->Position;
		lHand->setPos(lNewHandPos);
		rHand->setPos(rNewHandPos);

		//get hands orientation
		Ogre::Euler lHandOrient(-(Ogre::Real) Leap.getLeftHand().direction().yaw() * 1.1,
			(Ogre::Real)Leap.getLeftHand().direction().pitch() * 1.1,
			(Ogre::Real)Leap.getLeftHand().palmNormal().roll());
		//aply it
		lHand->setOrientation(rotateTransform*lHandOrient.toQuaternion());

		Ogre::Euler rHandOrient(-(Ogre::Real) Leap.getRightHand().direction().yaw() * 1.1,
			(Ogre::Real)Leap.getRightHand().direction().pitch() * 1.1,
			(Ogre::Real)Leap.getRightHand().palmNormal().roll());

		rHand->setOrientation(rotateTransform*rHandOrient.toQuaternion());
		
			for(size_t i = 0; i < Leap.getLeftHand().fingers().count(); i++)
			{
				Leap::Vector pos(Leap.getLeftHand().fingers()[i].tipPosition());
				Ogre::Vector3 fingerPos(toMeters(pos.x),toMeters(pos.y), toMeters(pos.z));

				Ogre::Vector3 newPos(fingerPos.x*X + fingerPos.y*Y + fingerPos.z*Z);
				lFingers[i]->setPos(newPos + newDecal + vBody->Position);
				
				Ogre::Euler dir(-Leap.getLeftHand().fingers()[i].direction().yaw(),
					Leap.getLeftHand().fingers()[i].direction().pitch(),
					Leap.getLeftHand().palmNormal().roll());
				Ogre::Quaternion qDir = dir.toQuaternion();
				
				lFingers[i]->setOrientation(rotateTransform * qDir);
				lFingers[i]->node()->setScale(1,1,(Leap.getLeftHand().fingers()[i].length()/1000)*1.1);
			}

			if(Leap.getLeftHand().fingers().count() < 5)
			{
				for(size_t i = Leap.getLeftHand().fingers().count(); i < 5; i++)
				{
					lFingers[i]->setPos(0,-10,0);
				}
			}

			/////////////////////////////////////////
			
			for(size_t i = 0; i < Leap.getRightHand().fingers().count(); i++)
			{
				Leap::Vector pos(Leap.getRightHand().fingers()[i].tipPosition());
				Ogre::Vector3 fingerPos(toMeters(pos.x),toMeters(pos.y), toMeters(pos.z));

				Ogre::Vector3 newPos(fingerPos.x*X + fingerPos.y*Y + fingerPos.z*Z);
				rFingers[i]->setPos(newPos + newDecal + vBody->Position);
				
				Ogre::Euler dir(-Leap.getRightHand().fingers()[i].direction().yaw(),
					Leap.getRightHand().fingers()[i].direction().pitch(),
					Leap.getRightHand().palmNormal().roll());
				Ogre::Quaternion qDir = dir.toQuaternion();
				
				rFingers[i]->setOrientation(rotateTransform * qDir);
				rFingers[i]->node()->setScale(1,1,(Leap.getRightHand().fingers()[i].length()/1000)*1.1);
			}

			if(Leap.getRightHand().fingers().count() < 5)
			{
				for(size_t i = Leap.getRightHand().fingers().count(); i < 5; i++)
				{
					rFingers[i]->setPos(0,-10,0);
				}
			}
		}

		GameEngine.refresh();
	}
	return 0;
}
