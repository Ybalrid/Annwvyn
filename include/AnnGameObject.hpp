/**
 * \file AnnGameObject.hpp
 * \brief Game Object class
 * \author A. Brainville (Ybalrid)
 */

#pragma once

#include "systemMacro.h"
#include <string>
#include <BtOgrePG.h>

//Annwvyn
#include "AnnTypes.h"
#include "AnnAbstractMovable.hpp"
#pragma warning(default : 4996)

namespace Annwvyn
{
	class AnnDllExport AnnGameObjectManager;
	class AnnBehaviorScript;
	class AnnAudioSource;

	///An object that exist in the game. Graphically and Potentially Physically
	class AnnDllExport AnnGameObject : public AnnAbstractMovable
	{
	public:
		///Class constructor
		AnnGameObject();

		///Deleted AnnGameObject Copy Constructor
		AnnGameObject(const AnnGameObject&) = delete;

		///Class Destructor. Virtual.
		virtual ~AnnGameObject();

		///Set position from spatial variables
		/// \param x X component of the position vector
		/// \param y Y component of the position vector
		/// \param z Z component of the position vector
		void setPosition(float x, float y, float z);

		///Set position from Vector 3D
		/// \param pos 3D position vector. Relative to scene root position
		void setPosition(AnnVect3 pos) override;

		///Set the world position from a vector
		/// \param pos 3D position vector. Relative to scene root position
		void setWorldPosition(AnnVect3 pos) const;

		///Set the world position from a few floats
		/// \param x X component of the position vector
		/// \param y Y component of the position vector
		/// \param z Z component of the position vector
		void setWorldPosition(float x, float y, float z) const;

		///Translate
		/// \param x X component of the translation vector
		/// \param y Y component of the translation vector
		/// \param z Z component of the translation vector
		void translate(float x, float y, float z) const;

		///Set orientation from Quaternion components
		/// \param w W component of the quaternion
		/// \param x X component of the quaternion
		/// \param y Y component of the quaternion
		/// \param z Z component of the quaternion
		void setOrientation(float w, float x, float y, float z);

		///Set Orientation from Quaternion
		/// \param orient Quaternion for absolute orientation
		void setOrientation(AnnQuaternion orient) override;

		///Set the world orientation as a quaternion
		/// \param orient Quaternion for absolute orientation
		void setWorldOrientation(AnnQuaternion orient) const;

		///Set the world orientation as some floats
		/// \param w W component of the quaternion
		/// \param x X component of the quaternion
		/// \param y Y component of the quaternion
		/// \param z Z component of the quaternion
		void setWorldOrientation(float w, float x, float y, float z) const;

		///Set scale
		/// \param x X component of the scale vector
		/// \param y Y component of the scale vector
		/// \param z Z component of the scale vector
		/// \param scaleMass If set to true (by default) will update the mass of the rigid body to reflect the change in size (constant density)
		void setScale(float x, float y, float z, bool scaleMass = true) const;

		///Set scale from Vector 3D
		/// \param scale Relative scaling factor
		/// \param scaleMass will adjust the mass accoring to the scaling vector. true by default
		void setScale(AnnVect3 scale, bool scaleMass = true) const;

		///Get Position
		AnnVect3 getPosition() override;

		///Get the position in world
		AnnVect3 getWorldPosition() const;

		///Get Orientation
		AnnQuaternion getOrientation() override;

		///Get the world orientation
		AnnQuaternion getWorldOrientation() const;

		///Get scale
		AnnVect3 getScale() const;

		///Get Ogre Node
		Ogre::SceneNode* getNode() const;

		///Get the item of this object
		Ogre::Item* getItem() const;

		///Get Rigid rigidBody
		btRigidBody* getBody() const;

		///Get distance from another object
		/// \param otherObject The object we're counting the distance from
		float getDistance(AnnGameObject* otherObject) const;

		///Play a sound file
		/// \param name Name of the audio file in a resource location
		/// \param loop If set to true, will play the sound in loop
		/// \param volume Floating point number between 0 and 1 to set the loudness of the sound
		void playSound(const std::string& name, bool loop = false, float volume = 1.0f) const;

		///Set currently playing animation
		/// \param name Name of the animation as defined by the 3D entity
		void setAnimation(const std::string& name);

		///Set if we want to play the animation
		/// \param play the playing state we want to apply
		void playAnimation(bool play = true) const;

		///Loop the animation ?
		/// \param loop the looping state of the animation
		void loopAnimation(bool loop = true) const;

		///Apply a physical force
		/// \param force Force vector that will be applied to the center of mass of the object
		void applyForce(AnnVect3 force) const;

		///Apply a physical impulsion
		/// \param impulse the impulsion force
		void applyImpulse(AnnVect3 impulse) const;

		///Set the linear speed of the object
		/// \param v The linear speed
		void setLinearSpeed(AnnVect3 v) const;

		///Set the friction coefficient
		///See the "Results" table from this page : https://www.thoughtspike.com/friction-coefficients-for-bullet-physics/
		/// \param coef friction coef applied to this object's body
		void setFrictionCoef(float coef) const;

		///Set up Physics
		/// \param mass The mass of the object
		/// \param type The type of shape you want to define for the object
		/// \praam hasPlayerCollision if set to true (by default) object can colide with the player body representation
		void setupPhysics(float mass = 0, phyShapeType type = staticShape, bool hasPlayerCollision = true);

		///Make the object visible
		void setVisible() const;

		///Make the object invisible
		void setInvisible() const;

		///Return the name of the object
		std::string getName() const;

		///Attach a script to this object
		/// \param scriptName name of a script
		void attachScript(const std::string& scriptName);

		///Return true if node is attached to the node owned by another AnnGameObject
		bool hasParent() const;

		///Get the parent Game Object
		std::shared_ptr<AnnGameObject> getParent() const;

		///Attach an object to this object.
		/// \param child The object you want to attach
		void attachChildObject(std::shared_ptr<AnnGameObject> child) const;

		///Make the node independent to any GameObject
		void detachFromParent() const;

		///Recursively check if any parent has a body, if one is found, returns true
		bool checkForBodyInParent();

		///Recursively check if any child has a body, if one is found, returns true
		bool checkForBodyInChild();

	private:
		///The GameObjectManager populate the content of this object when it goes through it's initialization
		friend class AnnEngine;
		friend class AnnGameObjectManager;

		//------------------ local utility
		///Do the actual recursion of checkForBodyInParent
		/// \param obj object to check (for recursion)
		bool parentsHaveBody(AnnGameObject* obj) const;

		///Do the actual recursion of checkForBodyInChild
		/// \param obj object to check (for recursion)
		static bool childrenHaveBody(AnnGameObject* obj);

		//----------------- engine utility
		///For engine : set node
		/// \param node ogre SceneNode
		void setNode(Ogre::SceneNode* node);

		///For engine : set item
		/// \param item Ogre::Item
		void setItem(Ogre::Item* item);

		///For engine : get elapsed time
		/// \param offsetTime time to add to the animation
		void addAnimationTime(double offsetTime) const;

		///For engine : update OpenAL source position
		void updateOpenAlPos() const;

		///SceneNode. This also holds the position/orientation/scale of the object
		Ogre::SceneNode* sceneNode;

		///Entity
		Ogre::Item* model3D;

		///Currently selected animation
		Ogre::SkeletonAnimation* currentAnimation;

		///Bullet shape
		btCollisionShape* collisionShape;

		///Bullet rigid body
		btRigidBody* rigidBody;

		///Mass of the rigid body
		float bodyMass;

		///AnnAudioEngine audioSource;
		std::shared_ptr<AnnAudioSource> audioSource;

		///Name of the object
		std::string name;

		///RigidBodyState of this object
		BtOgre::RigidBodyState* state;

		///list of script objects
		std::vector<std::shared_ptr<AnnBehaviorScript>> scripts;

	public:
		///Executed after object initialization
		virtual void postInit() {}

		///Executed at refresh time (each frames)
		virtual void update() {}

		///Call the update methods of all the script present in the scripts container
		void callUpdateOnScripts();
	};

	using AnnGameObjectPtr = std::shared_ptr<AnnGameObject>;
}
