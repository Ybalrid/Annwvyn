#include "stdafx.h"
#include "AnnSceneryManager.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnSceneryManager::AnnSceneryManager(std::shared_ptr<OgreVRRender> rendererFromEngine) : AnnSubSystem("SceneryManager"),
smgr(AnnGetEngine()->getSceneManager()),
renderer(rendererFromEngine)
{
}

void AnnSceneryManager::setAmbiantLight(AnnColor color)
{
	AnnDebug() << "Setting the ambiant light to color " << color;
	smgr->setAmbientLight(color.getOgreColor());
}

void AnnSceneryManager::setSkyDomeMaterial(bool activate, const char materialName[], float curvature, float tiling)
{
	AnnDebug() << "Setting skydome from material" << materialName;
	smgr->setSkyDome(activate, materialName, curvature, tiling);
}

void AnnSceneryManager::setSkyBoxMaterial(bool activate, const char materialName[], float distance, bool renderedFirst)
{
	AnnDebug() << "Setting skydome from material" << materialName;
	smgr->setSkyBox(activate, materialName, distance, renderedFirst);
}

void AnnSceneryManager::setWorldBackgroundColor(AnnColor v)
{
	AnnDebug() << "Setting the backgroud world color " << v;
	renderer->changeViewportBackgroundColor(v.getOgreColor());
}

void AnnSceneryManager::removeSkyDome()
{
	AnnDebug("Disabeling skydome");
	smgr->setSkyDomeEnabled(false);
}

void AnnSceneryManager::removeSkyBox()
{
	AnnDebug("Disabeling skybox");
	smgr->setSkyBoxEnabled(false);
}

void AnnSceneryManager::setNearClippingDistance(float distance)
{
	AnnDebug() << "Setting the near clipping distance to " << distance;
	renderer->setCamerasNearClippingDistance(distance);
}