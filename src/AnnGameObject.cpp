#include "stdafx.h"
#include "AnnGameObject.hpp"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"
#include "AnnException.hpp"

using namespace Annwvyn;

AnnGameObject::AnnGameObject() :
	Node(nullptr),
	Entity(nullptr),
	animIsSetted(false),
	animIsPlaying(false),
	animIsLooping(false),
	anim(nullptr),
	Shape(nullptr),
	Body(nullptr),
	audioSource(nullptr),
	state(nullptr)
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
		if (Shape->getShapeType() == TRIANGLE_MESH_SHAPE_PROXYTYPE)
			delete static_cast<btBvhTriangleMeshShape*>(Shape)->getMeshInterface();
		delete Shape;
	}
	if (state) delete state;

	//Prevent dereferencing null pointer here. Parent can be something other than root scene node now.
	if (Node->getParent())
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
	audioSource->setPositon(getWorldPosition());
	audioSource->play();
}

void AnnGameObject::updateOpenAlPos()
{
	audioSource->setPositon(getWorldPosition());
}

void AnnGameObject::callUpdateOnScripts()
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
	updateOpenAlPos();
}

void AnnGameObject::setWorldPosition(AnnVect3 pos)
{
	Node->_setDerivedPosition(pos);
	updateOpenAlPos();
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

void AnnGameObject::setWorldOrientation(AnnQuaternion orient)
{
	Node->_setDerivedOrientation(orient);
}

void AnnGameObject::setScale(AnnVect3 scale)
{
	Node->setScale(scale);
}

void AnnGameObject::setWorldOrientation(float w, float x, float y, float z)
{
	setWorldOrientation(AnnQuaternion{ w,x,y,z });
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

AnnVect3 AnnGameObject::getScale()
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
	if (checkForBodyInParent()) throw AnnPhysicsSetupParentError(this);
	if (checkForBodyInChild()) throw AnnPhysicsSetupChildError(this);

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
	Body->setUserPointer(this);

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
	return getWorldPosition().distance(otherObject->getWorldPosition());
}

btRigidBody* AnnGameObject::getBody()
{
	return Body;
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

std::string AnnGameObject::getName()
{
	return name;
}

void AnnGameObject::attachScript(const std::string & scriptName)
{
	auto script = AnnGetScriptManager()->getBehaviorScript(scriptName, this);
	if (script->isValid())
		scripts.push_back(script);
	script->registerAsListener();
}

bool AnnGameObject::hasParent()
{
	auto parentSceneNode = Node->getParentSceneNode();

	AnnDebug() << this << " " << getName() << " is testing for parent";
	AnnDebug() << "Our node is" << Node;
	AnnDebug() << "Parent node is " << parentSceneNode;
	AnnDebug() << "Root node is " << AnnGetEngine()->getSceneManager()->getRootSceneNode();

	if (reinterpret_cast<uint64_t>(parentSceneNode)
		== reinterpret_cast<uint64_t>(AnnGetEngine()->getSceneManager()->getRootSceneNode()))
	{
		AnnDebug() << "we caught the fact that parent scene node is root scene node";
		return false;
	}

	if (parentSceneNode != nullptr)
		return true;

	return false;
}

std::shared_ptr<AnnGameObject> AnnGameObject::getParent()
{
	return AnnGetGameObjectManager()->getFromNode(Node->getParentSceneNode());
}

void AnnGameObject::attachChildObject(std::shared_ptr<AnnGameObject> child)
{
	//child->Node has been detached from it's current parent(that was either a node or the root node)
	child->Node->getParentSceneNode()->removeChild(child->Node);

	//Attach it to the node of this object.
	Node->addChild(child->Node);
}

void AnnGameObject::detachFromParent()
{
	Node->getParentSceneNode()->removeChild(Node);
	AnnGetEngine()->getSceneManager()->getRootSceneNode()->addChild(Node);
}

bool AnnGameObject::checkForBodyInParent()
{
	return parentsHaveBody(this);
}

AnnVect3 AnnGameObject::getWorldPosition()
{
	return Node->_getDerivedPosition();
}

AnnQuaternion AnnGameObject::getWorldOrientation()
{
	return Node->_getDerivedOrientation();
}

bool AnnGameObject::parentsHaveBody(AnnGameObject* obj)
{
	if (!hasParent()) return false;
	auto addr = obj->getParent().get();
	if (!addr) return false;
	if (obj->getParent()->getBody()) return true;
	return parentsHaveBody(addr);
}

bool AnnGameObject::checkForBodyInChild()
{
	return childrenHaveBody(this);
}

bool AnnGameObject::childrenHaveBody(AnnGameObject* parentObj)
{
	for (auto childNode : parentObj->Node->getChildIterator())
	{
		auto node = childNode.second;
		auto childSceneNode = dynamic_cast<Ogre::SceneNode*>(node);

		//Is an actual SceneNode
		if (childSceneNode != nullptr)
		{
			auto obj = AnnGetGameObjectManager()->getFromNode(childSceneNode);
			//found an object
			if (obj != nullptr)
			{
				//Found a body
				if (obj->getBody()) return true;

				//Do child recursion here.
				return childrenHaveBody(obj.get());
			}
		}
	}

	return false;
}

void AnnGameObject::setWorldPosition(float x, float y, float z)
{
	setWorldPosition(AnnVect3{ x, y, z });
}