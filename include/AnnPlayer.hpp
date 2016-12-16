/**
* \file AnnPlayer.hpp
* \brief class that represent the player
* \author A. Brainville (Ybalrid)
*/

#ifndef ANN_PLAYER
#define ANN_PLAYER

#include "systemMacro.h"
#include "AnnVect3.hpp"
#include "AnnQuaternion.hpp"
#include "AnnPlayerActuator.hpp"

#include "euler.h"
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#define DEFAULT_STARTING_POS AnnVect3(0,1,10)
#define DEFAULT_STARTING_ORIENT Ogre::Euler(0)
namespace Annwvyn
{
	enum AnnPlayerMode { STANDING, ROOMSCALE };
	class AnnEngine; //pre-declaration of AnnEngine

	///Parameters of the user's VirtualBody
	class DLL bodyParams
	{
		/**
		* That class was a structure. It is kept as a class with public attributes to keep compatibility with legacy code.
		*/
	public:
		///Constructor that handle the default body parameters.
		bodyParams();

		float eyeHeight;
		float walkSpeed;
		float runFactor;
		float turnSpeed;
		float mass;
		AnnVect3 FeetPosition;
		Ogre::Euler Orientation;
		AnnQuaternion HeadOrientation;

		//bullet
		btCollisionShape* Shape;
		btRigidBody* Body;

		AnnVect3 getHeadPosition() { return (FeetPosition + eyeHeight*AnnVect3::UNIT_Y); }
	};

	///Correspondence between array position and walk direction for the "walking" array
	enum walkDirection { forward, backward, left, right };

	///class that represent the player. This is the user's "Virtual body" in the world. It's the object that you have to move and turn to explore the space.
	class DLL AnnPlayer
	{
	public:

		///Construct the player object
		AnnPlayer();

		///Destroy the player object
		~AnnPlayer();

		///Prevent modification of physical parameter
		void lockParameters();

		///Permit to know if parameters are locked
		bool isLocked();

		///Set the position of the player. If physics is enabled you need to call  AnnEngine::resetPlayerPhysics() to recreate player's body.
		/// \param Position 3D vector representing the position of the player (referenced by the point between his eyes)
		void setPosition(AnnVect3 Position);

		///Set body orientation
		/// \param Orientation Euler angle that represent the orientation of the player's BODY. That orientation is used as a "zero point" for the head orientation.
		void setOrientation(Ogre::Euler Orientation);

		///Set the head orientation
		/// \param HeadOrientation A quaternion representing the orientation of the head
		void setHeadOrientation(AnnQuaternion HeadOrientation);

		///distance between foot-plane and eyes
		/// \param eyeHeight floating point number in meter
		void setEyesHeight(float eyeHeight);

		///WalkSpeed, meters by second
		/// \param walkSpeed The speed a the user is walking
		void setWalkSpeed(float walkSpeed);

		///Turn-speed
		/// \param turnSpeed Angular speed that the user can turn his body
		void setTurnSpeed(float turnSpeed);

		///Mass in Kg
		/// \param mass Mass of the player in Kg
		void setMass(float mass);

		///Bullet shape
		/// \param Shape the Bullet collision shape used to simulate player physics
		void setShape(btCollisionShape* Shape);

		///Bullet RIGID body
		/// \param Body The bullet rigid body used for simulating player's physics
		void setBody(btRigidBody* Body);

		///Get the distance between foot-plane and eyes in meters
		float getEyesHeight();

		///Get a vector that correspond to the translation between the feet point and the eyes
		AnnVect3 getEyeTranslation();

		///Get walk-speed in meter/seconds
		float getWalkSpeed();

		///Get turn-speed in rad/seconds
		float getTurnSpeed();

		///Get mass in Kg
		float getMass();

		///Get position vector
		AnnVect3 getPosition();

		///Get body orientation (Euler vector)
		Ogre::Euler getOrientation();

		///Get rigid body
		btRigidBody* getBody();

		///Get Shape
		btCollisionShape* getShape();

		///Apply a relative yaw transform to the player. Useful to bind it to the mouse X axis for FPS-like gameplay.
		/// \param angle Radian angle of the transformation.
		void applyRelativeBodyYaw(Ogre::Radian angle);

		///Apply the rotation from the mouse relative value
		/// \param relValue Relative value in pixels of the mouse linear movement
		void applyMouseRelativeRotation(int relValue);

		///Return true if physics has been initialized once
		bool hasPhysics();

		///Get the translation vector (normalized) from the walking state
		AnnVect3 getTranslation();

		///Get the translation from analog joystick value
		AnnVect3 getAnalogTranslation();

		///Set the player actuator object
		void setActuator(AnnPlayerActuator* act);

		///Boolean false if the player can get orientation transformation from
		bool standing;

		///Get the ratio between walking and running speed
		float getRunFactor();

		///If the player is handled through the physics engine, this method will detach the rigid-body from the camera,
		///remove it from the dynamics world, de-allocate it from the memory and recreate it from scratch. This is useful for
		///"teleporting" the player, for example if you need to reset his position.
		void resetPlayerPhysics();

		///Teleport the player to the given location, and get it facing the given direction
		void teleport(AnnVect3 position, AnnRadian orientation);

		///Teleport the player without touching it's direction
		void teleport(AnnVect3 position);

		///Set the player mode between standing and roomscale;
		void setMode(AnnPlayerMode playerMode);

		void setRoomRefNode(Ogre::SceneNode* node);

		void reground(float YvalueForGround);
		void reground(AnnVect3 pointOnGround);
		void regroundOnPhysicsBody(float lenght = 1000, AnnVect3 preoffset = AnnVect3::ZERO);

	protected:

		///Object that keep body parameters (= legacy structure)
		bodyParams* playerBody;

	private:

		AnnPlayerMode mode;

		///Give back the right to modify some parameters
		void unlockParameters();

		///Give Annwvyn::AnnEngine the right to access private members
		friend class AnnEngine;
		friend class AnnPhysicsEngine;

		///Engine update call for each frame
		void engineUpdate(float time);

		///Get the pointer to bodyParams (compatibility with legacy code here. Highly dangerous, do not mess with whatever is pointed by that vector. Seriously.
		bodyParams* getLowLevelBodyParams();

		///The famous boolean that permit to prevent YOU for breaking my work! :D
		bool locked;

		///If there is contact with ground
		bool contactWithGround;

		///Apply yaw from analog value
		void applyAnalogYaw();

		///time length of the frame
		double updateTime;

		///Player body physics enabled
		bool physics;

		///PlayerActuator to use
		AnnPlayerActuator* actuator;

		Ogre::SceneNode* RoomReferenceNode;

	public:

		///Turning that off bypass the physics code. Cool for menu scene or weird manipulation of the player object
		bool ignorePhysics;

		///Waling state. Forward Backward Left Right
		bool walking[4];

		///Running state
		bool run;

		//Analog values between -1 and 1
		float analogWalk;
		float analogStraff;
		float analogRotate;
	};
}
#endif //ANN_PLAYER