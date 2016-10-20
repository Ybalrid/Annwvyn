/**
* \file AnnEngine.hpp
* \brief Main Annwvyn Engine class
*        Initialize the renderer and all subsystem. Provide access to all of the engine components
* \author A. Brainville (Ybalrid)
*/

#ifndef ANN_ENGINE
#define ANN_ENGINE

//Keep track of engine version here
#define ANN_MAJOR 0
#define ANN_MINOR 2
#define ANN_PATCH 5
#define ANN_EXPERIMENTAL true

#include "systemMacro.h"

//C++ STD & STL
#include <cassert>
#include <list>
#include <memory>

//Graphic rendering system for the rift
#include "OgreOculusRender.hpp"

//Graphic rendering system for the vive
#include "OgreOpenVRRender.hpp"

//Graphic rendering system for NO FREAKING VR SYSTEM
#include "OgreNoVRRender.hpp"

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

#include "AnnUserSpaceSubSystem.hpp"

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

//Get the deprecated warnings
#pragma warning(default:4996)

///Easy access to AnnEnigine instance
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
	///Main engine class. Creating an instance of this class make the engine start.
	class DLL AnnEngine
	{
		//It's more or less a singleton, and will be the only one in the engine architecture.
		//You can instantiate it like a normal class and bypass the idea of a singleton completely.
		//This is the base class of the whole engine, the idea is more or less the one described in the
		//"solutions to use a singleton for everything" in this article http://gameprogrammingpatterns.com/singleton.html

	private:
		///the singleton address itself is stored here
		static AnnEngine* singleton;

	public:

		///Get the current instance of AnnEngine. pointer
		static AnnEngine* Instance();

		///Return a string describing the version of the engine
		static std::string getAnnwvynVersion(long long int padding = -1);

		///Class constructor. take the name of the window
		/// \param title The title of the windows that will be created by the operating system
		/// \param fs the fullscreen state of the application. set it to false may help when developing with VS debugger on one screen
		AnnEngine(const char title[], std::string hmdCommand);

		///Class destructor. Do clean up stuff.
		~AnnEngine();

		///Log something to the console. If flag = true (by default), will print "Annwvyn - " in front of the message
		/// \param message Message to be logged
		/// \param flag If true : Put the "Annwvyn -" flag before the message
		static void log(std::string message, bool flag = true); //engine

		///Get the player
		std::shared_ptr<AnnPlayer> getPlayer();

		///Is key 'key' pressed ? (see OIS headers for KeyCode, generally 'OIS::KC_X' where X is the key you want.
		/// key an OIS key code
		inline bool isKeyDown(OIS::KeyCode key); //event

		///Get ogre camera scene node
		Ogre::SceneNode* getPlayerPovNode();

		///Open a console and redirect standard output to it.
		///This is only effective on Windows. There is no other
		///simple way to access the standard io on a Win32 application
		static void openConsole();

		///Get ogre scene manager
		Ogre::SceneManager* getSceneManager(); //scene or graphics

		/////////////////////////////////////////////////////////////////////////////////////////////////////SUBSYSTEMS

		///Get the event manager
		std::shared_ptr<AnnEventManager> getEventManager();

		///Get the file-system manager
		std::shared_ptr<AnnFilesystemManager> getFileSystemManager();

		///Return the Annwvyn OpenAL simplified audio engine
		std::shared_ptr<AnnAudioEngine> getAudioEngine(); //audio

		///Return the Physics Engine
		std::shared_ptr<AnnPhysicsEngine> getPhysicsEngine();

		///Get the current level manager
		std::shared_ptr<AnnLevelManager> getLevelManager();

		///Get the ResourceManager
		std::shared_ptr<AnnResourceManager> getResourceManager();

		///Get the GameObjectManager
		std::shared_ptr<AnnGameObjectManager> getGameObjectManager();

		///Get the SceneryManager
		std::shared_ptr<AnnSceneryManager> getSceneryManager();

		///Get the VRRenderer
		std::shared_ptr<OgreVRRender> getVRRenderer();

		/////////////////////////////////////////////explicit /////////////////////////////////////////////////END OF SUBSYSTEMS

		///Init the physics model
		void initPlayerPhysics(); //physics on player

		///Return true if the game want to terminate the program
		bool requestStop(); //engine

		///Refresh all for you
		bool refresh(); //engine main loop

		///Set the POV node to the AnnPlayer gameplay defined position/orientation of the player's body
		inline void syncPov();

		///This start the render loop. This also calls objects "atRefresh" and current level "runLogic" methods each frame
		void startGameplayLoop();

		///Toggle the display of the in-engine console
		void toogleOnScreenConsole();

		///Return true if the app is visible inside the head mounted display
		bool appVisibleInHMD();

		///Get elapsed time from engine startup in millisecond
		unsigned long getTimeFromStartUp();//engine

		///Get elapsed time from engine startup in seconds
		double getTimeFromStartupSeconds();

		///Get elapsed time between two frames in seconds
		double getFrameTime();

		///Get the pose of the HMD in VR world space
		OgrePose getHmdPose();

		///Register your own subsystem to be updated by the engine
		std::shared_ptr<AnnUserSubSystem> registerUserSubSystem(std::shared_ptr<AnnUserSubSystem> userSystem);

		///Get pointer to a subsystem by name
		std::shared_ptr<AnnSubSystem> getSubSystemByName(std::string name);

		///Know if subsystem is user defined
		static bool isUserSubSystem(std::shared_ptr<AnnSubSystem> subsystem);

		///Remove a subsystem form the engine. Only works if the system has been user defined.
		void removeUserSubSystem(std::shared_ptr<AnnUserSubSystem> subsystem);

	private:

		///VR renderer
		std::shared_ptr<OgreVRRender> renderer;

		///The onScreenConsole object
		static std::shared_ptr<AnnConsole> onScreenConsole;
		///ResourceManager
		std::shared_ptr<AnnResourceManager> resourceManager;
		///SceneryManager
		std::shared_ptr<AnnSceneryManager> sceneryManager;
		///FilesystemManager
		std::shared_ptr<AnnFilesystemManager> filesystemManager;
		///Audio engine
		std::shared_ptr<AnnAudioEngine> audioEngine;
		///Event manager
		std::shared_ptr<AnnEventManager> eventManager;
		///Physics
		std::shared_ptr<AnnPhysicsEngine> physicsEngine;
		///GameObjectManager
		std::shared_ptr<AnnGameObjectManager> gameObjectManager;
		///LevelManager
		std::shared_ptr<AnnLevelManager> levelManager;
		///Player
		std::shared_ptr<AnnPlayer> player;

		///The scene manager
		Ogre::SceneManager* SceneManager;
		///Point Of View : Node used as "root" for putting the VR "camera rig"
		Ogre::SceneNode* vrRendererPovGameplayPlacement;

		///Elapsed time between 2 frames
		double updateTime;

		///Container for all the subsystem. Populated in the update/delete order
		std::list<std::shared_ptr<AnnSubSystem>> SubSystemList;
	};
}
#endif ///ANN_ENGINE