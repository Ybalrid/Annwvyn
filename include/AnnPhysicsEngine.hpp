/**
 * \file AnnPhysicsEngine.hpp
 * \brief The Physics Engine handle the Bullet "dynamics world" where the simulation occurs.
 *		  It also handle collision feedback and triggers
 * \author A. Brainville (Ybalrid)
 */
#ifndef ANN_PHYSICS_ENGINE
#define ANN_PHYSICS_ENGINE

#include "systemMacro.h"

#include <memory>

 //Bullet
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

//btOgre
#include <BtOgre.hpp>

//AnnEngine
#include "AnnEngine.hpp"

namespace Annwvyn
{
	///Class that abstract the physics engine initialization and collision information queries for Annwvyn.
	class DLL AnnPhysicsEngine : public AnnSubSystem
	{
	public:

		///Create the physics engine
		AnnPhysicsEngine(Ogre::SceneNode* rootNode, std::shared_ptr<AnnPlayer> player);

		///Destroy the physics engine
		~AnnPhysicsEngine();

		///Add the player body to the dynamics world
		void addPlayerPhysicalBodyToDynamicsWorld() const;

		///Create player's rigid-body
		void createPlayerPhysicalVirtualBody(Ogre::SceneNode* node);

		///Create player's body shape (a capsule)
		void createVirtualBodyShape(float radius = 0.125f) const;

		///Pointer to the bullet's dynamics world
		btDiscreteDynamicsWorld* getWorld() const;

		///Step the simulation
		/// \param delta Interval in seconds that time has to be simulated
		void step(float delta) const;

		///Process the collision query system
		void processCollisionTesting() const;

		///Remove a body from simulation
		void removeRigidBody(btRigidBody* body) const;

		///Init the class "standing" designed for the Oculus physics where the player is not moving in the room at all
		void initPlayerStandingPhysics(Ogre::SceneNode* playerAnchorNode);

		///Init the "room-scale" physics model where the controls don't break when the player move is ass in the real world
		void initPlayerRoomscalePhysics(Ogre::SceneNode* playerAnchorNode) const;

		///Set the debug drawer state
		void setDebugPhysics(bool state);

		///Step the debug drawing for the physical representation
		void stepDebugDrawer() const;

		///Change the gravity vector
		void changeGravity(AnnVect3 gravity) const;

		///Restore the default gravity
		void resetGravity() const;

		///Toggle the debug physics overlay
		void toggleDebugPhysics();

		///Set the color multiplier to convert raw color to HDR light value
		void setDebugDrawerColorMultiplier(float value) const;

	private:

		friend class AnnEngine;
		///Update by steeping simulation by one frame time. Should be called only once, and only by AnnEngine
		void update() override;

		///Bullet Broadphase
		std::unique_ptr<btBroadphaseInterface> Broadphase;

		///Bullet Default Collision Configuration
		std::unique_ptr<btDefaultCollisionConfiguration> CollisionConfiguration;

		///Bullet Collision Dispatcher
		std::unique_ptr<btCollisionDispatcher> Dispatcher;

		///Bullet Sequential Impulse Constraint Solver
		std::unique_ptr<btSequentialImpulseConstraintSolver> Solver;

		///Bullet Dynamics World
		std::unique_ptr<btDiscreteDynamicsWorld> DynamicsWorld;

		///Should use the debug drawer
		bool debugPhysics;

		///Debug drawer object from BtOgre
		std::unique_ptr<BtOgre::DebugDrawer> debugDrawer;

		///Rigid body state from BtOgre for the Player object
		BtOgre::RigidBodyState* playerRigidBodyState;

		///Shared pointer to the player
		std::shared_ptr<AnnPlayer> playerObject;

		///Default value for gravity. Should be initialized to (0, -9.82f, 0) unless something is wrong with this planet.
		AnnVect3 defaultGravity;
	};
}

#endif //ANN_PHYSICS_ENGINE