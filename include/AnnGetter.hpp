#pragma once

#include "systemMacro.h"
#include "AnnEngine.hpp"

namespace Annwvyn
{
	///Get the current instance of AnnEngine
	DLL AnnEngine* AnnGetEngine();

	///Get the audio engine
	DLL AnnAudioEnginePtr AnnGetAudioEngine();
	///Get the physics engine
	DLL AnnPhysicsEnginePtr AnnGetPhysicsEngine();
	///Get the file-system manager
	DLL AnnFilesystemManagerPtr AnnGetFileSystemManager();
	///Get the level manager
	DLL AnnLevelManagerPtr AnnGetLevelManager();
	///Get the event manager
	DLL AnnEventManagerPtr AnnGetEventManager();
	///Get the player object
	DLL AnnPlayerBodyPtr  AnnGetPlayer();
	///Get the resource manager
	DLL AnnResourceManagerPtr AnnGetResourceManager();
	///Get the scenery manager
	DLL AnnSceneryManagerPtr AnnGetSceneryManager();
	///Get the VR renderer
	DLL AnnOgreVRRendererPtr AnnGetVRRenderer();
	///Get the script manager
	DLL AnnScriptManagerPtr AnnGetScriptManager();
	///Get the game-object manager
	DLL AnnGameObjectManagerPtr AnnGetGameObjectManager();
	///Get the on screen console
	DLL AnnConsolePtr AnnGetOnScreenConsole();
	///Get the string utility object
	DLL AnnStringUtilityPtr AnnGetStringUtility();
}