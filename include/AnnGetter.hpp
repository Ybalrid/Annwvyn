#pragma once

#include "systemMacro.h"
#include "AnnEngine.hpp"

namespace Annwvyn
{
	///Get the current instance of AnnEngine
	DLL AnnEngine* AnnGetEngine();

	///Get the audio engine
	DLL std::shared_ptr<AnnAudioEngine> AnnGetAudioEngine();
	///Get the physics engine
	DLL std::shared_ptr<AnnPhysicsEngine> AnnGetPhysicsEngine();
	///Get the file-system manager
	DLL std::shared_ptr<AnnFilesystemManager> AnnGetFileSystemManager();
	///Get the level manager
	DLL std::shared_ptr<AnnLevelManager> AnnGetLevelManager();
	///Get the event manager
	DLL std::shared_ptr<AnnEventManager> AnnGetEventManager();
	///Get the player object
	DLL std::shared_ptr<AnnPlayer>  AnnGetPlayer();
	///Get the resource manager
	DLL std::shared_ptr<AnnResourceManager> AnnGetResourceManager();
	///Get the scenery manager
	DLL std::shared_ptr<AnnSceneryManager> AnnGetSceneryManager();
	///Get the VR renderer
	DLL std::shared_ptr<OgreVRRender> AnnGetVRRenderer();
	///Get the script manager
	DLL std::shared_ptr<AnnScriptManager> AnnGetScriptManager();
	///Get the game-object manager
	DLL std::shared_ptr<AnnGameObjectManager> AnnGetGameObjectManager();
	///Get the on screen console
	DLL std::shared_ptr<AnnConsole> AnnGetOnScreenConsole();
	///Get the string utility object
	DLL std::shared_ptr<AnnStringUility> AnnGetStringUtility();
}