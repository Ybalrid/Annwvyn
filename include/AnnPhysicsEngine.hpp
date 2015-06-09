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
	///Class that abstract the physics engine initialisation and collision information querries for Annwvyn.
	class DLL AnnPhysicsEngine
	{
	public:
		AnnPhysicsEngine(Ogre::SceneNode* rootNode);
		~AnnPhysicsEngine();

		///Add the player body to the dynamics world
		void addPlayerPhysicalBodyToDynamicsWorld(AnnPlayer* player);

		///Create player's rigidbody
		void createPlayerPhysicalVirtualBody(AnnPlayer* player, Ogre::SceneNode* node);

		///Create player's body shape (a capsule)
		void createVirtualBodyShape(AnnPlayer* player);
		
		///Pointer to the bullet's dynamics world
		btDiscreteDynamicsWorld* getWorld();
		
		///Step the simulation
		/// \param delta Interval in seconds that time has to be simulated
		void step(float delta);

		///Process the collision querry system
		/// \param the objet list where to process collision querry
		void processCollisionTesting(AnnGameObjectVect& object);

		///Process triggers contact event
		/// \param player The player object
		/// \param triggers list where to process collision querry
		void processTriggersContacts(AnnPlayer* player, AnnTriggerObjectVect& triggers);

		///Remove a body from simulation
		void removeRigidBody(btRigidBody* body);

		///Init player's body physical simulation
		void initPlayerPhysics(AnnPlayer* player, Ogre::SceneNode* node);

		///Set the debug drawer state
		void setDebugPhysics(bool state);

		///Step the debug drawing for the physical representation
		void stepDebugDrawer();

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
	};
}

#endif //ANN_PHYSICS_ENGINE
