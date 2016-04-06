#include "stdafx.h"
#include "AnnPhysicsEngine.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnPhysicsEngine::AnnPhysicsEngine(Ogre::SceneNode* rootNode)
{    
	AnnDebug("Starting Physics subsystem");

	//Initialize the Bullet world
	Broadphase = new btDbvtBroadphase();
	CollisionConfiguration = new btDefaultCollisionConfiguration();
	Dispatcher = new btCollisionDispatcher(CollisionConfiguration);
	Solver = new btSequentialImpulseConstraintSolver();

	AnnDebug() << "btDiscreteDynamicsWorld instantiated";
	DynamicsWorld = new btDiscreteDynamicsWorld(Dispatcher, Broadphase, Solver, CollisionConfiguration);

	AnnDebug("Gravity vector = (0, -9.81f, 0)");
	DynamicsWorld->setGravity(btVector3(0, -9.81f, 0));

	debugPhysics = false;//by default
	debugDrawer = new BtOgre::DebugDrawer(rootNode, DynamicsWorld);
	DynamicsWorld->setDebugDrawer(debugDrawer);
}

AnnPhysicsEngine::~AnnPhysicsEngine()
{
	//Bullet
	delete DynamicsWorld;
	delete Broadphase;
	delete CollisionConfiguration;
	delete Dispatcher;
	delete Solver;
	delete debugDrawer;
}

void AnnPhysicsEngine::addPlayerPhysicalBodyToDynamicsWorld(AnnPlayer* player)
{
	assert(player->getBody());
	//TODO define name for the bullet's collision masks
	DynamicsWorld->addRigidBody(player->getBody(), MASK(0), MASK(1));
}

void AnnPhysicsEngine::createPlayerPhysicalVirtualBody(AnnPlayer* player, Ogre::SceneNode* node)
{
	AnnDebug() << "createPlayerPhysicalVirtualBody";
	//Player need to have a shape (capsule)
	assert(player->getShape());

	//Create a rigid body state through BtOgre
	BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(node);

	//Get inertia vector
	btVector3 inertia;
	player->getShape()->calculateLocalInertia(player->getMass(), inertia);

	//Set the body to the player
	player->setBody(new btRigidBody(
		player->getMass(), 
		state,
		player->getShape(), 
		inertia));	
}

void AnnPhysicsEngine::createVirtualBodyShape(AnnPlayer* player)
{
	assert(player);
	float radius(0.125f);
	player->setShape(new btCapsuleShape(radius, player->getEyesHeight() - 2 * radius));
}

btDiscreteDynamicsWorld* AnnPhysicsEngine::getWorld()
{
	return DynamicsWorld;
}

void AnnPhysicsEngine::step(float delta)
{
	DynamicsWorld->stepSimulation(delta, 3, 1.0f/90.0f);
}

void AnnPhysicsEngine::stepDebugDrawer()
{	
	if(debugPhysics)
		debugDrawer->step();
}

void AnnPhysicsEngine::processCollisionTesting(AnnGameObjectList& objects)
{
	//TODO make a typedeff for getting off the uglyness here 
	std::vector<struct collisionTest*> pairs;

	//get all collision mask
	auto objectIteartor(objects.begin());
	for(size_t i = 0; i < objects.size(); i++)
	{
		std::vector<struct collisionTest*> onThisObject = (*objectIteartor++)->getCollisionMask();

		for(size_t j = 0; j < onThisObject.size(); j++)
			pairs.push_back(onThisObject[j]);
	}

	//process for each maniflod
	int numManifolds = Dispatcher->getNumManifolds();
	//m is manifold identifier
	for (int m(0); m <numManifolds; m++)
	{
		btPersistentManifold* contactManifold =
			DynamicsWorld->getDispatcher()->getManifoldByIndexInternal(m);

		const btCollisionObject* obA = (btCollisionObject*) contactManifold->getBody0();
		const btCollisionObject* obB = (btCollisionObject*) contactManifold->getBody1();

		//Just get the address of the collision objects
		void* pair1 = (void*) obA;
		void* pair2 = (void*) obB;

		//for each known pair on the collision feedback system
		for(size_t p = 0; p < pairs.size(); p++)
		{
			//Get the bodies from the manifold
			void* body1 = (void*) pairs[p]->Object->getBody();
			void* body2 = (void*) pairs[p]->Receiver->getBody();

			//If there is a collision in either way
			if((pair1 == body1 && pair2 == body2) ||
				(pair2 == body1 && pair1 == body2))
			{
				if(contactManifold->getNumContacts() > 0)
					pairs[p]->collisionState = true;
				else
					pairs[p]->collisionState = false;
				break;
			}
		}
	}
}

void AnnPhysicsEngine::removeRigidBody(btRigidBody* body)
{
	AnnDebug() << "Removing " << body << " Form physics simulation";
	if(body)
		DynamicsWorld->removeRigidBody(body);
}

void AnnPhysicsEngine::initPlayerPhysics(AnnPlayer* player, Ogre::SceneNode* node)
{
	AnnDebug() << "Initializing player's physics " << player->getMass() << "Kg ~" << player->getEyesHeight();
	createVirtualBodyShape(player);
	createPlayerPhysicalVirtualBody(player, node);
	addPlayerPhysicalBodyToDynamicsWorld(player);
}

void AnnPhysicsEngine::setDebugPhysics(bool state)
{
	debugPhysics = state;
}

void AnnPhysicsEngine::processTriggersContacts(AnnPlayer* player, AnnTriggerObjectList& triggers)
{
	for(auto trigger : triggers)
	{
		AnnTriggerObject* current(trigger);
		if(current->computeVolumetricTest(player))
		{
			current->setContactInformation(true);
			current->atContact();
		}
		else
		{
			current->setContactInformation(false);
		}

		if((!current->lastFrameContactWithPlayer && current->contactWithPlayer)
			||(current->lastFrameContactWithPlayer && !current->contactWithPlayer))
		AnnEngine::Instance()->getEventManager()->spatialTrigger(current);
	}
}

void AnnPhysicsEngine::changeGravity(AnnVect3 gravity)
{
	DynamicsWorld->setGravity(gravity.getBtVector());
}
