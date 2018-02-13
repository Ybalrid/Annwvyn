// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "stdafx.h"
#include "AnnPhysicsEngine.hpp"
#include "AnnPlayerBody.hpp"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"
#include "AnnException.hpp"

using namespace Annwvyn;
using std::make_unique;

AnnPhysicsEngine::AnnPhysicsEngine(Ogre::SceneNode* rootNode,
								   AnnPlayerBodyPtr player) :
 AnnSubSystem("PhysicsEngie"),
 Broadphase(nullptr),
 CollisionConfiguration(nullptr),
 Solver(nullptr),
 DynamicsWorld(nullptr),
 debugDrawer(nullptr),
 playerRigidBodyState(nullptr),
 playerObject(player),
 defaultGravity(0, -9.81f, 0)
{
	AnnDebug() << "Initializing bullet version " << getBulletVersion();

	//Initialize the Bullet world
	Broadphase			   = make_unique<btDbvtBroadphase>();
	CollisionConfiguration = make_unique<btDefaultCollisionConfiguration>();
	Dispatcher			   = make_unique<btCollisionDispatcher>(CollisionConfiguration.get());
	Solver				   = make_unique<btSequentialImpulseConstraintSolver>();
	DynamicsWorld		   = make_unique<btDiscreteDynamicsWorld>(Dispatcher.get(), Broadphase.get(), Solver.get(), CollisionConfiguration.get());
	AnnDebug() << "btDiscreteDynamicsWorld instantiated";

	//Set gravity vector
	DynamicsWorld->setGravity(defaultGravity.getBtVector());
	AnnDebug() << "Gravity vector " << defaultGravity;

	debugPhysics = false; //by default
	debugDrawer  = make_unique<BtOgre::DebugDrawer>(rootNode, DynamicsWorld.get(), AnnGetEngine()->getSceneManager());
	DynamicsWorld->setDebugDrawer(debugDrawer.get());
	debugDrawer->setUnlitDiffuseMultiplier(16.0f);
}

AnnPhysicsEngine::~AnnPhysicsEngine()
{
}

void AnnPhysicsEngine::addPlayerPhysicalBodyToDynamicsWorld() const
{
	DynamicsWorld->addRigidBody(playerObject->getBody(), Player, General);
}

void AnnPhysicsEngine::createPlayerPhysicalVirtualBody(Ogre::SceneNode* node)
{
	AnnDebug() << "createPlayerPhysicalVirtualBody";

	//Create (new) a rigid body state through BtOgre
	if(playerRigidBodyState) delete playerRigidBodyState;
	playerRigidBodyState = new BtOgre::RigidBodyState(node);

	//Get inertia vector
	btVector3 inertia;
	const auto playerShape = playerObject->getShape();
	const auto playerMass  = playerObject->getMass();
	playerShape->calculateLocalInertia(playerMass, inertia);

	//Set the body to the player
	const auto body = new btRigidBody{
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
	if(debugPhysics)
		debugDrawer->step();
}

void AnnPhysicsEngine::processCollisionTesting() const
{
	const auto nbManifold = DynamicsWorld->getDispatcher()->getNumManifolds();
	for(auto i{ 0 }; i < nbManifold; ++i)
	{
		const auto contactManifold = DynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		if(contactManifold->getNumContacts() > 0)
			AnnGetEventManager()->detectedCollision(
				contactManifold->getBody0()->getUserPointer(),
				contactManifold->getBody1()->getUserPointer(),
				contactManifold->getContactPoint(0).getPositionWorldOnB(),
				contactManifold->getContactPoint(0).m_normalWorldOnB);
	}
}

void AnnPhysicsEngine::removeRigidBody(btRigidBody* body) const
{
	AnnDebug() << "Removing " << body << " Form physics simulation";
	if(body)
		DynamicsWorld->removeRigidBody(body);
}

void AnnPhysicsEngine::setDebugPhysics(bool state)
{
	if(state)
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
	auto body				 = new btRigidBody(0, nullptr, sphere);

	playerObject->setShape(sphere);
	playerObject->setBody(body);

	playerObject->setRoomRefNode(playerAnchorNode);
	playerObject->resetPlayerPhysics(); //this will properly put the player at it's intended position and will put it inside the dynamics world.
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

btCollisionShape* AnnPhysicsEngine::_getGameObjectShape(AnnGameObject* obj, phyShapeType type)
{
	BtOgre::StaticMeshToShapeConverter converter(obj->getItem());

	btCollisionShape* Shape;

	switch(type)
	{
		case boxShape:
			Shape = converter.createBox();
			break;
		case cylinderShape:
			Shape = converter.createCylinder();
			break;
		case capsuleShape:
			Shape = converter.createCapsule();
			break;
		case convexShape:
			Shape = converter.createConvex();
			break;
		case staticShape:
			Shape = converter.createTrimesh();
			break;
		case sphereShape:
			Shape = converter.createSphere();
			break;
		default:
			//non valid;
			AnnDebug() << "Error: Requested shape is invalid";
			throw AnnInvalidPhysicalShapeError(obj->getName());
	}
	return Shape;
}
