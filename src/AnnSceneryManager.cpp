#include "stdafx.h"
#include "AnnSceneryManager.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnSceneryManager::AnnSceneryManager(std::shared_ptr<OgreVRRender> rendererFromEngine) : AnnSubSystem("SceneryManager"),
smgr(AnnGetEngine()->getSceneManager()),
renderer(rendererFromEngine)
{
}

void AnnSceneryManager::setAmbientLight(AnnColor color)
{
	AnnDebug() << "Setting the ambient light to color " << color;
	smgr->setAmbientLight(color.getOgreColor());
}

void AnnSceneryManager::setSkyDomeMaterial(bool activate, const char materialName[], float curvature, float tiling)
{
	AnnDebug() << "Setting sky-dome from material" << materialName;
	smgr->setSkyDome(activate, materialName, curvature, tiling);
}

void AnnSceneryManager::setSkyBoxMaterial(bool activate, const char materialName[], float distance, bool renderedFirst)
{
	AnnDebug() << "Setting sky-dome from material" << materialName;
	smgr->setSkyBox(activate, materialName, distance, renderedFirst);
}

void AnnSceneryManager::setWorldBackgroundColor(AnnColor v)
{
	AnnDebug() << "Setting the background world color " << v;
	renderer->changeViewportBackgroundColor(v.getOgreColor());
}

void AnnSceneryManager::removeSkyDome()
{
	AnnDebug("Disabling sky-dome");
	smgr->setSkyDomeEnabled(false);
}

void AnnSceneryManager::removeSkyBox()
{
	AnnDebug("Disabling sky-box");
	smgr->setSkyBoxEnabled(false);
}