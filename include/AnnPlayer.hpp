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
	protected:
		bodyParams* playerBody;
	private:
		friend class AnnEngine;
		bodyParams* getLowLevelBodyParams();
	};

}

#endif //ANN_PLAYER