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

#include "OgreOculusRender.hpp"
//C++ STD & STL
#include <vector>
#include <sstream>
#include <cassert>
//Ogre 3D
#include <Ogre.h>
#include <OIS.h>
//Bullet
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <btBulletDynamicsCommon.h>
//OgreOculus by kojack
//#include "OgreOculus.h"


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
            ///Display config window and return an ogre root (create a new one by default)
            //Ogre::Root* askSetUpOgre(Ogre::Root* root = new Ogre::Root);

            ///Load data to the ressource group manager
            void loadZip(const char path[]);
            void loadDir(const char path[]);
            void loadResFile(const char path[]);
            ///Init ressources groups
            void initRessources();

            ///Init OgreOculus stuff
            void oculusInit();

            ///Init the BodyParams variable
            static void initBodyParams(Annwvyn::bodyParams* bodyP,
                    float eyeHeight = 1.59f,
                    float walkSpeed = 3.0f,
                    float turnSpeed = 0.003f,
                    float mass = 80.0f,
                    Ogre::Vector3 Position = Ogre::Vector3(0,0,10),
                    Ogre::Quaternion HeadOrientation = Ogre::Quaternion(1,0,0,0),
                    btCollisionShape* Shape = NULL,
                    btRigidBody* Body = NULL);


            void initPlayerPhysics();
            ///Update player location/orientation from the bullet body
            void updatePlayerFromPhysics();

            ///Translate the player (DONT DETECT COLLISIONS HERE !!!)
            void translatePhysicBody(Ogre::Vector3 translation);

            ///Set player linear speed from a 3D vector
            void setPhysicBodyLinearSpeed(Ogre::Vector3 V);

            ///Create a game object form the name of an entity.
            AnnGameObject* createGameObject(const char entityName[], AnnGameObject* object = new AnnGameObject);

            bool destroyGameObject(AnnGameObject* object);

            ///Set the ambiant light
            void setAmbiantLight(Ogre::ColourValue v);

            ///Add a light to the scene. return a pointer to the new light
            AnnLightObject* addLight();

            ///Calculate one frame of the game
            void renderOneFrame();

            ///Display bullet debuging drawing
            void setDebugPhysicState(bool state);///if state == true, display physics debug

            ///Run objects physics
            void runPhysics(); 

            ///Return true if the game want to terminate the program
            bool requestStop();

            ///Log something to the console. If flag = true (by default), will print "Annwvyn - " in front of the message
            static void log(std::string message, bool flag = true);

            ///Update camera position/orientation from rift and virtual body
            void updateCamera();

            ///Refresh all for you
            void refresh();

            ///Return a vector depending on WASD keys pressed
            bool processWASD(Ogre::Vector3* translate);///return the translation vector to aply on the body

            ///Caputre event form keyboard and mouse
            void captureEvents();///keyboard and mouse

            ///Update program time. retur the delay between the last call of this method
            float getTime();

            ///Get elapsed time from engine startup
            float getTimeFromStartUp();

            ///Step Objects animation
            void playObjectsAnnimation();

            ///Set the ground object
            void setGround(AnnGameObject* Ground);

            ///Return the Annwvyn OpenAL simplified audio engine
            AnnAudioEngine* getAudioEngine();

            ///LowLevel OIS Mouse
            OIS::Mouse* getOISMouse();

            ///LowLevel OIS Keyboard
            OIS::Keyboard* getOISKeyboard();

            ///LowLevel OIS Joystick
            OIS::JoyStick* getOISJoyStick();

            ///Is key 'key' pressed ? (see OIS headers for KeyCode, generaly 'OIS::KC_X' where X is the key you want.
            bool isKeyDown(OIS::KeyCode key); ///this is simplier to use if you are on the OIS namespace ;-)

            ///Return true if you touch the ground
            bool collisionWithGround();

            ///Process collision test form bullet manifold and objects collision mask
            void processCollisionTesting();

            ///Get the dynamicsWorld
            btDiscreteDynamicsWorld* getDynamicsWorld();

            ///Create a trigger object
            AnnTriggerObject* createTriggerObject(AnnTriggerObject* trigger = new AnnTriggerObject);

            ///Process contact test with triggers
            void processTriggersContacts();

            ///Get ogre scene manager
            Ogre::SceneManager* getSceneManager();

            ///Set the ogre material for the skydime with params
            void setSkyDomeMaterial(bool activate, const char materialName[], float curvature = 2.0f, float tiling = 1.0f);

            ///Get the AnnObject the player is looking at
            Annwvyn::AnnGameObject* playerLookingAt();

            ///Get the AnnGameObject form the given Ogre node
            Annwvyn::AnnGameObject* getFromNode(Ogre::SceneNode* node);

            ///Get bodyParams
            Annwvyn::bodyParams* getBodyParams();

            ///Get ogre camera scene node
            Ogre::SceneNode* getCamera();
            
            ///Get offset between viewport and distortion centre
            float getCentreOffset();

            void setReferenceQuaternion(Ogre::Quaternion q);
            Ogre::Quaternion getReferenceQuaternion();

            void attachVisualBody(const std::string entityName, 
                    float z_offset = -0.0644f, 
                    bool flip = false, 
                    bool animated = false, 
                    Ogre::Vector3 scale = Ogre::Vector3::UNIT_SCALE);

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
            
            ///Unable to continue, we have to cleanly cut the program before creating an error
            void emergency(void);            
        private:
            Annwvyn::bodyParams* m_bodyParams;

            //Ogre::Root* m_Root;
            Ogre::RenderWindow* m_Window;
            Ogre::SceneManager* m_SceneManager;
            Ogre::SceneNode* m_Camera;
            Ogre::SceneNode* VisualBodyAnchor;
			Ogre::Quaternion refVisualBody;
            Ogre::Entity* VisualBody;
            Ogre::AnimationState* VisualBodyAnimation;
			float visualBody_Zoffset;
            bool readyForLoadingRessources;

            Ogre::Entity* m_ent; //only used for creating nodes into the smgr

            //Oculus oculus;
            OgreOculusRender* oor;

            ///Dynamic container for games objects
            AnnGameObjectVect objects;
            AnnTriggerObjectVect triggers;

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

            ///Time
            unsigned long last,now; //Milisec
            
            ///Elapsed time
            float deltaT; //Sec

            //bullet
            btBroadphaseInterface* m_Broadphase;
            btDefaultCollisionConfiguration* m_CollisionConfiguration;
            btCollisionDispatcher* m_Dispatcher;
            btSequentialImpulseConstraintSolver* m_Solver;
            btGhostPairCallback* m_ghostPairCallback;
            
            ///Bullet Dynamics World
            btDiscreteDynamicsWorld* m_DynamicsWorld;
            
            bool debugPhysics;
            BtOgre::DebugDrawer* m_debugDrawer;

            btQuaternion fixedBodyOrient;

            Ogre::Quaternion QuatReference;

            AnnGameObject* m_Ground;

            ///Audio engine
            AnnAudioEngine* AudioEngine;
    };
}
#endif ///ANN_ENGINE
