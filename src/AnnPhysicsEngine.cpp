#include "stdafx.h"
#include "AnnPhysicsEngine.hpp"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"
using namespace Annwvyn;

AnnPhysicsEngine::AnnPhysicsEngine(Ogre::SceneNode * rootNode,
	std::shared_ptr<AnnPlayer> player) : AnnSubSystem("PhysicsEngie"),
	Broadphase(nullptr),
	CollisionConfiguration(nullptr),
	Solver(nullptr),
	DynamicsWorld(nullptr),
	debugDrawer(nullptr),
	playerRigidBodyState(nullptr),
	playerObject(player),
	defaultGravity(0, -9.81f, 0)
{
	//Initialize the Bullet world
	Broadphase = std::make_unique<btDbvtBroadphase>();
	CollisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
	Dispatcher = std::make_unique<btCollisionDispatcher>(CollisionConfiguration.get());
	Solver = std::make_unique<btSequentialImpulseConstraintSolver>();
	DynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(Dispatcher.get(), Broadphase.get(), Solver.get(), CollisionConfiguration.get());
	AnnDebug() << "btDiscreteDynamicsWorld instantiated";

	//Set gravity vector
	DynamicsWorld->setGravity(defaultGravity.getBtVector());
	AnnDebug() << "Gravity vector " << defaultGravity;

	debugPhysics = false;//by default
	debugDrawer = std::make_unique< BtOgre::DebugDrawer>(rootNode, DynamicsWorld.get(), AnnGetEngine()->getSceneManager());
	DynamicsWorld->setDebugDrawer(debugDrawer.get());
	debugDrawer->setUnlitDiffuseMultiplier(16.0f);
}

AnnPhysicsEngine::~AnnPhysicsEngine()
{
}

void AnnPhysicsEngine::addPlayerPhysicalBodyToDynamicsWorld() const
{
	// TODO ISSUE define name for the bullet's collision masks
	DynamicsWorld->addRigidBody(playerObject->getBody(), MASK(0), MASK(1));
}

void AnnPhysicsEngine::createPlayerPhysicalVirtualBody(Ogre::SceneNode* node)
{
	AnnDebug() << "createPlayerPhysicalVirtualBody";

	//Create (new) a rigid body state through BtOgre
	if (playerRigidBodyState) delete playerRigidBodyState;
	playerRigidBodyState = new BtOgre::RigidBodyState(node);

	//Get inertia vector
	btVector3 inertia;
	auto playerShape = playerObject->getShape();
	const auto playerMass = playerObject->getMass();
	playerShape->calculateLocalInertia(playerMass, inertia);

	//Set the body to the player
	auto body = new btRigidBody
	{
		playerMass,
		playerRigidBodyState,
		playerShape,
		inertia
	};

	playerObject->setBody(body);
}

void AnnPhysicsEngine::createVirtualBodyShape(float radius) const
{
	//remove the diameter of the two half sphere on top and bottom of the capsule
	playerObject->setShape(new btCapsuleShape{ radius, playerObject->getEyesHeight() - 2 * radius });
}

btDiscreteDynamicsWorld* AnnPhysicsEngine::getWorld() const
{
	return DynamicsWorld.get();
}

void AnnPhysicsEngine::step(float delta) const
{
	DynamicsWorld->stepSimulation(delta, 10, 1.0f / 240.0f);
}

void AnnPhysicsEngine::stepDebugDrawer() const
{
	if (debugPhysics)
		debugDrawer->step();
}

void AnnPhysicsEngine::processCollisionTesting() const
{
	const auto nbManifold = DynamicsWorld->getDispatcher()->getNumManifolds();
	for (auto i{ 0 }; i < nbManifold; ++i)
	{
		auto contactManifold = DynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		if (contactManifold->getNumContacts() > 0)
			AnnGetEventManager()->detectedCollision(contactManifold->getBody0()->getUserPointer(),
				contactManifold->getBody1()->getUserPointer());
	}
}

void AnnPhysicsEngine::removeRigidBody(btRigidBody* body) const
{
	AnnDebug() << "Removing " << body << " Form physics simulation";
	if (body)
		DynamicsWorld->removeRigidBody(body);
}

void AnnPhysicsEngine::setDebugPhysics(bool state)
{
	if (state)
	{
		debugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe
			| btIDebugDraw::DBG_FastWireframe
			| btIDebugDraw::DBG_DrawContactPoints);
	}
	else
	{
		debugDrawer->setDebugMode(0);
	}

	debugPhysics = state;
	debugDrawer->step();
}

void AnnPhysicsEngine::changeGravity(AnnVect3 gravity) const
{
	DynamicsWorld->setGravity(gravity.getBtVector());
}

void AnnPhysicsEngine::resetGravity() const
{
	changeGravity(defaultGravity);
}

void AnnPhysicsEngine::update()
{
	stepDebugDrawer();
	step(float(AnnGetEngine()->getFrameTime()));
	processCollisionTesting();
}

void AnnPhysicsEngine::toggleDebugPhysics()
{
	setDebugPhysics(!debugPhysics);
}

void AnnPhysicsEngine::initPlayerRoomscalePhysics(Ogre::SceneNode* playerAnchorNode) const
{
	playerObject->setMode(ROOMSCALE);
	AnnDebug() << "Initializing player's physics in RoomScale mode";

	btCollisionShape* sphere = new btSphereShape(0.25f);
	auto body = new btRigidBody(0, nullptr, sphere);

	playerObject->setShape(sphere);
	playerObject->setBody(body);

	playerObject->setRoomRefNode(playerAnchorNode);
	playerObject->resetPlayerPhysics();//this will properly put the player at it's intended position and will put it inside the dynamics world.
}

void AnnPhysicsEngine::initPlayerStandingPhysics(Ogre::SceneNode* node)
{
	playerObject->setMode(STANDING);
	AnnDebug() << "Player's Capsule RigidBody : " << playerObject->getMass() << "Kg" << playerObject->getEyesHeight();
	createVirtualBodyShape();
	createPlayerPhysicalVirtualBody(node);
	addPlayerPhysicalBodyToDynamicsWorld();
}

void AnnPhysicsEngine::setDebugDrawerColorMultiplier(float value) const
{
	debugDrawer->setUnlitDiffuseMultiplier(value);
}