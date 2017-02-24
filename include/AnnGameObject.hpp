/**
 * \file AnnGameObject.hpp
 * \brief Game Object class
 * \author A. Brainville (Ybalrid)
 */

#ifndef ANNGAMEOBJECT
#define ANNGAMEOBJECT

#include "systemMacro.h"

#include <string>

#include <OgreVector3.h>
#include <OgreQuaternion.h>
#include <OgreSceneNode.h>
#include <OgreEntity.h>
#include <OgreItem.h>

#include "BtOgreGP.h"
#include "BtOgrePG.h"
#include "BtOgreExtras.h"

 //Annwvyn
#include "AnnTypes.h"
#include "AnnAudioEngine.hpp"
#include "AnnScriptManager.hpp"

#include "AnnAbstractMovable.hpp"
#pragma warning(default:4996)

namespace Annwvyn
{
	class DLL AnnGameObjectManager;

	///An object that exist in the game. Graphically and Potentially Physically
	class DLL AnnGameObject : public AnnAbstractMovable
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
		void setWorldPosition(AnnVect3 pos) const;

		///Set the world position from a few floats
		void setWorldPosition(float x, float y, float z) const;

		///Translate
		/// \param x X component of the translation vector
		/// \param y Y component of the translation vector
		/// \param z Z component of the translation vector
		void translate(float x, float y, float z) const;

		///Set orientation from Quaternion components
		/// \param w W component of a quaternion
		/// \param x X component of a quaternion
		/// \param y Y component of a quaternion
		/// \param z Z component of a quaternion
		void setOrientation(float w, float x, float y, float z);

		///Set Orientation from Quaternion
		/// \param orient Quaternion for absolute orientation
		void setOrientation(AnnQuaternion orient) override;

		///Set the world orientation as a quaternion
		void setWorldOrientation(AnnQuaternion orient) const;

		///Set the world orientation as some floats
		void setWorldOrientation(float w, float x, float y, float z) const;

		///Set scale
		/// \param x X component of the scale vector
		/// \param y Y component of the scale vector
		/// \param z Z component of the scale vector
		void setScale(float x, float y, float z) const;

		///Set scale from Vector 3D
		/// \param scale Relative scaling factor
		void setScale(AnnVect3 scale) const;

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

		///Get Ogre Entity
		DEPRECATED Ogre::Item* getEntity() const { return getItem(); };
		Ogre::Item* getItem() const;

		///Get Rigid Body
		btRigidBody* getBody() const;

		///Get distance from another object
		/// \param otherObject The object we're counting the distance from
		float getDistance(AnnGameObject* otherObject) const;

		///Play a sound file
		/// \param path Path to the audio file
		/// \param loop If set to true, will play the sound in loop
		/// \param volume Floating point number between 0 and 1 to set the loudness of the sound
		void playSound(std::string path, bool loop = false, float volume = 1.0f) const;

		// TODO create animation state machine
		///Set currently playing animation
		/// \param name Name of the animation as defined by the 3D entity
		void setAnimation(const char name[]);

		// TODO create animation state machine
		///Set if we want to play the animation
		/// \param play the playing state we want to apply
		void playAnimation(bool play = true);

		// TODO create animation state machine
		///Loop the animation ?
		/// \param loop the looping state of the animation
		void loopAnimation(bool loop = true);

		///Apply a physical force
		void applyForce(AnnVect3 force) const;

		///Apply a physical impulsion
		/// \param impulse the impulsion force
		void applyImpulse(AnnVect3 impulse) const;

		///Set the linear speed of the object
		/// \param v The linear speed
		void setLinearSpeed(AnnVect3 v) const;

		///Set up Physics
		/// \param mass The mass of the object
		/// \param type The type of shape you want to define for the object
		void setUpPhysics(float mass = 0, phyShapeType type = staticShape, bool colide = true);

		///Make the object visible
		void setVisible() const;

		///Make the object invisible
		void setInvisible() const;

		///Return the name of the object
		std::string getName() const;

		///Attach a script to this object
		void attachScript(const std::string& scriptName);

		///Return true if node is attached to the node owned by another AnnGameObject
		bool hasParent() const;

		///Get the parent Game Object
		std::shared_ptr<AnnGameObject> getParent() const;

		///Attach an object to this object.
		void attachChildObject(std::shared_ptr<AnnGameObject> child) const;

		///Make the node independent to any GameObject
		void detachFromParent() const;

		///Recursively check if any parent has a body, if one is found, returns true
		bool checkForBodyInParent();

		///Recursively check if any child has a body, if one is found, returns true
		bool checkForBodyInChild();

	private:

		///Do the actual recursion of checkForBodyInParent
		bool parentsHaveBody(AnnGameObject* obj) const;

		///Do the actual recursion of checkForBodyInChild
		static bool childrenHaveBody(AnnGameObject* obj);

		///Make Annwvyn::AnnEngine access these methods :
		friend class AnnEngine;
		friend class AnnGameObjectManager;

		///For engine : set node
		void setNode(Ogre::SceneNode* node);

		///For engine : set Entity
		void setItem(Ogre::Item* item);

		void setPhysicsMesh(Ogre::v1::MeshPtr mesh);

		// TODO create animation state machine
		///For engine : get elapsed time
		void addAnimationTime(double offsetTime) const;

		///For engine : update OpenAL source position
		void updateOpenAlPos() const;

		/**
		* You will certainly find strange to see that the Object does not carry a "position" vector.
		* We use the position of the Ogre Node or the Bullet body to align the object.
		*
		* The reference is the position of the node. You can access it through the getters/setters setPos() and getPos()
		*
		* Same is true with the Orientation. We use Ogre node
		*/

		///SceneNode
		Ogre::SceneNode* Node;

		///Entity
		Ogre::Item* Model;
		Ogre::v1::MeshPtr v1mesh;

		/*// TODO create animation state machine
		bool animIsSetted;
		bool animIsPlaying;
		bool animIsLooping;
		Ogre::AnimationState* anim;*/

		btCollisionShape* Shape;
		btRigidBody* Body;

		///AnnAudioEngine audioSource;
		std::shared_ptr<AnnAudioSource> audioSource;

		///Name of the object
		std::string name;

		///True if the object is visible
		BtOgre::RigidBodyState *state;

		///list of script objects
		std::vector<std::shared_ptr<AnnBehaviorScript>> scripts;

	public:
		///Executed after object initialization
		virtual void postInit() { }

		///Executed at refresh time (each frames)
		virtual void atRefresh() { }

		///Call the update methods of all the script present in the scripts container
		void callUpdateOnScripts();
	};
}
#endif