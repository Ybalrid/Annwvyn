#ifndef ANN_PHYSICS_ENGINE
#define ANN_PHYSICS_ENGINE

#include "systemMacro.h"

//Bullet
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <btBulletDynamicsCommon.h>

//btOgre
#include "BtOgrePG.h"
#include "BtOgreGP.h"
#include "BtOgreExtras.h"

#include "AnnEngine.hpp"

namespace Annwvyn
{
	class DLL AnnPhysicsEngine
	{
	public:
		AnnPhysicsEngine();

	private:

	};
}

#endif //ANN_PHYSICS_ENGINE