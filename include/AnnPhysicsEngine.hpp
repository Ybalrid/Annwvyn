/**
 * \file AnnPhysicsEngine.hpp
 * \brief The Physics Engine handle the Bullet "dynamics world" where the simulation occurs.
 *		  It also handle collision feedback and triggers
 * \author A. Brainville (Ybalrid)
 */
#pragma once

#include "systemMacro.h"

#include <memory>

//Bullet
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

//btOgre
#include <BtOgre.hpp>

//Annwvyn
#include <AnnTypes.h>
#include <AnnSubsystem.hpp>

//easy bitmasks
#define MASK(x) (1 << (x))

namespace Annwvyn
{
	class AnnPhysicsEngine;
	using AnnPhysicsEnginePtr = std::shared_ptr<AnnPhysicsEngine>;

	class AnnPlayerBody;
	class AnnGameObject;
	class AnnVect3;

	using AnnPlayerBodyPtr = std::shared_ptr<AnnPlayerBody>;
}

namespace Annwvyn
{
	///Class that abstract the physics engine initialization and collision information queries for Annwvyn.
	class AnnDllExport AnnPhysicsEngine : public AnnSubSystem
	{
	public:
		static std::string getBulletVersion()
		{
			const auto major = BT_BULLET_VERSION / 100;
			const auto minor = BT_BULLET_VERSION - (major * 100);
			return std::to_string(major) + "." + std::to_string(minor);
		}

		///Flag for collisions
		enum CollisionMasks : int {
			Player  = MASK(0),
			General = MASK(1),

			ColideWithAll = Player | General,
		};

		///Create the physics engine
		AnnPhysicsEngine(Ogre::SceneNode* rootNode, AnnPlayerBodyPtr player);

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

		///advanced : functions called to setup physics by game objects
		btCollisionShape* _getGameObjectShape(AnnGameObject* obj, phyShapeType type);

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
		AnnPlayerBodyPtr playerObject;

		///Default value for gravity. Should be initialized to (0, -9.82f, 0) unless something is wrong with this planet.
		AnnVect3 defaultGravity;
	};
}
