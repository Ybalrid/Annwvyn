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
		AnnPhysicsEngine(Ogre::SceneNode* rootNode);
		~AnnPhysicsEngine();

		void addPlayerPhysicalBodyToDynamicsWorld(AnnPlayer* player);
		void createPlayerPhysicalVirtualBody(AnnPlayer* player, Ogre::SceneNode* node);
		void createVirtualBodyShape(AnnPlayer* player);
		
		btDiscreteDynamicsWorld* getWorld();
		
		void step(float delta);
		bool collisionWithGround(AnnPlayer* player);
		void processCollisionTesting(AnnGameObjectVect& object);
		void removeRigidBody(btRigidBody* body);
		void initPlayerPhysics(AnnPlayer* player, Ogre::SceneNode* node);
		void setGround(AnnGameObject* ground);
	private:
		btBroadphaseInterface* m_Broadphase;
		btDefaultCollisionConfiguration* m_CollisionConfiguration;
		btCollisionDispatcher* m_Dispatcher;
		btSequentialImpulseConstraintSolver* m_Solver;
		btGhostPairCallback* m_ghostPairCallback;

		//Bullet Dynamics World
		btDiscreteDynamicsWorld* m_DynamicsWorld;

		bool debugPhysics;
		BtOgre::DebugDrawer* m_debugDrawer;

		AnnGameObject* m_Ground;
	};
}

#endif //ANN_PHYSICS_ENGINE