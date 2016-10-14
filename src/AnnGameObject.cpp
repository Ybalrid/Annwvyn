#include "stdafx.h"
#include "AnnGameObject.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnGameObject::AnnGameObject() :
	Node(NULL),
	Entity(NULL),
	bulletReady(false),
	DynamicsWorld(NULL),
	Body(NULL),
	Shape(NULL),
	anim(NULL),
	animIsLooping(false),
	animIsPlaying(false),
	animIsSetted(false),
	visible(true),
	audioSource(nullptr),
	state(nullptr)
{
}

AnnGameObject::~AnnGameObject()
{
	AnnDebug() << "Destructing game object !";
	//Clean OpenAL desaloc
	if (AnnGetAudioEngine())
		AnnGetAudioEngine()->removeSource(audioSource);
	AnnDebug() << "Tidy my physics !";

	if (AnnGetPhysicsEngine())
		AnnGetPhysicsEngine()->removeRigidBody(Body);

	if (Body) delete Body;
	if (Shape)
	{
		if (Shape->getShapeType() == BroadphaseNativeTypes::TRIANGLE_MESH_SHAPE_PROXYTYPE)
			delete static_cast<btBvhTriangleMeshShape*>(Shape)->getMeshInterface();
		delete Shape;
	}
	if (state) delete state;

	Node->getParent()->removeChild(Node);
	std::vector<Ogre::MovableObject*> attachedObject;
	for (unsigned short i(0); i < Node->numAttachedObjects(); i++)
		attachedObject.push_back(Node->getAttachedObject(i));
	Node->detachAllObjects();
	for (auto object : attachedObject)
		AnnGetEngine()->getSceneManager()->destroyMovableObject(object);
	AnnGetEngine()->getSceneManager()->destroySceneNode(Node);
	Node = nullptr;
}

void AnnGameObject::playSound(std::string path, bool loop, float volume)
{
	audioSource->changeSound(path);
	audioSource->setLooping(loop);
	audioSource->setVolume(volume);
	audioSource->setPositon(getPosition());
	audioSource->play();
}

void AnnGameObject::updateOpenAlPos()
{
	audioSource->setPositon(getPosition());
}

void AnnGameObject::setPosition(float x, float y, float z)
{
	if (Node == NULL)
		return;

	AnnVect3 newPosition(x, y, z);

	/*
	*Position of object have to be the same in each part of the engine
	*(graphics, physics, audio)
	*/
	//change BulletPosition
	if (bulletReady)
	{
		auto currentPosition = Node->getPosition();
		Body->translate(btVector3(x - currentPosition.x,
						y - currentPosition.y,
						z - currentPosition.z));
	}
	//change OgrePosition
	Node->setPosition(x, y, z);

	//change OpenAL Source Position
	audioSource->setPositon(newPosition);
}

void AnnGameObject::translate(float x, float y, float z)
{
	//Ogre
	Node->translate(x, y, z);
	//Bullet
	if (Body)
		Body->translate(btVector3(x, y, z));
		//OpenAL
	auto currentPosition = Node->getPosition();
	updateOpenAlPos();
}

void AnnGameObject::setPosition(AnnVect3 pos)
{
	setPosition(pos.x, pos.y, pos.z);
}

void AnnGameObject::setOrientation(float w, float x, float y, float z)
{
	setOrientation(AnnQuaternion(w, x, y, z));
}

void AnnGameObject::setOrientation(AnnQuaternion orient)
{
	//Ogre3D
	if (Node != NULL)
		Node->setOrientation(orient);

	//bullet
	if (Body != NULL)
	{
		btTransform t = Body->getCenterOfMassTransform();
		t.setRotation(orient.getBtQuaternion());
		Body->setCenterOfMassTransform(t);
	}
}

void AnnGameObject::setScale(AnnVect3 scale)
{
	Node->setScale(scale);
}

void AnnGameObject::setScale(float x, float y, float z)
{
	Node->setScale(AnnVect3(x, y, z));
}

AnnVect3 AnnGameObject::getPosition()
{
	if (Node != NULL)
		return Node->getPosition();
	return AnnVect3::ZERO;
}

AnnQuaternion AnnGameObject::getOrientation()
{
	if (Node != NULL)
		return Node->getOrientation();
	return AnnQuaternion::IDENTITY;
}

void AnnGameObject::setNode(Ogre::SceneNode* newNode)
{
	Node = newNode;
}

void AnnGameObject::setEntity(Ogre::Entity* newEntity)
{
	Entity = newEntity;
}

void AnnGameObject::setBulletDynamicsWorld(btDiscreteDynamicsWorld* dynamicsWorld)
{
	DynamicsWorld = dynamicsWorld;
}

void AnnGameObject::setUpPhysics(float mass, phyShapeType type, bool colideWithPlayer)
{
	//check if everything is OK
	if (DynamicsWorld == NULL)
		return;
	if (Node == NULL)
		return;
	if (Entity == NULL)
		return;

	//init shape converter
	BtOgre::StaticMeshToShapeConverter converter(Entity);

	//create the correct shape
	switch (type)
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
			return;
	}

	if (Shape == NULL)
	{
		AnnDebug() << "Error: The shape hasn't been created";
		return;
	}

	AnnVect3 scale = getNode()->getScale();
	Shape->setLocalScaling(scale.getBtVector());

	btVector3 inertia;

	if (mass != 0)
		Shape->calculateLocalInertia(mass, inertia);
	else
		inertia = btVector3(0, 0, 0); //No influence. But mass zero objects are static

	//create rigidBody from shape
	if (!Body)
	{
		state = new BtOgre::RigidBodyState(Node);
		Body = new btRigidBody(mass, state, Shape, inertia);
	}

	if (Body)
	{
		if (colideWithPlayer)
		{
			DynamicsWorld->addRigidBody(Body, MASK(1), MASK(0) | MASK(1));
		}
		else
		{
			DynamicsWorld->addRigidBody(Body, MASK(1), MASK(1));
		}
	}
	else
	{
		AnnDebug() << "Error: RigidBody hasn't been created";
		return; //Unable to create the physical representation
	}

	bulletReady = true;
}

Ogre::SceneNode* AnnGameObject::getNode()
{
	return Node;
}

Ogre::Entity* AnnGameObject::getEntity()
{
	return Entity;
}

float AnnGameObject::getDistance(AnnGameObject *otherObject)
{
	return getPosition().distance(otherObject->getPosition());
}

btRigidBody* AnnGameObject::getBody()
{
	return Body;
}

std::vector<struct collisionTest*> AnnGameObject::getCollisionMask()
{
	return collisionMask;
}

bool AnnGameObject::collideWith(AnnGameObject* Object)
{
	for (size_t i = 0; i < collisionMask.size(); i++)
		if (collisionMask[i]->Object == Object)
			return collisionMask[i]->collisionState;
	return false;
}

void AnnGameObject::updateCollisionStateWith(AnnGameObject* Object, bool updatedState)
{
	for (size_t i = 0; i < collisionMask.size(); i++)
		if (collisionMask[i]->Object == Object)
			collisionMask[i]->collisionState = updatedState;
}

void AnnGameObject::cleanCollisionMask()
{
	for (size_t i = 0; i < collisionMask.size(); i++)
	{
		delete collisionMask[i];
		collisionMask.erase(collisionMask.begin() + i);
	}
}

void AnnGameObject::resetCollisionMask()
{
	for (size_t i = 0; i < collisionMask.size(); i++)
		collisionMask[i]->collisionState = false;
}

void AnnGameObject::testCollisionWith(AnnGameObject* Object)
{
	if (Object == this) return; //Explain me how I can colide with myself o.O

	struct collisionTest* tester = new collisionTest;

	tester->collisionState = false;
	tester->Object = Object;

	tester->Receiver = this;

	collisionMask.push_back(tester);
}

void AnnGameObject::stopGettingCollisionWith(AnnGameObject* Object)
{
	if (!Object) return;
	for (size_t i = 0; i < collisionMask.size(); i++)
		if (collisionMask[i]->Object == Object)
		{
			delete(collisionMask[i]);
			collisionMask[i] = NULL;
		}
}

void AnnGameObject::setAnimation(const char animationName[])
{
	if (animIsSetted)
	{
		anim->setEnabled(false);
		anim->setLoop(false);
		animIsSetted = false;
		animIsLooping = false;
		animIsPlaying = false;
		anim = NULL;
	}

	anim = Entity->getAnimationState(animationName);
	if (anim != NULL)
		animIsSetted = true;
}

void AnnGameObject::playAnimation(bool play)
{
	if (animIsSetted)
	{
		anim->setEnabled(play);
		animIsPlaying = play;
	}
}

void AnnGameObject::loopAnimation(bool loop)
{
	if (animIsSetted)
	{
		anim->setLoop(loop);
		animIsLooping = loop;
	}
}

void AnnGameObject::addAnimationTime(double offset)
{
	if (!animIsSetted || !animIsPlaying)
		return;

	anim->addTime(float(offset));
}

void AnnGameObject::applyImpulse(AnnVect3 force)
{
	Body->applyCentralImpulse(force.getBtVector());
}

void AnnGameObject::applyForce(AnnVect3 force)
{
	Body->applyCentralForce(force.getBtVector());
}

void AnnGameObject::setLinearSpeed(AnnVect3 v)
{
	if (bulletReady)
		Body->setLinearVelocity(v.getBtVector());
}

void AnnGameObject::setVisible()
{
	visible = true;
	getNode()->setVisible(true);
}

void AnnGameObject::setInvisible()
{
	visible = false;
	getNode()->setVisible(false);
}

bool AnnGameObject::isVisible()
{
	return visible;
}

void AnnGameObject::setID(std::string ID)
{
	id = ID;
}

std::string AnnGameObject::getID()
{
	return id;
}