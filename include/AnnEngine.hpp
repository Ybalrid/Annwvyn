/**
* \file AnnEngine.hpp
* \brief Main Annwvyn Engine class
*        handle intialization, destruction of object at runtime
*        handle rendering initialization, physics initialization and sound engine initialization
* \author A. Brainville (Ybalrid)
*/

#ifndef ANN_ENGINE
#define ANN_ENGINE

//Keep track of engine version here
#define ANN_MAJOR 0
#define ANN_MINOR 2
#define ANN_PATCH 3 
#define ANN_EXPERIMENTAL true

#include "systemMacro.h"

//C++ STD & STL
#include <cassert>
#include <list>

//Graphic rendering system for the rift
#include "OgreOculusRender.hpp"

//Annwvyn
#include "AnnEventManager.hpp"
#include "AnnTriggerObject.hpp"
#include "AnnTypes.h"
#include "AnnAudioEngine.hpp"
#include "AnnPhysicsEngine.hpp"
#include "AnnConsole.hpp"
#include "AnnLevelManager.hpp"
#include "AnnFilesystem.hpp"
#include "AnnResourceManager.hpp"
#include "AnnGameObject.hpp"
#include "AnnGameObjectManager.hpp"
#include "AnnSceneryManager.hpp"

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

//Get the deprecated warnings
#pragma warning(default:4996)

///Easy acces to AnnEnigine instance
#define AnnGetEngine() AnnEngine::Instance()
///Get audio
#define AnnGetAudioEngine() AnnGetEngine()->getAudioEngine()
///Get physics
#define AnnGetPhysicsEngine() AnnGetEngine()->getPhysicsEngine()
///Get FileManager
#define AnnGetFileSystemManager() AnnGetEngine()->getFileSystemManager()
///Get LevelManager
#define AnnGetLevelManager() AnnGetEngine()->getLevelManager()
///Get EventManager
#define AnnGetEventManager() AnnGetEngine()->getEventManager()
///Get PlayerObject
#define AnnGetPlayer() AnnGetEngine()->getPlayer()
///Get ResourceManager
#define AnnGetResourceManager() AnnGetEngine()->getResourceManager()
///Get GameObjectManager
#define AnnGetGameObjectManager() AnnGetEngine()->getGameObjectManager()
///Get SceneryManager
#define AnnGetSceneryManager() AnnGetEngine()->getSceneryManager()
///GEt VRREnderer
#define AnnGetVRRenderer() AnnGetEngine()->getVRRenderer()

namespace Annwvyn
{
	///Main engine class. Creating an instance of that class make the engine start.
	class DLL AnnEngine
	{
	private:
		///the singleton address itself is stored here
		static AnnEngine* singleton;

		//It's more or less a singleton, and will be the only one in the engine architecture. 
		//You can intantiate it like a normal class and bypass the idea of a singleton complettely.
		//This is the base class of the whole engine, the idea is more or less the one described in the 
		//"solutions to use a singleton for everything" in this article http://gameprogrammingpatterns.com/singleton.html
	public:
		

		///Get the current instance of AnnEngine. pointer
		static AnnEngine* Instance();

		///Return a string descibing the version of the engine
		static std::string getAnnwvynVersion();

		///Class constructor. take the name of the window
		/// \param title The title of the windows that will be created by the operating system
		/// \param fs the fullscreen state of the application. set it to false may help when developping with VS debugger on one screen
		AnnEngine(const char title[]);

		///Class destructor. Do clean up stuff.
		~AnnEngine();

		///Log something to the console. If flag = true (by default), will print "Annwvyn - " in front of the message
		/// \param message Message to be loged 
		/// \param flag If true : Put the "Annwvyn -" flag before the message
		static void log(std::string message, bool flag = true); //engine

		///Get the player
		AnnPlayer* getPlayer();

		///Is key 'key' pressed ? (see OIS headers for KeyCode, generaly 'OIS::KC_X' where X is the key you want.
		/// key an OIS key code
		inline bool isKeyDown(OIS::KeyCode key); //event

		///Get ogre camera scene node
		Ogre::SceneNode* getCamera();

		///Open a console and redirect standard output to it.
		///This is only effective on Windows. There is no other
		///simple way to acces the standard io on a Win32 application
		static void openConsole();

		///Get ogre scene manager
		Ogre::SceneManager* getSceneManager(); //scene or graphics

		/////////////////////////////////////////////////////////////////////////////////////////////////////SUBSYSTEMS
		
		///Get the event manager
		AnnEventManager* getEventManager();

		///Get the filesystem manager
		AnnFilesystemManager* getFileSystemManager();

		///Return the Annwvyn OpenAL simplified audio engine
		AnnAudioEngine* getAudioEngine(); //audio

		///Return the Physics Engine
		AnnPhysicsEngine* getPhysicsEngine();

		///Get the current level manager
		AnnLevelManager* getLevelManager();

		///Get the ResourceManager
		AnnResourceManager* getResourceManager();

		///Get the GameObjectManager
		AnnGameObjectManager* getGameObjectManager();

		///Get the SceneryManager
		AnnSceneryManager* getSceneryManager();

		///Get the VRRenderer
		OgreVRRender* getVRRenderer();

		
		/////////////////////////////////////////////////////////////////////////////////////////////////////SUBSYSTEMS
		////////////////////////////////////////////////////////////////////////////////////////////////TO CALL AT INIT

	    ///Init the physics model
		void initPlayerPhysics(); //physics on player
		
		////////////////////////////////////////////////////////////////////////////////////////////////TO CALL AT INIT
		//////////////////////////////////////////////////////////////////////////////////////////////////GAMEPLAY LOOP

		///Return true if the game want to terminate the program
		bool requestStop(); //engine

		///Refresh all for you
		bool refresh(); //engine main loop

		///Set the POV node to the AnnPlayer gameplay defined position/orientation of the player's body
		inline void syncPov();

		///This start the reder loop. This also calls objects "atRefresh" and current level "runLogic" methods each frame
		void startGameplayLoop();

		///Toogle the display of the in-engine console
		void toogleOnScreenConsole();

		///Return true if the app is visible inside the head mounted display
		bool appVisibleInHMD();

		//////////////////////////////////////////////////////////////////////////////////////////////////GAMEPLAY LOOP
		///////////////////////////////////////////////////////////////////////////////////////////////TIMER MANAGEMENT

		///Get elapsed time from engine startup in millisec
		unsigned long getTimeFromStartUp();//engine

		///Get elapsed time from engine startup in seconds
		double getTimeFromStartupSeconds();

		///Get elapsed time between two frames in seconds
		double getFrameTime();

		///Get the pose of the HMD in VR world space
		OgrePose getHmdPose();
		///////////////////////////////////////////////////////////////////////////////////////////////TIMER MANAGEMENT

	private:
		void destroyAllSubSystem();

		///The onScreenConsole object
		static AnnConsole* onScreenConsole;

		///Audio engine
		AnnAudioEngine* audioEngine;
		///Player
		AnnPlayer* player;
		///Event manager
		AnnEventManager* eventManager;
		///Physics
		AnnPhysicsEngine* physicsEngine;
		///LevelManager
		AnnLevelManager* levelManager;
		///FilesystemManager
		AnnFilesystemManager* filesystemManager;
		///ResourceManager
		AnnResourceManager* resourceManager;
		///GameObjectManager
		AnnGameObjectManager* gameObjectManager;
		///SceneryManager
		AnnSceneryManager* sceneryManager;

		///The scene manager
		Ogre::SceneManager* SceneManager;
		///Point Of View : Node used as "root" for putting the VR "camera rig"
		Ogre::SceneNode* povNode;

		///VR renderer
		OgreVRRender* renderer;

		///Elapsed time between 2 frames
		double updateTime;

		///Container for all the subsystem. Populated in the update/delete order 
		std::list<AnnSubSystem*> SubSystemList;

		///If false, all getStuff will return nullptr
		bool canAccessSubSystems;
	};
}
#endif ///ANN_ENGINE
