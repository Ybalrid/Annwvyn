/**
 * \file AnnPhysicsEngine.hpp
 * \brief The Physics Engine handle the Bullet "dynamics world" where the simulation occurs.
 *		  It also handle collision feedback and triggers
 * \author A. Brainville (Ybalrid)
 */
#ifndef ANN_PHYSICS_ENGINE
#define ANN_PHYSICS_ENGINE

#include "systemMacro.h"

//Bullet
#include <btBulletCollisionCommon.h>
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
		void processCollisionTesting(AnnGameObjectList& object);

		///Process triggers contact event
		/// \param player The player object
		/// \param triggers list where to process collision querry
		void processTriggersContacts(AnnPlayer* player, AnnTriggerObjectList& triggers);

		///Remove a body from simulation
		void removeRigidBody(btRigidBody* body);

		///Init player's body physical simulation
		void initPlayerPhysics(AnnPlayer* player, Ogre::SceneNode* node);

		///Set the debug drawer state
		void setDebugPhysics(bool state);

		///Step the debug drawing for the physical representation
		void stepDebugDrawer();

		///Change the gravity vector
		void changeGravity(AnnVect3 gravity);

	private:
		btBroadphaseInterface* Broadphase;
		btDefaultCollisionConfiguration* CollisionConfiguration;
		btCollisionDispatcher* Dispatcher;
		btSequentialImpulseConstraintSolver* Solver;

		//Bullet Dynamics World
		btDiscreteDynamicsWorld* DynamicsWorld;

		bool debugPhysics;
		BtOgre::DebugDrawer* debugDrawer;
	};
}

#endif //ANN_PHYSICS_ENGINE
