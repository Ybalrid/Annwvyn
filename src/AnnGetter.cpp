#include "stdafx.h"
#include "AnnGetter.hpp"

//using namespace Annwvyn;
using namespace std;

namespace Annwvyn
{
	AnnEngine* AnnGetEngine() { return AnnEngine::Instance(); }

	shared_ptr<AnnAudioEngine> AnnGetAudioEngine() { return AnnGetEngine()->getAudioEngine(); }
	shared_ptr<AnnPhysicsEngine> AnnGetPhysicsEngine() { return AnnGetEngine()->getPhysicsEngine(); }
	shared_ptr<AnnFilesystemManager> AnnGetFileSystemManager() { return AnnGetEngine()->getFileSystemManager(); }
	shared_ptr<AnnLevelManager> AnnGetLevelManager() { return AnnGetEngine()->getLevelManager(); }
	shared_ptr<AnnEventManager> AnnGetEventManager() { return AnnGetEngine()->getEventManager(); }
	shared_ptr<AnnPlayer> AnnGetPlayer() { return AnnGetEngine()->getPlayer(); };
	shared_ptr<AnnResourceManager> AnnGetResourceManager() { return AnnGetEngine()->getResourceManager(); }
	shared_ptr<AnnSceneryManager> AnnGetSceneryManager() { return AnnGetEngine()->getSceneryManager(); }
	shared_ptr<OgreVRRender> AnnGetVRRenderer() { return AnnGetEngine()->getVRRenderer(); }
	shared_ptr<AnnScriptManager> AnnGetScriptManager() { return AnnGetEngine()->getScriptManager(); }
	shared_ptr<AnnGameObjectManager> AnnGetGameObjectManager() { return AnnGetEngine()->getGameObjectManager(); }
}