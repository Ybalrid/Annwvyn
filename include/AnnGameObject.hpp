/**
 * \file AnnGameObject.hpp
 * \brief Game Object class
 * \author A. Brainville (Ybalrid)
 */

#ifndef ANNGAMEOBJECT
#define ANNGAMEOBJECT


#include "systemMacro.h"

#include <string>

//Ogre3D
#include <Ogre.h>
//Bullet
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
//btOgre
#include "BtOgrePG.h"
#include "BtOgreGP.h"
#include "BtOgreExtras.h"
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
            void setPos(float x, float y, float z);

            ///Set position from Vector 3D
            void setPos(Ogre::Vector3 pos);

            ///Translate 
            void translate(float x, float y, float z);

            ///Set orientation from Quaternion components
            void setOrientation(float w, float x, float y, float z);

            ///Set Orientation from Quaternion
            void setOrientation(Ogre::Quaternion orient);

            ///Set scale
            void setScale(float x, float y, float z);

            ///Set scale from Vector 3D
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
            float getDistance(AnnGameObject* otherObject);

            ///Play a sond file
            void playSound(std::string path, bool loop = false, float volume = 1.0f);

            ///collision handeling
            std::vector<struct collisionTest*> getCollisionMask();

            ///Set all collisionState to false
            void resetCollisionMask();

            ///empty the vector
            void cleanCollisionMask();

            ///remove this object of the collisionMask
            void stopGettingCollisionWith(AnnGameObject* Object);

            ///add this object to the collisionState
            void testCollisionWith(AnnGameObject* Object);

            ///change the collisionState
            void updateCollisionStateWith(AnnGameObject* Object, bool collisionState);

            ///return the collisionState with the object from the collisionMask. 
            //if the object is not on the collisionMask, return false
            bool collideWith(AnnGameObject* Object);

            ///Set curently playing animation
            void setAnimation(const char name[]);

            ///Play the animation ?
            void playAnimation(bool play = true);

            ///Loop the animation ?
            void loopAnimation(bool loop = true);

            ///Apply a physical force            
            void applyForce(Ogre::Vector3 force);

            ///Apply a physical impultion
            void applyImpulse(Ogre::Vector3 impulse);

			void setLinearSpeed(Ogre::Vector3 v);

            ///Set up Bullet 
            void setUpBullet(float mass = 0, phyShapeType type = staticShape);

            ///SetUpPhysics
            void setUpPhysics(float mass = 0, phyShapeType type = staticShape){setUpBullet(mass,type);}

        private:
            ///Make Annwvyn::AnnEngine acces these methods : 
            friend class AnnEngine;

			void setTimePtr(float* timePtr);


            ///For engine : set node
            void setNode(Ogre::SceneNode* node);
			
			///Set the GameEngine pointer. 
			void setEngine(AnnEngine* e);

            ///For engine : set Entity
            void setEntity(Ogre::Entity* entity);

            ///For engine : set bullet world
            void setBulletDynamicsWorld(btDiscreteDynamicsWorld* dynamicsWorld);

            ///For engine : get elapsed time
            void addTime(float offsetTime);

            ///For engine : update OpenAL source position
            void updateOpenAlPos();

            ///For engine set Audio engine    
			void setAudioEngine(AnnAudioEngine* AudioEngine);

			///Make the object visible
			void setVisible();

			///Make the object invisible
			void setInvisible();

			///Get if object visible
			bool isVisible();

        private:
			/**
			* You will certainly find strange to see that the Object does not cary a "position" vector.
			* We use the position of the Ogre Node ro the Bullet body to align the object.
			*
			* The reference is the position of the node. You can access it throug the getters/setters setPos() and getPos()
			*  
			* Same is true with the Orientation. We use Ogre node
			*/
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

            std::vector<collisionTest * > collisionMask;

			///Audio Engine 
            AnnAudioEngine* m_AudioEngine;
			///OpenAL audio source
            ALuint m_Source; 
			///OpenAL buffer
            ALuint m_Buffer; 

			float* time;

			Ogre::Vector3 visualLinearSpeed;

			std::string name;


			bool visible;

        public:
            ///Executed after object initialization
            virtual void postInit(){return;}

            ///Executed at refresh time (each frames)
            virtual void atRefresh(){return;}
    };
}
#endif
