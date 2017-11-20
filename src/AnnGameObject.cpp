#include "stdafx.h"
#include "AnnGameObject.hpp"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"
#include "AnnException.hpp"

using namespace Annwvyn;

AnnGameObject::AnnGameObject() :
	sceneNode(nullptr), model3D(nullptr), currentAnimation(nullptr),
	collisionShape(nullptr),
	rigidBody(nullptr),
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
		AnnGetPhysicsEngine()->removeRigidBody(rigidBody);

	if (rigidBody) delete rigidBody;
	if (collisionShape)
	{
		if (collisionShape->getShapeType() == TRIANGLE_MESH_SHAPE_PROXYTYPE)
			delete static_cast<btBvhTriangleMeshShape*>(collisionShape)->getMeshInterface();
		delete collisionShape;
	}
	if (state) delete state;

	//Prevent dereferencing null pointer here. Parent can be something other than root scene node now.
	if (sceneNode)
	{
		if (sceneNode->getParent())
			sceneNode->getParent()->removeChild(sceneNode);
		std::vector<Ogre::MovableObject*> attachedObject;
		for (unsigned short i(0); i < sceneNode->numAttachedObjects(); ++i)
			attachedObject.push_back(sceneNode->getAttachedObject(i));
		sceneNode->detachAllObjects();
		for (auto object : attachedObject)
			AnnGetEngine()->getSceneManager()->destroyMovableObject(object);
		AnnGetEngine()->getSceneManager()->destroySceneNode(sceneNode);
		sceneNode = nullptr;
	}
}

void AnnGameObject::playSound(const std::string& path, bool loop, float volume) const
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
	sceneNode->translate(x, y, z);
	//Bullet
	if (rigidBody)
	{
		rigidBody->translate(btVector3(x, y, z));
		rigidBody->activate();
	}
	//OpenAL
	updateOpenAlPos();
}

void AnnGameObject::setPosition(AnnVect3 pos)
{
	if (rigidBody)
	{
		const auto currentPosition = sceneNode->getPosition();
		rigidBody->translate(btVector3(pos.x - currentPosition.x,
			pos.y - currentPosition.y,
			pos.z - currentPosition.z));

		//Activate the body in the physics engine
		rigidBody->activate();
	}
	//change OgrePosition
	sceneNode->setPosition(pos);

	//change OpenAL Source Position
	updateOpenAlPos();
}

void AnnGameObject::setWorldPosition(AnnVect3 pos) const
{
	sceneNode->_setDerivedPosition(pos);
	updateOpenAlPos();
}

void AnnGameObject::setOrientation(float w, float x, float y, float z)
{
	setOrientation(AnnQuaternion(w, x, y, z));
}

void AnnGameObject::setOrientation(AnnQuaternion orient)
{
	//Ogre3D
	sceneNode->setOrientation(static_cast<Ogre::Quaternion>(orient));

	//bullet
	if (rigidBody)
	{
		auto t = rigidBody->getCenterOfMassTransform();
		t.setRotation(orient.getBtQuaternion());
		rigidBody->setCenterOfMassTransform(t);

		//activate the body
		rigidBody->activate();
	}
}

void AnnGameObject::setWorldOrientation(AnnQuaternion orient) const
{
	sceneNode->_setDerivedOrientation(orient);
}

void AnnGameObject::setScale(AnnVect3 scale, bool scaleMass) const
{
	sceneNode->setScale(scale);
	if (rigidBody && collisionShape)
	{
		collisionShape->setLocalScaling(scale.getBtVector());

		auto world = AnnGetPhysicsEngine()->getWorld();
		world->removeRigidBody(rigidBody);

		btVector3 inertia;
		float scaleLenght;

		if (scaleMass)
			scaleLenght = scale.length() / 3.0f;
		else
			scaleLenght = 1.0f;

		collisionShape->calculateLocalInertia(scaleLenght * bodyMass, inertia);
		rigidBody->setMassProps(scaleLenght*bodyMass, inertia);

		world->addRigidBody(rigidBody, AnnPhysicsEngine::CollisionMasks::General, AnnPhysicsEngine::CollisionMasks::ColideWithAll);
		rigidBody->activate();
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
	return sceneNode->getPosition();
}

AnnQuaternion AnnGameObject::getOrientation()
{
	return sceneNode->getOrientation();
}

AnnVect3 AnnGameObject::getScale() const
{
	return sceneNode->getScale();
}

void AnnGameObject::setNode(Ogre::SceneNode* newNode)
{
	sceneNode = newNode;
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
	collisionShape = physicsEngine->_getGameObjectShape(this, type);

	//Apply local scaling
	AnnVect3 scale = getNode()->getScale();
	collisionShape->setLocalScaling(scale.getBtVector());

	//Register the mass
	bodyMass = mass;

	//Calculate inertia
	btVector3 inertia{ 0,0,0 };
	if (bodyMass > 0.0f)
		collisionShape->calculateLocalInertia(bodyMass, inertia);

	//create rigidBody from shape
	state = new BtOgre::RigidBodyState(sceneNode);
	rigidBody = new btRigidBody(bodyMass, state, collisionShape, inertia);
	rigidBody->setUserPointer(this);

	//Add body to the dynamics world while respecting collision masks settings
	physicsEngine->getWorld()->addRigidBody(rigidBody,
		AnnPhysicsEngine::CollisionMasks::General,
		colideWithPlayer ? AnnPhysicsEngine::CollisionMasks::ColideWithAll : AnnPhysicsEngine::CollisionMasks::General);
}

Ogre::SceneNode* AnnGameObject::getNode() const
{
	return sceneNode;
}

float AnnGameObject::getDistance(AnnGameObject *otherObject) const
{
	return getWorldPosition().distance(otherObject->getWorldPosition());
}

btRigidBody* AnnGameObject::getBody() const
{
	return rigidBody;
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
	const auto selectedAnimation = getItem()->getSkeletonInstance()->getAnimation(animationName);
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
	rigidBody->applyCentralImpulse(force.getBtVector());
}

void AnnGameObject::applyForce(AnnVect3 force) const
{
	rigidBody->applyCentralForce(force.getBtVector());
}

void AnnGameObject::setLinearSpeed(AnnVect3 v) const
{
	if (rigidBody)
		rigidBody->setLinearVelocity(v.getBtVector());
}

void AnnGameObject::setFrictionCoef(float coef) const
{
	if (rigidBody)
		rigidBody->setFriction(coef);
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

void AnnGameObject::attachScript(const std::string& scriptName)
{
	auto script = AnnGetScriptManager()->getBehaviorScript(scriptName, this);
	if (script->isValid())
		scripts.push_back(script);
	script->registerAsListener();
}

bool AnnGameObject::hasParent() const
{
	auto parentSceneNode = sceneNode->getParentSceneNode();

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
	return AnnGetGameObjectManager()->getFromNode(sceneNode->getParentSceneNode());
}

void AnnGameObject::attachChildObject(std::shared_ptr<AnnGameObject> child) const
{
	//child->sceneNode has been detached from it's current parent(that was either a node or the root node)
	child->sceneNode->getParentSceneNode()->removeChild(child->sceneNode);

	//Attach it to the node of this object.
	sceneNode->addChild(child->sceneNode);
}

void AnnGameObject::detachFromParent() const
{
	sceneNode->getParentSceneNode()->removeChild(sceneNode);
	AnnGetEngine()->getSceneManager()->getRootSceneNode()->addChild(sceneNode);
}

bool AnnGameObject::checkForBodyInParent()
{
	return parentsHaveBody(this);
}

AnnVect3 AnnGameObject::getWorldPosition() const
{
#ifdef _DEBUG
	if (sceneNode->isCachedTransformOutOfDate())
	{
		AnnDebug() << "cached transform was out of date when " << name << " wanted it's own world position";
		return sceneNode->_getDerivedPositionUpdated();
	}
#endif
	return sceneNode->_getDerivedPosition();
}

AnnQuaternion AnnGameObject::getWorldOrientation() const
{
#ifdef _DEBUG
	if (sceneNode->isCachedTransformOutOfDate())
	{
		AnnDebug() << "cached transofrm was out of date when " << name << " wanted it's own world orientaiton";
		return sceneNode->_getDerivedOrientationUpdated();
	}
#endif
	return sceneNode->_getDerivedOrientation();
}

bool AnnGameObject::parentsHaveBody(AnnGameObject* obj) const
{
	if (!hasParent()) return false;
	const auto addr = obj->getParent().get();
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
	for (auto childNode : parentObj->sceneNode->getChildIterator())
	{
		const auto node = childNode;
		const auto childSceneNode = dynamic_cast<Ogre::SceneNode*>(node);

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
	model3D = item;
}

Ogre::Item* AnnGameObject::getItem() const
{
	return model3D;
}