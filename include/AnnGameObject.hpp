#ifndef ANNGAMEOBJECT
#define ANNGAMEOBJECT
#undef DLL
//windows DLL
#ifdef DLLDIR_EX
   #define DLL  __declspec(dllexport)   // export DLL information
#else
   #define DLL  __declspec(dllimport)   // import DLL information
#endif

//bypass on linux
#ifdef __gnu_linux__
#undef DLL
#define DLL
#endif


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
//#include "AnnEngine.hpp"
#include "AnnAudioEngine.hpp"

namespace Annwvyn
{
	class DLL AnnGameObject; 

	struct collisionTest
	{
		AnnGameObject* Object;
		AnnGameObject* Receiver;
		bool collisionState;
	};

	class DLL AnnGameObject
	{
	public:
		AnnGameObject();
        ~AnnGameObject();
		//Setters pos
		void setPos(float x, float y, float z);
		void setPos(Ogre::Vector3 pos);

		void translate(float x, float y, float z);

		void setOrientation(float w, float x, float y, float z);
		void setOrientation(Ogre::Quaternion orient);
        
        void setScale(float x, float y, float z);
        void setScale(Ogre::vector3 scale);

		//Getters pos

		Ogre::Vector3 pos();
		Ogre::Quaternion Orientation();

		//Setters engine
		
		void setNode(Ogre::SceneNode* node);

		void setEntity(Ogre::Entity* entity);

		void setBulletDynamicsWorld(btDiscreteDynamicsWorld* dynamicsWorld);

		void setUpBullet(float mass = 0, phyShapeType type = staticShape);

		void setAudioEngine(AnnAudioEngine* AudioEngine);

		float getDistance(AnnGameObject* otherObject);

		//lowlevel getters engine
		Ogre::SceneNode* node();
		Ogre::Entity* Entity();
		btRigidBody* RigidBody();

		//utility 
		void stepBulletSimulation();

		void playSound(std::string path, bool loop = false, float volume = 1.0f);
		void updateOpenAlPos();

		btRigidBody* getBody();
		btCollisionShape* getShape();

		//collision handeling
		std::vector<struct collisionTest*> getCollisionMask();
		
		//set all collisionState to false
		void resetCollisionMask();
		
		//empty the vector
		void cleanCollisionMask();

		//remove this object of the collisionMask
		void stopGettingCollisionWith(AnnGameObject* Object);

		//add this object to the collisionState
		void testCollisionWith(AnnGameObject* Object);

		//change the collisionState
		void updateCollisionStateWith(AnnGameObject* Object, bool collisionState);

		//return the collisionState with the object from the collisionMask. if the object is not on the collisionMask, return false
		bool collideWith(AnnGameObject* Object);//
		
		void setAnimation(const char name[]);
		void playAnimation(bool play = true);
		void loopAnimation(bool loop = true);
		void addTime(float offsetTime);

	protected:
		Ogre::SceneNode* m_node;
		Ogre::Entity* m_entity;
		Ogre::AnimationState* m_anim;
		//std::vector<Ogre::AnimationState*> anims;
		bool animIsSetted;
		bool animIsPlaying;
		bool animIsLooping;

	

		btDiscreteDynamicsWorld* m_DynamicsWorld;
		btCollisionShape* m_Shape;
		btRigidBody* m_Body;
		bool bulletReady;

		std::vector<struct collisionTest*> collisionMask;

		AnnAudioEngine* m_AudioEngine;
		ALuint m_Source; //OpenAL audio source
		ALuint m_Buffer; //OpenAL buffer
	};

	
}

#endif
