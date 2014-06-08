#ifndef ANN_ENGINE
#define ANN_ENGINE
#undef DLL
//windows DLL
#ifdef DLLDIR_EX
   #define DLL  __declspec(dllexport)   /// export DLL information
#else
   #define DLL  __declspec(dllimport)   /// import DLL information
#endif

//bypass on linux
#ifdef __gnu_linux__
#undef DLL
#define DLL
#endif

//C++ STD & STL
#include <vector>
#include <sstream>
//Ogre 3D
#include <Ogre.h>
#include <OIS.h>
//Bullet
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <btBulletDynamicsCommon.h>
//CEGUI
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>
//OgreOculus by kojack
#include "OgreOculus.h"
//btOgre
#include "BtOgrePG.h"
#include "BtOgreGP.h"
#include "BtOgreExtras.h"
//Annwvyn
#include "AnnGameObject.hpp"
#include "AnnTriggerObject.hpp"
#include "AnnTypes.h"
#include "AnnTools.h"
#include "AnnAudioEngine.hpp"

#ifdef __gnu_linux__
#include <unistd.h>
#endif



namespace Annwvyn
{
	class DLL AnnEngine
	{
	public:
		///Class constructor. take the name of the window
		AnnEngine(const char title[] = "Annwvyn Game Engine");

		///Class destructor. Do clean up stuff.
		~AnnEngine();
        
        AnnEngine* getAddress() {return this;}
        void initCEGUI();
		///display config window and return an ogre root (create a new one by default)
		Ogre::Root* askSetUpOgre(Ogre::Root* root = new Ogre::Root);

		///load data to the ressource group manager
		void loadZip(const char path[]);
		void loadDir(const char path[]);
        void loadResFile(const char path[]);
		///init ressources groups
		void initRessources();

		///init OgreOculus stuff
		void oculusInit();

		///init the BodyParams variable
		static void initBodyParams(Annwvyn::bodyParams* bodyP,
			float eyeHeight = 1.65f,
			float walkSpeed = 3.0f,
			float turnSpeed = 0.003f,
			float mass = 80.0f,
			Ogre::Vector3 Position = Ogre::Vector3(0,0,10),
			Ogre::Quaternion HeadOrientation = Ogre::Quaternion(1,0,0,0),
			btCollisionShape* Shape = NULL,
			btRigidBody* Body = NULL);
		

		void initPlayerPhysics();
		///update player location/orientation from the bullet body
		void updatePlayerFromPhysics();

		///translate the player (DONT DETECT COLLISIONS HERE !!!)
		void translatePhysicBody(Ogre::Vector3 translation);

		///set player linear speed from a 3D vector
		void setPhysicBodyLinearSpeed(Ogre::Vector3 V);

		///create a game object form the name of an entity loaded on the ressource group manager.
		AnnGameObject* createGameObject(const char entityName[]);
        
        bool destroyGameObject(AnnGameObject* object);

		///set the ambiant light
		void setAmbiantLight(Ogre::ColourValue v);

		///add a light to the scene. return a pointer to the new light
		AnnLightObject* addLight();

		///calculate one frame of the game
		void renderOneFrame();

		///display bullet debuging drawing
		void setDebugPhysicState(bool state);///if state == true, display physics debug


		void runPhysics(); 

		///return true if the game want to terminate the program
		bool requestStop();

		///log something to the console. If flag = true (by default), will print "Annwvyn - " in front of the message
		static void log(std::string message, bool flag = true);

		///update camera position/orientation from rift and virtual body
		void updateCamera();

		///refresh all for you
		void refresh();

		///return a vector depending on WASD keys pressed
		bool processWASD(Ogre::Vector3* translate);///return the translation vector to aply on the body

		///caputre event form keyboard and mouse
		void captureEvents();///keyboard and mouse

		///update program time. retur the delay between the last call of this method
		float getTime();
        float getTimeFromStartUp();
        
        ///Step Objects animation
		void playObjectsAnnimation();


		///set the ground object
		void setGround(AnnGameObject* Ground);

		///return the Annwvyn OpenAL simplified audio engine
		AnnAudioEngine* getAudioEngine();

		///LowLevel OIS Mouse
		OIS::Mouse* getOISMouse();
		
        ///LowLevel OIS Keyboard
		OIS::Keyboard* getOISKeyboard();
		
        ///LowLevel OIS Joystick
        OIS::JoyStick* getOISJoyStick();

		///is key 'key' pressed ? (see OIS headers for KeyCode, generaly 'OIS::KC_X' where X is the key you want.
		bool isKeyDown(OIS::KeyCode key); ///this is simplier to use if you are on the OIS namespace ;-)

		///return true if you touch the ground
		bool collisionWithGround();
		
        ///process collision test form bullet manifold and objects collision mask
		void processCollisionTesting();
        
        ///get the dynamicsWorld
		btDiscreteDynamicsWorld* getDynamicsWorld();
        
        ///create a trigger object
		AnnTriggerObject* createTriggerObject();

        ///process contact test with triggers
		void processTriggersContacts();
        
        ///get ogre scene manager
		Ogre::SceneManager* getSceneManager();
        
        ///set the ogre material for the skydime with params
		void setSkyDomeMaterial(bool activate, const char materialName[], float curvature = 2.0f, float tiling = 1.0f);
        
        ///get the AnnObject the player is looking at
		Annwvyn::AnnGameObject* playerLookingAt();
        
        ///get the AnnGameObject form the given Ogre node
        Annwvyn::AnnGameObject* getFromNode(Ogre::SceneNode* node);

        ///get bodyParams
		Annwvyn::bodyParams* getBodyParams();
        
        ///get ogre camera scene node
		Ogre::SceneNode* getCamera();

        float getCentreOffset();

        void setReferenceQuaternion(Ogre::Quaternion q);
        Ogre::Quaternion getReferenceQuaternion();

        void attachVisualBody(const std::string entityName, bool flip=false, Ogre::Vector3 scale = Ogre::Vector3::UNIT_SCALE);

        void resetOculusOrientation();

    private:
        void setUpOgre(const char title[]);
        void setUpBullet();
        void setUpOIS();
        void setUpTime();
        void setUpAudio();
        void setUpGUI();
		
		void createVirtualBodyShape();
		void createPlayerPhysicalVirtualBody();
		void addPlayerPhysicalBodyToDynamicsWorld();

		float updateTime(); ///return deltaT
	private:
		Annwvyn::bodyParams* m_bodyParams;
		
		Ogre::Root* m_Root;
		Ogre::RenderWindow* m_Window;
		Ogre::SceneManager* m_SceneManager;
		Ogre::SceneNode* m_Camera;
        Ogre::SceneNode* VisualBodyAnchor;
		
		bool readyForLoadingRessources;

		Ogre::Entity* m_ent; ///only used for creating nodes into the smgr
		
		Oculus oculus;

		///dynamic container for games objects
		AnnGameObjectVect objects;
		std::vector <AnnTriggerObject*> triggers;

		///Events processing : 
		OIS::InputManager *m_InputManager ;
		OIS::Keyboard *m_Keyboard;
		OIS::Mouse *m_Mouse;
        OIS::JoyStick *m_Joystick;
		OIS::ParamList pl;

		size_t windowHnd;
		std::ostringstream windowHndStr;

		bool activateWASD;
		bool activateJump;
		float jumpForce;

		///time
		unsigned long last,now; ///Milisec
		float deltaT; ///Sec

		 ///bullet
		btBroadphaseInterface* m_Broadphase;
		btDefaultCollisionConfiguration* m_CollisionConfiguration;
		btCollisionDispatcher* m_Dispatcher;
		btSequentialImpulseConstraintSolver* m_Solver;
		btGhostPairCallback* m_ghostPairCallback;
		btDiscreteDynamicsWorld* m_DynamicsWorld;

		bool debugPhysics;
		BtOgre::DebugDrawer* m_debugDrawer;

		btQuaternion fixedBodyOrient;
        
        Ogre::Quaternion QuatReference;

		AnnGameObject* m_Ground;

		///Audio
		AnnAudioEngine* AudioEngine;

        ///gui
        CEGUI::OgreRenderer* m_CEGUI_Renderer;
	};
}
#endif ///ANN_ENGINE
