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
	class AnnEngine; //predeclaration of AnnEngine

	///Parameters of the user's VirtualBody		
	class DLL bodyParams
	{
		/**
		* That class was a structure. It is keeped as a class with public attributes to keep compatibility with legacy code.
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

	///Correspondance between array position and walk direction for the "walking" array
	enum walkDirection{forward, backward, left, right};


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
		/// \param Position 3D vector representing the position of the player (refernced by the point between his eyes)
		void setPosition(AnnVect3 Position);

		///Set body orientation
		/// \param Orientation Euler angle that represent the orientation of the player's BODY. That orientation is used as a "zero point" for the head orientation.
		void setOrientation(Ogre::Euler Orientation);

		///Set the head orientation
		/// \param HeadOrientation A quaternion representing the orientation of the head
		void setHeadOrientation(AnnQuaternion HeadOrientation);

		///distance between footplane and eyes
		/// \param eyeHeight floating point number in metter
		void setEyesHeight(float eyeHeight);

		///WalkSpeed, metters by second
		/// \param walkSpeed The speed a the user is walking
		void setWalkSpeed(float walkSpeed);

		///Turnspeed
		/// \param turnSpeed Angular speed that the user can turn his body
		void setTurnSpeed(float turnSpeed);

		///Mass in Kg
		/// \param mass Mass of the player in Kg 
		void setMass(float mass);

		///Bullet shape
		/// \param Shape the Bullet collision shape used to simulate player physics
		void setShape(btCollisionShape* Shape);

		///Bullet RIGID body
		/// \param The bullet rigid body used for simulating player's physics
		void setBody(btRigidBody* Body);

		///Get the distance between footplane and eyes in metters
		float getEyesHeight();

		///Get walkspeed in metter/seconds
		float getWalkSpeed();

		///Get turnspeed in rad/seconds
		float getTurnSpeed();

		///Get mass in Kg
		float getMass();

		///Get position vector
		AnnVect3 getPosition();

		///Get body orientation (euler vector)
		Ogre::Euler getOrientation();

		///Get rigid body
		btRigidBody* getBody();

		///Get Shape
		btCollisionShape* getShape();

		///Apply a relative yaw transform to the player. Usefull to bind it to the mouse X axis for FPS-like gameplay.
		/// \param angle Radian angle of the transformaton.
		void applyRelativeBodyYaw(Ogre::Radian angle);

		///Apply the rotation from the mouse relative value
		/// \param relValue Relative value in pixels of the mouse linear mouvement
		void applyMouseRelativeRotation(int relValue);

		///Return true if physics has been initialized once
		bool hasPhysics();

		///Get the translation vector (normalised) from the walking state
		AnnVect3 getTranslation();

		///Get the translation from analog joystick value
		AnnVect3 getAnalogTranslation();

		///Set the player actuator object
		void setActuator(AnnPlayerActuator* act);

		///Boolean false if the player can get orientation transformation from 
		bool standing;

		///Get the ratio between walking and running speed
		float getRunFactor();

		///If the player is handeled throug the physics engine, this method will detach the rigidbody from the camera,
		///remove it from the dynamics world, unalocate it from the memory and recreate it from scratch. This is usefull for
		///"teleporting" the player, for example if you need to reset his position.
		void resetPlayerPhysics();


		void teleport(AnnVect3 position, AnnRadian orientation);
		void teleport(AnnVect3 position);


	protected:

		///Object that keep body parameters (= legacy structure)
		bodyParams* playerBody;

	private:

		///Give back the right to modify some parameters
		void unlockParameters();

		///Give Annwvyn::AnnEngine the rght to access private members
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

		///time lengh of the frame 
		double updateTime;

		///Player body physics enabled
		bool physics;

		///PlayerActuator to use
		AnnPlayerActuator* actuator;

	public:

		///Turning that off bypass the physics code. Cool for menu scene or weird manipulation of the player object
		bool ignorePhysics;
		
		///Waling state. Forward Backward Left Right
		bool walking[4];

		///Runing state
		bool run;

		//Analog values between -1 and 1
		float analogWalk;
		float analogStraff;
		float analogRotate;
	}; 

}
#endif //ANN_PLAYER
