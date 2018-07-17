// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "AnnOgreNoVRRenderer.hpp"

#include "AnnLogger.hpp"
#include "AnnGetter.hpp"

using namespace Annwvyn;

AnnOgreNoVRRenderer* AnnOgreNoVRRenderer::noVRself(nullptr);

AnnOgreNoVRRenderer::AnnOgreNoVRRenderer(std::string name) :
 AnnOgreVRRenderer(name),
 running(true)
{
	rendererName = "OpenGL/NoVR";
	noVRself	 = dynamic_cast<AnnOgreNoVRRenderer*>(self);
}

void AnnOgreNoVRRenderer::initVrHmd()
{}

void AnnOgreNoVRRenderer::initScene()
{
	createMainSmgr();
}

void AnnOgreNoVRRenderer::initRttRendering()
{
	auto compositor = getRoot()->getCompositorManager2();

	//We loaded the HDR workspace from file earlier already
	compositorWorkspaces[monoCompositor] = compositor->addWorkspace(smgr, window, monoCam, "HdrWorkspace", true, 0, nullptr, nullptr, nullptr);
}

void AnnOgreNoVRRenderer::initClientHmdRendering()
{
	loadOpenGLFunctions();

	//No HMD to initialize rendering for.
}

bool AnnOgreNoVRRenderer::shouldQuit()
{
	return !running;
}

void AnnOgreNoVRRenderer::getTrackingPoseAndVRTiming()
{
	calculateTimingFromOgre();

	trackedHeadPose.position	= feetPosition + Annwvyn::AnnGetPlayer()->getEyeTranslation();
	trackedHeadPose.orientation = bodyOrientation;
}

void AnnOgreNoVRRenderer::renderAndSubmitFrame()
{
	handleWindowMessages();
	if(window->isClosed())
	{
		running = false;
		return;
	}

	root->renderOneFrame();
}

void AnnOgreNoVRRenderer::recenter()
{}

void AnnOgreNoVRRenderer::showDebug(DebugMode mode)
{}

void AnnOgreNoVRRenderer::updateEyeCameraFrustrum()
{
	if(!monoCam) return;

	//Here we don't use a custom projection matrix. Just tell the Ogre Camera to use the current near/far clip planes
	monoCam->setNearClipDistance(nearClippingDistance);
	monoCam->setFarClipDistance(farClippingDistance);
}

bool AnnOgreNoVRRenderer::shouldRecenter()
{
	return false;
}

bool AnnOgreNoVRRenderer::isVisibleInHmd()
{
	return true;
}

void AnnOgreNoVRRenderer::handleIPDChange()
{}

AnnOgreNoVRRenderer::~AnnOgreNoVRRenderer()
{
	noVRself = nullptr;
}
