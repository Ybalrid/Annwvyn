#include "stdafx.h"
#include "AnnSceneryManager.hpp"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"

using namespace Annwvyn;

AnnSceneryManager::AnnSceneryManager(AnnOgreVRRendererPtr rendererFromEngine) : AnnSubSystem("SceneryManager"),
smgr(AnnGetEngine()->getSceneManager()),
renderer(rendererFromEngine),
defaultExposure(0.0f),
defaultMinAutoExposure(0),
defaultMaxAutoExposure(0),
defaultSkyColorMultiplier(15.f),
defaultBloom(5),
defaultUpperAmbientLightMul(2.5f),
defaultLowerAmbientLightMul(2.925f),
defaultSkyColor(0.05f, 0.45f, 1.f),
defaultUpperAmbient(0.3f, 0.5f, 0.7f),
defaultLowerAmbient(0.6f, 0.45f, 0.3f)
{
	setDefaultExposure();
	setDefaultSkyColor();
	setDefaultBloomThreshold();
	setDefaultAmbientLight();
}

void AnnSceneryManager::setAmbientLight(AnnColor upperColor, float upperMul, AnnColor lowerColor, float lowerMul, AnnVect3 direction, float environementMapScaling) const
{
	AnnDebug() << "Setting the ambient light to"
		<< upperColor.getOgreColor()*upperMul << " "
		<< lowerColor.getOgreColor()*lowerMul << " "
		<< direction << environementMapScaling;

	smgr->setAmbientLight(upperColor.getOgreColor() * upperMul,
		lowerColor.getOgreColor() * lowerMul,
		direction,
		environementMapScaling);
}

void AnnSceneryManager::setSkyDomeMaterial(bool activate, const std::string& materialName, float curvature, float tiling) const
{
	AnnDebug() << "Setting sky-dome from material" << materialName;
	smgr->setSkyDome(activate, materialName, curvature, tiling);
}

void AnnSceneryManager::setSkyBoxMaterial(bool activate, const std::string& materialName, float distance, bool renderedFirst) const
{
	AnnDebug() << "Setting sky-dome from material" << materialName;
	smgr->setSkyBox(activate, materialName, distance, renderedFirst);
}

void AnnSceneryManager::removeSkyDome() const
{
	AnnDebug("Disabling sky-dome");
	smgr->setSkyDomeEnabled(false);
}

void AnnSceneryManager::removeSkyBox() const
{
	AnnDebug("Disabling sky-box");
	smgr->setSkyBoxEnabled(false);
}

void AnnSceneryManager::setExposure(float exposure, float minExposure, float maxExposure) const
{
	AnnDebug() << "Setting exposure to " << exposure << "ev with adjustable range [" << minExposure << "; " << maxExposure << "]";
	renderer->setExposure(exposure, minExposure, maxExposure);
}

void AnnSceneryManager::setDefaultExposure() const
{
	setExposure(defaultExposure, defaultMinAutoExposure, defaultMaxAutoExposure);
}

void AnnSceneryManager::setSkyColor(AnnColor color, float multiplier) const
{
	AnnDebug() << "Setting sky color : " << color << "*" << multiplier;
	renderer->setSkyColor(color.getOgreColor(), multiplier, "HdrRenderingNode");
}

void AnnSceneryManager::setDefaultSkyColor() const
{
	setSkyColor(defaultSkyColor, defaultSkyColorMultiplier);
}

void AnnSceneryManager::setBloomThreshold(float threshold) const
{
	AnnDebug() << "Set bloom threshold to " << threshold;
	renderer->setBloomThreshold(std::max(threshold - 2.f, 0.f), std::max(threshold, 0.01f));
}

void AnnSceneryManager::setDefaultBloomThreshold() const
{
	setBloomThreshold(defaultBloom);
}

void AnnSceneryManager::setDefaultAmbientLight() const
{
	setAmbientLight(defaultUpperAmbient, defaultUpperAmbientLightMul, defaultLowerAmbient, defaultLowerAmbientLightMul, AnnGetVRRenderer()->getSceneManager()->getAmbientLightHemisphereDir());
}

void AnnSceneryManager::resetSceneParameters() const
{
	removeSkyBox();
	removeSkyDome();
	setDefaultSkyColor();
	setDefaultAmbientLight();
	setDefaultBloomThreshold();
	setDefaultExposure();
}