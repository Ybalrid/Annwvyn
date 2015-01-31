#ifndef ANN_PLAYER
#define ANN_PLAYER

#include "systemMacro.h"
#include <Ogre.h>
#include "euler.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

namespace Annwvyn
{
		///Parameters of the VirtualBody		
	class bodyParams
	{
	public:
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
	protected:
		bodyParams* playerBody;
	};
}

#endif //ANN_PLAYER