#include "stdafx.h"
#include "AnnGameObject.hpp"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"
#include "AnnException.hpp"

using namespace Annwvyn;

AnnGameObject::AnnGameObject() :
	Node(nullptr), Model(nullptr), currentAnimation(nullptr),
	Shape(nullptr),
	Body(nullptr),
	bodyMass(0),
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
	if (Node)
	{
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
}

void AnnGameObject::playSound(std::string path, bool loop, float volume) const
{
	audioSource->changeSound(path);
	audioSource->setLooping(loop);
	audioSource->setVolume(volume);
	audioSource->setPositon(getWorldPosition());
	audioSource->play();
}

void AnnGameObject::updateOpenAlPos() const
{
	audioSource->setPositon(getWorldPosition());
}

void AnnGameObject::callUpdateOnScripts()
{
	for (auto script : scripts) script->update();
}

void AnnGameObject::setPosition(float x, float y, float z)
{
	setPosition(AnnVect3{ x, y, z });
}

void AnnGameObject::translate(float x, float y, float z) const
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

void AnnGameObject::setWorldPosition(AnnVect3 pos) const
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
	Node->setOrientation(static_cast<Ogre::Quaternion>(orient));

	//bullet
	if (Body)
	{
		auto t = Body->getCenterOfMassTransform();
		t.setRotation(orient.getBtQuaternion());
		Body->setCenterOfMassTransform(t);
	}
}

void AnnGameObject::setWorldOrientation(AnnQuaternion orient) const
{
	Node->_setDerivedOrientation(orient);
}

void AnnGameObject::setScale(AnnVect3 scale, bool scaleMass) const
{
	Node->setScale(scale);
	if (Body&&Shape)
	{
		Shape->setLocalScaling(scale.getBtVector());

		auto world = AnnGetPhysicsEngine()->getWorld();
		world->removeRigidBody(Body);

		btVector3 inertia;
		float scaleLenght;

		if (scaleMass)
			scaleLenght = scale.length() / 3.0f;
		else
			scaleLenght = 1.0f;

		Shape->calculateLocalInertia(scaleLenght * bodyMass, inertia);
		Body->setMassProps(scaleLenght*bodyMass, inertia);

		world->addRigidBody(Body, AnnPhysicsEngine::CollisionMasks::General, AnnPhysicsEngine::CollisionMasks::ColideWithAll);
		Body->activate();
	}
}

void AnnGameObject::setWorldOrientation(float w, float x, float y, float z) const
{
	setWorldOrientation(AnnQuaternion{ w, x, y, z });
}

void AnnGameObject::setScale(float x, float y, float z, bool mass) const
{
	setScale(AnnVect3(x, y, z), mass);
}

AnnVect3 AnnGameObject::getPosition()
{
	return Node->getPosition();
}

AnnQuaternion AnnGameObject::getOrientation()
{
	return Node->getOrientation();
}

AnnVect3 AnnGameObject::getScale() const
{
	return Node->getScale();
}

void AnnGameObject::setNode(Ogre::SceneNode* newNode)
{
	Node = newNode;
}

void AnnGameObject::setUpPhysics(float mass, phyShapeType type, bool colideWithPlayer)
{

	//Some sanity checks
	if (checkForBodyInParent()) throw AnnPhysicsSetupParentError(this);
	if (checkForBodyInChild()) throw AnnPhysicsSetupChildError(this);
	if (mass < 0) return;
    
    //Easy access to physics engine
    auto physicsEngine = AnnGetPhysicsEngine();
    
    //Get the collision shape from the physics engine
    Shape = physicsEngine->_getGameObjectShape(this, type);

    //Apply local scaling
	AnnVect3 scale = getNode()->getScale();
	Shape->setLocalScaling(scale.getBtVector());

	//Register the mass
	bodyMass = mass;
	
    //Calculate inertia
    btVector3 inertia{ 0,0,0 };
	if (bodyMass > 0.0f)
		Shape->calculateLocalInertia(bodyMass, inertia);

	//create rigidBody from shape
	state = new BtOgre::RigidBodyState(Node);
	Body = new btRigidBody(bodyMass, state, Shape, inertia);
	Body->setUserPointer(this);

    //Add body to the dynamics world while respecting collision masks settings
	short bulletMask = AnnPhysicsEngine::CollisionMasks::ColideWithAll;
	if (!colideWithPlayer)
		bulletMask = AnnPhysicsEngine::CollisionMasks::General;

	physicsEngine->getWorld()->addRigidBody(Body, 
            AnnPhysicsEngine::CollisionMasks::General, bulletMask);
}

Ogre::SceneNode* AnnGameObject::getNode() const
{
	return Node;
}

float AnnGameObject::getDistance(AnnGameObject *otherObject) const
{
	return getWorldPosition().distance(otherObject->getWorldPosition());
}

btRigidBody* AnnGameObject::getBody() const
{
	return Body;
}

void AnnGameObject::setAnimation(const std::string&	animationName)
{
	//Check if the item has a skeleton
	if (!getItem()->hasSkeleton())
	{
		AnnDebug() << "Attempting to set a skeleton animation on a skeleton-less object. (" << getName() << ") Check yo' programin' bro!";
		return;
	}

	//Attempt to get the animation
	auto selectedAnimation = getItem()->getSkeletonInstance()->getAnimation(animationName);
	if (!selectedAnimation)
	{
		AnnDebug() << "Looks like " << getName() << " doesn't have an animation called " << animationName;
		return;
	}

	//If an animation was already playing, disable it
	if (currentAnimation)
	{
		currentAnimation->setEnabled(false);
	}

	//Set the current animation, but don't start playing it just yet.
	currentAnimation = selectedAnimation;
}

void AnnGameObject::playAnimation(bool play) const
{
	if (currentAnimation) currentAnimation->setEnabled(play);
}

void AnnGameObject::loopAnimation(bool loop) const
{
	if (currentAnimation) currentAnimation->setLoop(loop);
}

void AnnGameObject::addAnimationTime(double offset) const
{
	if (currentAnimation) currentAnimation->addTime(float(offset));
}

void AnnGameObject::applyImpulse(AnnVect3 force) const
{
	Body->applyCentralImpulse(force.getBtVector());
}

void AnnGameObject::applyForce(AnnVect3 force) const
{
	Body->applyCentralForce(force.getBtVector());
}

void AnnGameObject::setLinearSpeed(AnnVect3 v) const
{
	if (Body)
		Body->setLinearVelocity(v.getBtVector());
}

void AnnGameObject::setFrictionCoef(float coef) const
{
	if (Body)
		Body->setFriction(coef);
}

void AnnGameObject::setVisible() const
{
	getNode()->setVisible(true);
}

void AnnGameObject::setInvisible() const
{
	getNode()->setVisible(false);
}

std::string AnnGameObject::getName() const
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

bool AnnGameObject::hasParent() const
{
	auto parentSceneNode = Node->getParentSceneNode();

	if (reinterpret_cast<uint64_t>(parentSceneNode)
		== reinterpret_cast<uint64_t>(AnnGetEngine()->getSceneManager()->getRootSceneNode()))
	{
		return false;
	}

	if (parentSceneNode != nullptr)
		return true;

	return false;
}

std::shared_ptr<AnnGameObject> AnnGameObject::getParent() const
{
	return AnnGetGameObjectManager()->getFromNode(Node->getParentSceneNode());
}

void AnnGameObject::attachChildObject(std::shared_ptr<AnnGameObject> child) const
{
	//child->Node has been detached from it's current parent(that was either a node or the root node)
	child->Node->getParentSceneNode()->removeChild(child->Node);

	//Attach it to the node of this object.
	Node->addChild(child->Node);
}

void AnnGameObject::detachFromParent() const
{
	Node->getParentSceneNode()->removeChild(Node);
	AnnGetEngine()->getSceneManager()->getRootSceneNode()->addChild(Node);
}

bool AnnGameObject::checkForBodyInParent()
{
	return parentsHaveBody(this);
}

AnnVect3 AnnGameObject::getWorldPosition() const
{
#ifdef _DEBUG
	if (Node->isCachedTransformOutOfDate())
	{
		AnnDebug() << "cached transform was out of date when " << name << " wanted it's own world position";
		return Node->_getDerivedPositionUpdated();
	}
#endif
	return Node->_getDerivedPosition();
}

AnnQuaternion AnnGameObject::getWorldOrientation() const
{
#ifdef _DEBUG
	if (Node->isCachedTransformOutOfDate())
	{
		AnnDebug() << "cached transofrm was out of date when " << name << " wanted it's own world orientaiton";
		return Node->_getDerivedOrientationUpdated();
	}
#endif
	return Node->_getDerivedOrientation();
}

bool AnnGameObject::parentsHaveBody(AnnGameObject* obj) const
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
		auto node = childNode;
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

void AnnGameObject::setWorldPosition(float x, float y, float z) const
{
	setWorldPosition(AnnVect3{ x, y, z });
}

void AnnGameObject::setItem(Ogre::Item* item)
{
	Model = item;
}

Ogre::Item* AnnGameObject::getItem() const
{
	return Model;
}
