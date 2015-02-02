#ifndef ANN_PLAYER
#define ANN_PLAYER

#include "systemMacro.h"
#include <Ogre.h>
#include "euler.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

namespace Annwvyn
{
	class AnnEngine; //predeclaration of AnnEngine
	///Parameters of the VirtualBody		
	class bodyParams
	{
	public:

		bodyParams();

		float eyeHeight;
		float walkSpeed;
		float turnSpeed;
		float mass;
		Ogre::Vector3 Position;
		Ogre::Euler Orientation;
		Ogre::Quaternion HeadOrientation;

		//bullet
		btCollisionShape* Shape;
		btRigidBody* Body;
	};


	class AnnPlayer
	{
	public:
		AnnPlayer();
		~AnnPlayer();

		void lockParameters();
		
		bool isLocked();

		void setPosition(Ogre::Vector3 Position);
		void setOrientation(Ogre::Euler Orientation);
		void setHeadOrientation(Ogre::Quaternion HeadOrientation);
		
		//Physical parameters
		void setEyesHeight(float eyeHeight);
		void setWalkSpeed(float walkSpeed);
		void setTurnSpeed(float turnSpeed);
		void setMass(float mass);

		void setShape(btCollisionShape* Shape);
		void setBody(btRigidBody* Body);


		float getEyesHeight();
		float getWalkSpeed();
		float getTurnSpeed();
		float getMass();


		Ogre::Vector3 getPosition();

		btRigidBody* getBody();
		btCollisionShape* getShape();

	protected:
		bodyParams* playerBody;
	private:
		void unlockParameters();
		friend class AnnEngine;
		bodyParams* getLowLevelBodyParams();

		bool locked;
	};

}

#endif //ANN_PLAYER