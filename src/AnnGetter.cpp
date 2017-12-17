// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "stdafx.h"
#include "AnnGetter.hpp"

using std::shared_ptr;

namespace Annwvyn
{
	AnnEngine* AnnGetEngine() { return AnnEngine::Instance(); }

	AnnAudioEnginePtr AnnGetAudioEngine() { return AnnGetEngine()->getAudioEngine(); }
	AnnPhysicsEnginePtr AnnGetPhysicsEngine() { return AnnGetEngine()->getPhysicsEngine(); }
	AnnFilesystemManagerPtr AnnGetFileSystemManager() { return AnnGetEngine()->getFileSystemManager(); }
	AnnLevelManagerPtr AnnGetLevelManager() { return AnnGetEngine()->getLevelManager(); }
	AnnEventManagerPtr AnnGetEventManager() { return AnnGetEngine()->getEventManager(); }
	AnnPlayerBodyPtr AnnGetPlayer() { return AnnGetEngine()->getPlayer(); };
	AnnResourceManagerPtr AnnGetResourceManager() { return AnnGetEngine()->getResourceManager(); }
	AnnSceneryManagerPtr AnnGetSceneryManager() { return AnnGetEngine()->getSceneryManager(); }
	AnnOgreVRRendererPtr AnnGetVRRenderer() { return AnnGetEngine()->getVRRenderer(); }
	AnnScriptManagerPtr AnnGetScriptManager() { return AnnGetEngine()->getScriptManager(); }
	AnnGameObjectManagerPtr AnnGetGameObjectManager() { return AnnGetEngine()->getGameObjectManager(); }
	AnnConsolePtr AnnGetOnScreenConsole() { return AnnGetEngine()->getOnScreenConsole(); }
	AnnStringUtilityPtr AnnGetStringUtility() { return AnnGetEngine()->getStringUtility(); }
}