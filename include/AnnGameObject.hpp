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
	class DLL AnnGameObject;
	class DLL AnnGameObjectManager;

	// TODO get rid of
	///Simple collision state representation (pointer between 2 objects and a boolean)
	struct collisionTest
	{
		///The object tested
		AnnGameObject* Object;

		///The base object
		AnnGameObject* Receiver;

		///Contact or not ?
		bool collisionState;
	};

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
		void setPosition(AnnVect3 pos);

		///Translate
		/// \param x X component of the translation vector
		/// \param y Y component of the translation vector
		/// \param z Z component of the translation vector
		void translate(float x, float y, float z);

		///Set orientation from Quaternion components
		/// \param w W component of a quaternion
		/// \param x X component of a quaternion
		/// \param y Y component of a quaternion
		/// \param z Z component of a quaternion
		void setOrientation(float w, float x, float y, float z);

		///Set Orientation from Quaternion
		/// \param orient Quaternion for absolute orientation
		void setOrientation(AnnQuaternion orient);

		///Set scale
		/// \param x X component of the scale vector
		/// \param y Y component of the scale vector
		/// \param z Z component of the scale vector
		void setScale(float x, float y, float z);

		///Set scale from Vector 3D
		/// \param scale Relative scaling factor
		void setScale(AnnVect3 scale);

		///Get Position
		AnnVect3 getPosition();

		///Get Orientation
		AnnQuaternion getOrientation();

		///Get scale
		AnnVect3 getScale();

		///Get Ogre Node
		Ogre::SceneNode* getNode();

		///Get Ogre Entity
		Ogre::Entity* getEntity();

		///Get Rigid Body
		btRigidBody* getBody();

		///Get distance from another object
		/// \param otherObject The object we're counting the distance from
		float getDistance(AnnGameObject* otherObject);

		///Play a sound file
		/// \param path Path to the audio file
		/// \param loop If set to true, will play the sound in loop
		/// \param volume Floating point number between 0 and 1 to set the loudness of the sound
		void playSound(std::string path, bool loop = false, float volume = 1.0f);

		///collision handling
		std::vector<struct collisionTest*> getCollisionMask();

		///Set all collisionState to false
		void resetCollisionMask();

		///empty the vector
		void cleanCollisionMask();

		///remove this object of the collisionMask
		/// \param Object Object we don't want to know if we collide anymore
		void stopGettingCollisionWith(AnnGameObject* Object);

		///add this object to the collisionState
		/// \param Object the object we want to know collision information
		void testCollisionWith(AnnGameObject* Object);

		///change the collisionState
		/// \param Object the object we are testing
		/// \param collisionState the state of the collision. True if contact.
		void updateCollisionStateWith(AnnGameObject* Object, bool collisionState);

		///return the collisionState with the object from the collisionMask.
		///if the object is not on the collisionMask, return false
		/// \param Object the object we want to know the current collision state
		bool collideWith(AnnGameObject* Object);

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
		void applyForce(AnnVect3 force);

		///Apply a physical impulsion
		/// \param the impulsion force
		void applyImpulse(AnnVect3 impulse);

		///Set the linear speed of the object
		/// \param v The linear speed
		void setLinearSpeed(AnnVect3 v);

		///Set up Physics
		/// \param mass The mass of the object
		/// \param type The type of shape you want to define for the object
		void setUpPhysics(float mass = 0, phyShapeType type = staticShape, bool colide = true);

		///Make the object visible
		void setVisible();

		///Make the object invisible
		void setInvisible();

		///Get if object visible
		bool isVisible();

		///Set the ID of the GameObject
		void setID(std::string ID);

		///Get the ID of the Object
		std::string getID();

		///
		std::string getName();

	private:
		///Make Annwvyn::AnnEngine access these methods :
		friend class AnnEngine;
		friend class AnnGameObjectManager;

		///For engine : set node
		void setNode(Ogre::SceneNode* node);

		///For engine : set Entity
		void setEntity(Ogre::Entity* entity);

		///For engine : set bullet world
		void setBulletDynamicsWorld(btDiscreteDynamicsWorld* dynamicsWorld);

		// TODO create animation state machine
		///For engine : get elapsed time
		void addAnimationTime(double offsetTime);

		///For engine : update OpenAL source position
		void updateOpenAlPos();

	private:
		/**
		* You will certainly find strange to see that the Object does not carry a "position" vector.
		* We use the position of the Ogre Node or the Bullet body to align the object.
		*
		* The reference is the position of the node. You can access it through the getters/setters setPos() and getPos()
		*
		* Same is true with the Orientation. We use Ogre node
		* TODO rework collision feedback system
		*/
		std::vector<collisionTest *> collisionMask;

		Ogre::SceneNode* Node;
		Ogre::Entity* Entity;

		// TODO create animation state machine
		bool animIsSetted;
		bool animIsPlaying;
		bool animIsLooping;
		Ogre::AnimationState* anim;

		btDiscreteDynamicsWorld* DynamicsWorld;
		btCollisionShape* Shape;
		btRigidBody* Body;
		bool bulletReady;

		///AnnAudioEngine audioSource;
		std::shared_ptr<AnnAudioSource> audioSource;

		///Name of the object
		std::string name;
		std::string id;

		///True if the object is visible
		bool visible;
		BtOgre::RigidBodyState *state;

	public:
		///Executed after object initialization
		virtual void postInit() { return; }

		///Executed at refresh time (each frames)
		virtual void atRefresh() { return; }
	};
}
#endif