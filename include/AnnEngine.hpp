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
#include "AnnPhysicsEngine.hpp"

namespace Annwvyn
{
	class AnnPhysicsEngine;


	///Main engine class. Creating an instance of that class make the engine start.
	///It's more or less a singleton, and will be the only one in the engine architecture. 
	///You can intantiate it like a normal class and bypass the idea of a singleton complettely.
	///This is the base class of the whole engine, the idea is more or less the one described in the 
	///"solutions to use a singleton for everything" in this article http://gameprogrammingpatterns.com/singleton.html
	class DLL AnnEngine
	{
	private:
		///the singleton itsefl is stored here
		static AnnEngine* singleton;
	public:
		///Get the current instance of AnnEngine. pointer
		static AnnEngine* Instance();

		///Class constructor. take the name of the window
		/// \param title The title of the windows that will be created by the operating system
		AnnEngine(const char title[] = "Annwvyn Game Engine");

		///Class destructor. Do clean up stuff.
		~AnnEngine();

		///Get the event manager
		AnnEventManager* getEventManager();

		///Get the player
		AnnPlayer* getPlayer();

		///////////////////////////////////////////////////////////////////////////////////////////////////////RESOURCE

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

		///////////////////////////////////////////////////////////////////////////////////////////////////////RESOURCE

		///Init OgreOculus stuff
		void oculusInit(bool fullscreen = true); //oculus

		///Init the physics model
		void initPlayerPhysics(); //physics on player 

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

		///Display bullet debuging drawing
		/// \param state debug state
		void setDebugPhysicState(bool state); //engine debug

		///Return true if the game want to terminate the program
		bool requestStop(); //engine

		///Log something to the console. If flag = true (by default), will print "Annwvyn - " in front of the message
		/// \param message Message to be loged 
		/// \param flag If true : Put the "Annwvyn -" flag before the message
		static void log(std::string message, bool flag = true); //engine

		///Refresh all for you
		bool refresh(); //engine main loop

		///Update program time. retur the delay between the last call of this method
		double getTime();//engine

		///Get elapsed time from engine startup
		double getTimeFromStartUp();//engine

		///Return the Annwvyn OpenAL simplified audio engine
		AnnAudioEngine* getAudioEngine(); //audio

		///Is key 'key' pressed ? (see OIS headers for KeyCode, generaly 'OIS::KC_X' where X is the key you want.
		/// key an OIS key code
		bool isKeyDown(OIS::KeyCode key); //event

		///Create a trigger object
		/// \param trigger an empty trigger object
		AnnTriggerObject* createTriggerObject(AnnTriggerObject* trigger = new AnnTriggerObject); //object factory

		///Get ogre scene manager
		Ogre::SceneManager* getSceneManager(); //scene or graphics

		///Set the ogre material for the skydime with params
		/// \param activate if true put a skydome
		/// \param materialName name of a material known from the Ogre Resource group manager
		/// \param curvature curvature of the texture
		/// \param tilling tilling of the texture
		void setSkyDomeMaterial(bool activate, 
			const char materialName[], 
			float curvature = 2.0f, 
			float tiling = 1.0f); //scene

		///Get the AnnObject the player is looking at
		Annwvyn::AnnGameObject* playerLookingAt(); //physics

		///Get the AnnGameObject form the given Ogre node
		Annwvyn::AnnGameObject* getFromNode(Ogre::SceneNode* node); //engine

		///Get ogre camera scene node
		Ogre::SceneNode* getCamera();

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

		///Set the distance of the near clipping plane
		/// \param distace the distance to the clipping plane
		void setNearClippingDistance(Ogre::Real distance); //graphics

		///Set the engine to use the "default" event listener.
		///The default event listerner implement a simple "FPS-like" controll scheme 
		/// WASD for walking
		/// Horizontal view with mouse X relative mouvement
		/// That event listener is designed as an example of an event listener, and for exploring the environement without having to write a custom event listene
		void useDefaultEventListener();

		///Get the address of the default event listener declared by "use default event listener"
		AnnDefaultEventListener* getInEngineDefaultListener();

		///Get a pose information object
		OgrePose getPoseFromOOR()
		{
			if(oor)
				return oor->returnPose;
			OgrePose p; return p;
		}

	private:
		//Audio engine
		AnnAudioEngine* AudioEngine;
		//Player
		AnnPlayer* player;
		//Event manager
		AnnEventManager* eventManager;
		AnnDefaultEventListener* defaultEventListener;
		//Physics
		AnnPhysicsEngine* physicsEngine;

		Ogre::RenderWindow* m_Window;
		Ogre::SceneManager* m_SceneManager;
		Ogre::SceneNode* m_CameraReference;
		Ogre::SceneNode* VisualBodyAnchor;
		Ogre::Quaternion refVisualBody;
		Ogre::Entity* VisualBody;
		Ogre::AnimationState* VisualBodyAnimation;
		float visualBody_Zoffset;
		bool readyForLoadingRessources;

		//Oculus oculus;
		OgreOculusRender* oor;

		///Dynamic container for games objects present in engine.
		std::vector<AnnGameObject*>	objects;
		std::vector<AnnTriggerObject*> triggers;

		//Elapsed time
		double deltaT; //Sec

		double lastFrameTimeCode;
		double currentFrameTimeCode;

#ifdef __gnu_linux__
		std::string x11LayoutAtStartup;
#endif

	};
}
#endif ///ANN_ENGINE
