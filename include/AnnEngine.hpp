/**
 * \file AnnEngine.hpp
 * \brief Main Annwvyn Engine class
 *        handle intialization, destruction of object at runtime
 *        handle rendering initialization, physics initialization and sound engine initialization
 * \author A. Brainville (Ybalrid)
 */

#ifndef ANN_ENGINE
#define ANN_ENGINE


//Graphic rendering system for the rift
#include "OgreOculusRender.hpp"


#include "systemMacro.h"

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

//btOgre
#include "BtOgrePG.h"
#include "BtOgreGP.h"
#include "BtOgreExtras.h"

//Annwvyn
#include "AnnPlayer.hpp"
#include "AnnEventManager.hpp"
#include "AnnGameObject.hpp"
#include "AnnTriggerObject.hpp"
#include "AnnTypes.h"
#include "AnnTools.h"
#include "AnnAudioEngine.hpp"

#ifdef __gnu_linux__
#include <unistd.h> //for some unix functions
#endif

namespace Annwvyn
{
	///Main engine class. Creating an instance of that class make the engine start.
    class DLL AnnEngine
    {
        public:
            ///Class constructor. take the name of the window
			/// \param title The title of the windows that will be created by the operating system
            AnnEngine(const char title[] = "Annwvyn Game Engine");

            ///Class destructor. Do clean up stuff.
            ~AnnEngine();

			///Return pointer to the engine itself
			AnnEngine* getAddress() {return this;}

			///Get the event manager
			AnnEventManager* getEventManager();
			
			///Get the player
			AnnPlayer* getPlayer();

            ///Give a zipped archive resource location to the Ogre Resource Group Manager
			/// \param path The path to a zip file.
			/// \param resourceGroupName name of the resource group where the content will be added
            void loadZip(const char path[], const char resourceGroupName[] = "ANNWVYN_DEFAULT");

			///Give a directory resouce location to the Ogre Resource Group Manager
			/// \param path The path to the directory
			/// \param resourceGroupName name of the resource group
            void loadDir(const char path[], const char resourceGroupName[] = "ANNWVYN_DEFAULT");

			///Load a standard Ogre resource.cfg file
			/// \param path path to the resource file
            void loadResFile(const char path[]); //resource

            ///Init All ressources groups
            void initResources(); //resource

			///Add to the default resource group "FileSystem=media" and "Zip=media/CORE.zip"
			void addDefaultResourceLocaton();

			///Init a resource group
			/// \param resourceGroup name of the resourceGroup
			void initAResourceGroup(std::string resourceGroup); //resource

            ///Init OgreOculus stuff
            void oculusInit(bool fullscreen = true); //oculus

			///Init the physics model
            void initPlayerPhysics(); //physics on player 

			///Update player location/orientation from the bullet body
            void updatePlayerFromPhysics(); //physics on player

            ///Translate the player (DONT DETECT COLLISIONS HERE !!!)
            void translatePhysicBody(Ogre::Vector3 translation); //physics on player 

            ///Set player linear speed from a 3D vector
            void setPhysicBodyLinearSpeed(Ogre::Vector3 V); //physics on player

            ///Create a game object form the name of an entity.
			/// \param entityName Name of an entity loaded to the Ogre ResourceGroupManager
			/// \param object An instance of an empty AnnGameObject. Usefull for creating object of herited class
            AnnGameObject* createGameObject(const char entityName[], AnnGameObject* object = new AnnGameObject); //object factory

			///Destroy the given object
			/// \param object the object to be destroyed
            bool destroyGameObject(AnnGameObject* object); //object factory

            ///Set the ambiant light
			/// \param v the color of the light
            void setAmbiantLight(Ogre::ColourValue v); //scene parameter

            ///Add a light to the scene. return a pointer to the new light
            AnnLightObject* addLight(); //object factory

            ///Calculate one frame of the game
            void renderOneFrame(); //graphics

            ///Display bullet debuging drawing
			/// \param state debug state
            void setDebugPhysicState(bool state); //engine debug

            ///Run objects physics
            void runPhysics(); //physics engine call

            ///Return true if the game want to terminate the program
            bool requestStop(); //engine

            ///Log something to the console. If flag = true (by default), will print "Annwvyn - " in front of the message
			/// \param message Message to be loged 
			/// \param flag If true : Put the "Annwvyn -" flag before the message
            static void log(std::string message, bool flag = true); //engine

            ///Update camera position/orientation from rift and virtual body
            void updateCamera(); //graphics
			
			///Apply yaw transform with mouse relative X displacement
			void applyMouseYaw(); //player engine call

			///Get camera position and ask OOR to do the render
			void doRender(); //graphics engine call 
			
			///Sync all audo objects
			void updateAudioSystemState(); //audio
			
			///Run the base gameplay loop content (WASD + jump + mouse for yaw)
			void runBasicGameplay(); //gameplay on user

            ///Refresh all for you
            void refresh(); //engine main loop

            ///Return a vector depending on WASD keys pressed
            bool processWASD(Ogre::Vector3* translate); //event on user

            ///Caputre event form keyboard and mouse
            void captureEvents(); //events

            ///Update program time. retur the delay between the last call of this method
            float getTime();//engine

            ///Get elapsed time from engine startup
            float getTimeFromStartUp();//engine

            ///Step Objects animation
            void playObjectsAnnimation(); //object engine call

            ///Set the ground object
			/// \param Ground an object that play the role of the ground.
            void setGround(AnnGameObject* Ground); //scene

            ///Return the Annwvyn OpenAL simplified audio engine
            AnnAudioEngine* getAudioEngine(); //audio

            ///LowLevel OIS Mouse
            OIS::Mouse* getOISMouse(); //event

            ///LowLevel OIS Keyboard
            OIS::Keyboard* getOISKeyboard(); //event

            ///LowLevel OIS Joystick
            OIS::JoyStick* getOISJoyStick(); //event

            ///Is key 'key' pressed ? (see OIS headers for KeyCode, generaly 'OIS::KC_X' where X is the key you want.
            /// key an OIS key code
			bool isKeyDown(OIS::KeyCode key); //event

            ///Return true if you touch the ground
            bool collisionWithGround(); //physics

            ///Process collision test form bullet manifold and objects collision mask
            void processCollisionTesting(); //physics on object engine call

            ///Get the dynamicsWorld
            btDiscreteDynamicsWorld* getDynamicsWorld(); //physics

            ///Create a trigger object
            /// \param trigger an empty trigger object
			AnnTriggerObject* createTriggerObject(AnnTriggerObject* trigger = new AnnTriggerObject); //object factory

            ///Process contact test with triggers
            void processTriggersContacts(); //gameplay engine call 

            ///Get ogre scene manager
            Ogre::SceneManager* getSceneManager(); //scene or graphics

            ///Set the ogre material for the skydime with params
			/// \param activate if true put a skydome
			/// \param materialName name of a material known from the Ogre Resource group manager
			/// \param curvature curvature of the texture
			/// \param tilling tilling of the texture
            void setSkyDomeMaterial(bool activate, const char materialName[], float curvature = 2.0f, float tiling = 1.0f); //scene

            ///Get the AnnObject the player is looking at
            Annwvyn::AnnGameObject* playerLookingAt(); //physics

            ///Get the AnnGameObject form the given Ogre node
            Annwvyn::AnnGameObject* getFromNode(Ogre::SceneNode* node); //engine

            ///DEPRECATED Get bodyParams
            Annwvyn::bodyParams* getBodyParams(); 

            ///Get ogre camera scene node
            Ogre::SceneNode* getCamera(); //... 
        
			///Reference orientation. Usefull if you are inside a vehicule for example
			/// \param q the reference orientation for the point of view. Usefull for applying vehicle movement to the player
            void setReferenceQuaternion(Ogre::Quaternion q); //engine...

			///Retrive the said reference quaternion
            Ogre::Quaternion getReferenceQuaternion(); //engine 

			///Attach a 3D mesh to the camera to act as player's body.
			/// \param entityName name of the entity that will serve as player body
			/// \param z_offset offset betwenn camera and player center eye pont
			/// \param flip if you need to flip the object to be correctly oriented (looking to negative Z)
			/// \param scale The scale to be aplied to the body object
            void attachVisualBody(const std::string entityName,  
                    float z_offset = -0.0644f, 
                    bool flip = false, 
                    bool animated = false, 
                    Ogre::Vector3 scale = Ogre::Vector3::UNIT_SCALE); //I seriously have something to do about that...

			///Reset the Rift Orientation
            void resetOculusOrientation();///Gameplay... but engine related function. 

			///Get a pose information object
			OgrePose getPoseFromOOR()
			{
				if(oor)
					return oor->returnPose;
				OgrePose p; return p;
			}
		
			///Set the distance of the near clipping plane
			/// \param distace the distance to the clipping plane
			void setNearClippingDistance(Ogre::Real distance); //graphics

        private:

			///Set up graphics
            void setUpOgre(const char title[]);

			///Set up physics
            void setUpBullet();

			///Set up inputs
            void setUpOIS();

			///Set up timing
            void setUpTime();

			///Set up 3D audio system
            void setUpAudio();

			///Set up GUI/HUD rendering
            void setUpGUI();

			///Create the bullet shape of the player's body
            void createVirtualBodyShape();

			///Create a physical object from the calculated shape
            void createPlayerPhysicalVirtualBody();

			///Add the players body to the Physics simulation
            void addPlayerPhysicalBodyToDynamicsWorld();
			
			///Returns internal timing
            float updateTime();
            
            ///Unable to continue, we have to cleanly cut the program before creating an error
            void emergency(void);

        private:

			//Audio engine
            AnnAudioEngine* AudioEngine;
			//Player
			AnnPlayer* player;
			//Event manager
			AnnEventManager* eventManager;
			

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

            //Events processing : 
            OIS::InputManager *m_InputManager;
            OIS::Keyboard *m_Keyboard;
            OIS::Mouse *m_Mouse;
            OIS::JoyStick *m_Joystick;
            OIS::ParamList pl;

            size_t windowHnd;
            std::ostringstream windowHndStr;

            bool activateWASD;
            bool activateJump;
            float jumpForce;

            //Time
            unsigned long last, now; //Milisec
            
            //Elapsed time
            float deltaT; //Sec

            //bullet
            btBroadphaseInterface* m_Broadphase;
            btDefaultCollisionConfiguration* m_CollisionConfiguration;
            btCollisionDispatcher* m_Dispatcher;
            btSequentialImpulseConstraintSolver* m_Solver;
            btGhostPairCallback* m_ghostPairCallback;
            
            //Bullet Dynamics World
            btDiscreteDynamicsWorld* m_DynamicsWorld;
            
            bool debugPhysics;
            BtOgre::DebugDrawer* m_debugDrawer;

            btQuaternion fixedBodyOrient;

            Ogre::Quaternion QuatReference;

            AnnGameObject* m_Ground;
    };
}
#endif ///ANN_ENGINE
