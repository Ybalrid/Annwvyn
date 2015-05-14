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

//Annwvyn
#include "AnnTypes.h"
#include "AnnAudioEngine.hpp"


namespace Annwvyn
{
    class DLL AnnGameObject; 

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

	///An object that exist in the game. Graphicaly and Potentialy Physicialy
    class DLL AnnGameObject
    {
        public:

            ///Class constructor 
            AnnGameObject();

            ///Class Destructor. Virutal.
            virtual ~AnnGameObject();

            ///Set position from spatial varaibles
			/// \param x X componant of the position vector
			/// \param y Y componant of the position vector
			/// \param z Z componant of the position vector
            void setPos(float x, float y, float z);

            ///Set position from Vector 3D
			/// \param pos 3D position vector. Relative to scene root position
            void setPos(Ogre::Vector3 pos);

            ///Translate
			/// \param x X componant of the translation vector
			/// \param y Y componant of the translation vector
			/// \param z Z componant of the translation vector
            void translate(float x, float y, float z);

            ///Set orientation from Quaternion components
			/// \param w W composant of a quaternion
			/// \param x X composant of a quaternion
			/// \param y Y composant of a quaternion
			/// \param z Z composant of a quaternion
            void setOrientation(float w, float x, float y, float z);

            ///Set Orientation from Quaternion
			/// \param orient Quaternion for aboslute orientation
            void setOrientation(Ogre::Quaternion orient);

            ///Set scale
			/// \param x X componant of the scale vector
			/// \param y Y componant of the scale vector
			/// \param z Z componant of the scale vector
            void setScale(float x, float y, float z);

            ///Set scale from Vector 3D
			/// \param scale Relative scaling factor
            void setScale(Ogre::Vector3 scale);

            ///Get Position
            Ogre::Vector3 pos();

            ///Get Orientation
            Ogre::Quaternion Orientation();

            ///Get Ogre Node
            Ogre::SceneNode* node();

            ///Get Ogre Entity
            Ogre::Entity* Entity();

            ///Get Physic Body
            btRigidBody* RigidBody();

            ///Get Rigid Body
            btRigidBody* getBody();

            ///Get shape
            btCollisionShape* getShape();

            ///Get distance from another object 
			/// \param otherObject The object we're counting the distance from
            float getDistance(AnnGameObject* otherObject);

            ///Play a sond file
			/// \param path Path to the audio file
			/// \param loop If set to true, will play the sound in loop
			/// \param volume Floating point number between 0 and 1 to set the loudness of the sound
            void playSound(std::string path, bool loop = false, float volume = 1.0f);

            ///collision handeling
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
			/// \param Object the objet we want to know the current collision state
            bool collideWith(AnnGameObject* Object);

            ///Set curently playing animation
			/// \param name Name of the animation as defined by the 3D entity
            void setAnimation(const char name[]);

            ///Set if we want to play the animation
			/// \param play the playing state we want to apply
            void playAnimation(bool play = true);

            ///Loop the animation ?
			/// \param loop the looping state of the animation
            void loopAnimation(bool loop = true);

            ///Apply a physical force            
            void applyForce(Ogre::Vector3 force);

            ///Apply a physical impultion
			/// \param the impultion force
            void applyImpulse(Ogre::Vector3 impulse);
			
			///Set the linear speed of the objet
			/// \param v The linear speed
			void setLinearSpeed(Ogre::Vector3 v);

            ///Set up Bullet 
            /// \param mass The mass of the object
			/// \param type The type of shape you want to define for the object
			void setUpBullet(float mass = 0, phyShapeType type = staticShape, bool colideWithPlayer = true);

            ///SetUpPhysics
            void setUpPhysics(float mass = 0, phyShapeType type = staticShape, bool colide = true){setUpBullet(mass,type,colide);}

			///Make the object visible
			void setVisible();

			///Make the object invisible
			void setInvisible();

			///Get if object visible
			bool isVisible();

        private:
            ///Make Annwvyn::AnnEngine acces these methods : 
            friend class AnnEngine;

            ///For engine : set node
            void setNode(Ogre::SceneNode* node);

            ///For engine : set Entity
            void setEntity(Ogre::Entity* entity);

            ///For engine : set bullet world
            void setBulletDynamicsWorld(btDiscreteDynamicsWorld* dynamicsWorld);

            ///For engine : get elapsed time
            void addTime(double offsetTime);

            ///For engine : update OpenAL source position
            void updateOpenAlPos();

        private:
			/**
			* You will certainly find strange to see that the Object does not cary a "position" vector.
			* We use the position of the Ogre Node ro the Bullet body to align the object.
			*
			* The reference is the position of the node. You can access it throug the getters/setters setPos() and getPos()
			*  
			* Same is true with the Orientation. We use Ogre node
			*/
			std::vector<collisionTest *> collisionMask;

            Ogre::SceneNode* m_node;
            Ogre::Entity* m_entity;
            Ogre::AnimationState* m_anim;

            bool animIsSetted;
            bool animIsPlaying;
            bool animIsLooping;

            btDiscreteDynamicsWorld* m_DynamicsWorld;
            btCollisionShape* m_Shape;
            btRigidBody* m_Body;
            bool bulletReady;

			///OpenAL audio source
            ALuint m_Source; 
			///OpenAL buffer
            ALuint m_Buffer; 

			///Name of the object
			std::string name;

			///True if the object is visible
			bool visible;

        public:
            ///Executed after object initialization
            virtual void postInit(){return;}

            ///Executed at refresh time (each frames)
            virtual void atRefresh(){return;}
    };
}
#endif
