#ifndef LHD
#define LHD
#include <iostream>
#include <AnnEngine.hpp>
#include <AnnLeap.hpp>
#include <Ogre.h>

#define ENHANCEMENT 1.3f

/**
* LeapDraw -> Draw hands form 2 3D objects :
*	A palm witch is the plam of your hand
*	A standardized finger wich is a 1meter long cylinder-style volume wich will be scaled to the size of a finger 
*	(1meter = 1 into Annwvyn coordinate system)
*/

class LeapDraw
{
public:

	LeapDraw(Annwvyn::AnnEngine* engine) :
	decal(Ogre::Vector3::NEGATIVE_UNIT_Z + Ogre::Vector3::NEGATIVE_UNIT_Y),
	meshLoaded(false)
	{	
		m_engine = engine;	
	}

	LeapDraw()
	{
		std::cerr << "You must specify an AnnEngine pointer" << std::endl; 
		exit(-127);
	}
	
	void charge3dMeshs(const char palm[], const char finger[]) //palm & fingers = mesh entity names from ressources loaded into Annwvyn
	{
		bool ok = true;
		lHand = m_engine->createGameObject(palm);
		rHand = m_engine->createGameObject(palm);

		if(!lHand || !rHand) ok = false;
		for(size_t i = 0; i < 5; i++)
		{
			lFingers[i] = m_engine->createGameObject(finger);
			rFingers[i] = m_engine->createGameObject(finger);
			if(!lFingers[i] || !rFingers[i]) ok = false;
		}

		meshLoaded = ok;
	}
	
	void setDecalVector(Ogre::Vector3 decalVector)
	{
		decal = decalVector;
	}

	
	void updateHands()
	{
		if(!meshLoaded) return; //do NOTHING until meshes aren't proprely loaded
		
		//if no new hands position known
		if(!(Leap.getNbHands() > 0))
		{
			//hide all below the base plane
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
		
		//get & convert hands positions
		Ogre::Vector3 lHandPos
			(toMeters(Leap.getLeftHand().palmPosition().x), 
			toMeters(Leap.getLeftHand().palmPosition().y), 
			toMeters(Leap.getLeftHand().palmPosition().z));
		
		Ogre::Vector3 rHandPos 
			(toMeters(Leap.getRightHand().palmPosition().x), 
			toMeters(Leap.getRightHand().palmPosition().y), 
			toMeters(Leap.getRightHand().palmPosition().z));

		////////////////// GENERATE XYZ ROTATIVE PLAYER BASE
		//set Y base vector (never change)
		Ogre::Vector3 Y(0,1,0);

		//get player's body basic parameters
		Annwvyn::bodyParams* vBody = m_engine->getBodyParams();
		
		//get body orientation angle (radian)
		double theta =  vBody->Orientation.getYaw().valueRadians();
		
		//calculate the quaternion for this rotation transform
		Ogre::Quaternion rotateTransform((Ogre::Radian)theta, Y);

		//aply it to an orthogonal vector of Y to get Z 
		Ogre::Vector3 Z(-1*(rotateTransform*Ogre::Vector3::NEGATIVE_UNIT_Z));
		
		//do a vectorial product to get the 3rd base vector
		Ogre::Vector3 X(Y.crossProduct(Z));

		////NOW, (X,Y,Z) IS A CARTESIAN BASE ALIGNED WITH PLAYER'S BODY LOCAL

		/////////////////////////////////// GET LEAP COORDINATE INTO SCENE BASE

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

		//////////////////////////////////// GET OTHER TRANSFORM AND ALIGN HANDS WITH BODY

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
		
		/////////////////////////////////////// DO THE SAME FOR EACH FINGERS

		//////////////////////////// LEFT HAND
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

			//////////////////// RIGHT HAND
			
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

	}
	
private:
	
	float toMeters(float millimeters)
	{
		//convert millimeters into meters, amplifinyng by a ENHANCEMENT factor the mouvements to be smoother to use for the player
		return millimeters/1000.0f * ENHANCEMENT;
	}
	
	bool meshLoaded;

	//Annwvyn objects to be displayed
	Annwvyn::AnnGameObject* lFingers[5];
	Annwvyn::AnnGameObject* rFingers[5];
	Annwvyn::AnnGameObject* lHand,* rHand;

	//AnnLeap plugin
	Annwvyn::AnnLeap Leap;

	//AnnEngine pointer
	Annwvyn::AnnEngine* m_engine;

	//A vector, relative position of Leap Motion Center from the OculusRift Center
	Ogre::Vector3 decal;
	
};

#endif