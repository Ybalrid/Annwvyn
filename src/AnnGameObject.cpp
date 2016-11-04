#include "stdafx.h"
#include "AnnGameObject.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnGameObject::AnnGameObject() :
	Node(nullptr),
	Entity(nullptr),
	Body(nullptr),
	Shape(nullptr),
	state(nullptr),
	anim(nullptr),
	audioSource(nullptr),
	animIsLooping(false),
	animIsPlaying(false),
	animIsSetted(false)
{
}

AnnGameObject::~AnnGameObject()
{
	for (auto script : scripts) script->unregisterAsListener();

	AnnDebug() << "Destructing game object " << getName() << " !";
	//Clean OpenAL de-aloc
	if (AnnGetAudioEngine())
		AnnGetAudioEngine()->removeSource(audioSource);

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

void Annwvyn::AnnGameObject::callUpdateOnScripts()
{
	for (auto script : scripts) script->update();
}

void AnnGameObject::setPosition(float x, float y, float z)
{
	setPosition(AnnVect3{ x,y,z });
}

void AnnGameObject::translate(float x, float y, float z)
{
	//Ogre
	Node->translate(x, y, z);
	//Bullet
	if (Body) Body->translate(btVector3(x, y, z));
	//OpenAL
	auto currentPosition = Node->getPosition();
	updateOpenAlPos();
}

void AnnGameObject::setPosition(AnnVect3 pos)
{
	if (Body)
	{
		auto currentPosition = Node->getPosition();
		Body->translate(btVector3(pos.x - currentPosition.x,
						pos.y - currentPosition.y,
						pos.z - currentPosition.z));
	}
	//change OgrePosition
	Node->setPosition(pos);

	//change OpenAL Source Position
	audioSource->setPositon(pos);
}

void AnnGameObject::setOrientation(float w, float x, float y, float z)
{
	setOrientation(AnnQuaternion(w, x, y, z));
}

void AnnGameObject::setOrientation(AnnQuaternion orient)
{
	//Ogre3D
	Node->setOrientation(orient);

	//bullet
	if (Body)
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
	return Node->getPosition();
}

AnnQuaternion AnnGameObject::getOrientation()
{
	return Node->getOrientation();
}

AnnVect3 Annwvyn::AnnGameObject::getScale()
{
	return Node->getScale();
}

void AnnGameObject::setNode(Ogre::SceneNode* newNode)
{
	Node = newNode;
}

void AnnGameObject::setEntity(Ogre::Entity* newEntity)
{
	Entity = newEntity;
}

void AnnGameObject::setUpPhysics(float mass, phyShapeType type, bool colideWithPlayer)
{
	//init shape converter
	BtOgre::StaticMeshToShapeConverter converter(Entity);

	// TODO put this thing inside the Physics engine
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

	AnnVect3 scale = getNode()->getScale();
	Shape->setLocalScaling(scale.getBtVector());

	btVector3 inertia;
	Shape->calculateLocalInertia(mass, inertia);

	//create rigidBody from shape
	state = new BtOgre::RigidBodyState(Node);
	Body = new btRigidBody(mass, state, Shape, inertia);

	short bulletMask = MASK(0) | MASK(1);
	if (!colideWithPlayer)
		bulletMask = MASK(1);
	AnnGetPhysicsEngine()->getWorld()->addRigidBody(Body, MASK(1), bulletMask);
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
	for (auto cm : collisionMask)
		delete cm;
	collisionMask.clear();
}

void AnnGameObject::resetCollisionMask()
{
	for (auto cm : collisionMask) cm->collisionState = false;
}

void AnnGameObject::testCollisionWith(AnnGameObject* Object)
{
	struct collisionTest* tester = new collisionTest;

	tester->collisionState = false;
	tester->Object = Object;
	tester->Receiver = this;

	collisionMask.push_back(tester);
}

void AnnGameObject::stopGettingCollisionWith(AnnGameObject* Object)
{
	auto query = std::find_if(collisionMask.begin(), collisionMask.end(),
							  [=](collisionTest* test) {return test->Object == Object; });

	if (query == collisionMask.end()) return;
	delete *query;
	collisionMask.erase(query);
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
		anim = nullptr;
	}

	anim = Entity->getAnimationState(animationName);
	if (anim != nullptr)
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
	if (Body)
		Body->setLinearVelocity(v.getBtVector());
}

void AnnGameObject::setVisible()
{
	getNode()->setVisible(true);
}

void AnnGameObject::setInvisible()
{
	getNode()->setVisible(false);
}

std::string Annwvyn::AnnGameObject::getName()
{
	return name;
}

void Annwvyn::AnnGameObject::attachScript(const std::string & scriptName)
{
	auto script = AnnGetScriptManager()->getBehaviorScript(scriptName, this);
	if (script->isValid())
		scripts.push_back(script);
	script->registerAsListener();
}