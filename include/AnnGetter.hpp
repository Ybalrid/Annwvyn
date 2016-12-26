#pragma once

#include "systemMacro.h"
#include "AnnEngine.hpp"

namespace Annwvyn
{
	DLL AnnEngine* AnnGetEngine();
	DLL std::shared_ptr<AnnAudioEngine> AnnGetAudioEngine();
	DLL std::shared_ptr<AnnPhysicsEngine> AnnGetPhysicsEngine();
	DLL std::shared_ptr<AnnFilesystemManager> AnnGetFileSystemManager();
	DLL std::shared_ptr<AnnLevelManager> AnnGetLevelManager();
	DLL std::shared_ptr<AnnEventManager> AnnGetEventManager();
	DLL std::shared_ptr<AnnPlayer>  AnnGetPlayer();
	DLL std::shared_ptr<AnnResourceManager> AnnGetResourceManager();
	DLL std::shared_ptr<AnnSceneryManager> AnnGetSceneryManager();
	DLL std::shared_ptr<OgreVRRender> AnnGetVRRenderer();
	DLL std::shared_ptr<AnnScriptManager> AnnGetScriptManager();
	DLL std::shared_ptr<AnnGameObjectManager> AnnGetGameObjectManager();
}