#pragma once

#include "systemMacro.h"
#include "AnnEngine.hpp"

namespace Annwvyn
{
	///Get the current instance of AnnEngine
	AnnDllExport AnnEngine* AnnGetEngine();

	///Get the audio engine
	AnnDllExport AnnAudioEnginePtr AnnGetAudioEngine();
	///Get the physics engine
	AnnDllExport AnnPhysicsEnginePtr AnnGetPhysicsEngine();
	///Get the file-system manager
	AnnDllExport AnnFilesystemManagerPtr AnnGetFileSystemManager();
	///Get the level manager
	AnnDllExport AnnLevelManagerPtr AnnGetLevelManager();
	///Get the event manager
	AnnDllExport AnnEventManagerPtr AnnGetEventManager();
	///Get the player object
	AnnDllExport AnnPlayerBodyPtr  AnnGetPlayer();
	///Get the resource manager
	AnnDllExport AnnResourceManagerPtr AnnGetResourceManager();
	///Get the scenery manager
	AnnDllExport AnnSceneryManagerPtr AnnGetSceneryManager();
	///Get the VR renderer
	AnnDllExport AnnOgreVRRendererPtr AnnGetVRRenderer();
	///Get the script manager
	AnnDllExport AnnScriptManagerPtr AnnGetScriptManager();
	///Get the game-object manager
	AnnDllExport AnnGameObjectManagerPtr AnnGetGameObjectManager();
	///Get the on screen console
	AnnDllExport AnnConsolePtr AnnGetOnScreenConsole();
	///Get the string utility object
	AnnDllExport AnnStringUtilityPtr AnnGetStringUtility();
}
