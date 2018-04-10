#include "AnnOgreOpenHMDRenderer.hpp"
#include "Annwvyn.h"

Annwvyn::AnnOgreOpenHMDRenderer::AnnOgreOpenHMDRenderer(const std::string& windowName) :
 AnnOgreVRRenderer(windowName)
{
}

Annwvyn::AnnOgreOpenHMDRenderer::~AnnOgreOpenHMDRenderer()
{
}

void Annwvyn::AnnOgreOpenHMDRenderer::initVrHmd()
{
}

void Annwvyn::AnnOgreOpenHMDRenderer::initScene()
{
}

void Annwvyn::AnnOgreOpenHMDRenderer::initRttRendering()
{
}

void Annwvyn::AnnOgreOpenHMDRenderer::initClientHmdRendering()
{
}

bool Annwvyn::AnnOgreOpenHMDRenderer::shouldQuit()
{
	return false;
}

bool Annwvyn::AnnOgreOpenHMDRenderer::shouldRecenter()
{
	return false;
}

bool Annwvyn::AnnOgreOpenHMDRenderer::isVisibleInHmd()
{
	return false;
}

void Annwvyn::AnnOgreOpenHMDRenderer::getTrackingPoseAndVRTiming()
{
}

void Annwvyn::AnnOgreOpenHMDRenderer::renderAndSubmitFrame()
{
}

void Annwvyn::AnnOgreOpenHMDRenderer::recenter()
{
}

void Annwvyn::AnnOgreOpenHMDRenderer::updateProjectionMatrix()
{
}

void Annwvyn::AnnOgreOpenHMDRenderer::showDebug(DebugMode mode)
{
}

void Annwvyn::AnnOgreOpenHMDRenderer::handleIPDChange()
{
}

void Annwvyn::AnnOgreOpenHMDRenderer::initCameras()
{
}

void Annwvyn::AnnOgreOpenHMDRenderer::cycleDebugHud()
{
}

bool Annwvyn::AnnOgreOpenHMDRenderer::usesCustomAudioDevice()
{
	return false;
}

std::string Annwvyn::AnnOgreOpenHMDRenderer::getAudioDeviceIdentifierSubString()
{
	return {};
}

Annwvyn::AnnOgreVRRenderer* AnnRendererBootstrap_OpenHMD(const std::string& appName)
{
	return static_cast<Annwvyn::AnnOgreVRRenderer*>(new Annwvyn::AnnOgreOpenHMDRenderer(appName));
}
