#include "stdafx.h"
#include "OgreNoVRRender.hpp"

#include "AnnLogger.hpp"
#include "AnnGetter.hpp"

OgreNoVRRender* OgreNoVRRender::noVRself(nullptr);

OgreNoVRRender::OgreNoVRRender(std::string name) : OgreVRRender(name),
noVRViewport(nullptr),
running(true)
{
	rendererName = "OpenGL/NoVR";
	noVRself = dynamic_cast<OgreNoVRRender*>(self);
}

void OgreNoVRRender::initVrHmd()
{}

void OgreNoVRRender::initScene()
{	
	createMainSmgr();
}

///TODO move that to the Scenery Manager
void OgreNoVRRender::initRttRendering()
{
	auto compositor = getRoot()->getCompositorManager2();			
	float multiplier = 60.0f;
	auto skyColor = backgroundColor;
	auto renderingNodeName = "HdrRenderingNode";

	compositor->createBasicWorkspaceDef(monoscopicWorkspaceName, backgroundColor);
	//compositorWorkspaces[monoCompositor] = compositor->addWorkspace(smgr, window, monoCam, monoscopicCompositor, true, 0, nullptr, nullptr, nullptr, Ogre::Vector4(0, 0, 1, 1), 0x03, 0x03);
	//We loaded the HDR workspace from file earlier already
	compositorWorkspaces[monoCompositor] = compositor->addWorkspace(smgr, window, monoCam, "HdrWorkspace", true, 0, nullptr, nullptr, nullptr);

}

void OgreNoVRRender::initClientHmdRendering()
{
	loadOpenGLFunctions();

	//No HMD to initialize rendering for.
}

bool OgreNoVRRender::shouldQuit()
{
	return !running;
}

void OgreNoVRRender::getTrackingPoseAndVRTiming()
{
	calculateTimingFromOgre();

	trackedHeadPose.position = feetPosition + Annwvyn::AnnGetPlayer()->getEyeTranslation();
	trackedHeadPose.orientation = bodyOrientation;
}

void OgreNoVRRender::renderAndSubmitFrame()
{
	Ogre::WindowEventUtilities::messagePump();
	if (window->isClosed())
	{
		running = false;
		return;
	}

	root->renderOneFrame();
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void OgreNoVRRender::recenter()
{}

///TODO This is deprecated and will be removed
void OgreNoVRRender::changeViewportBackgroundColor(Ogre::ColourValue color)
{
	backgroundColor = color;
}

void OgreNoVRRender::showDebug(DebugMode mode)
{}

void OgreNoVRRender::updateProjectionMatrix()
{
	if (!monoCam) return;

	//Here we don't use a custom projection matrix. Just tell the Ogre Camera to use the current near/far clip planes
	monoCam->setNearClipDistance(nearClippingDistance);
	monoCam->setFarClipDistance(farClippingDistance);
}

bool OgreNoVRRender::shouldRecenter()
{
	return false;
}

bool OgreNoVRRender::isVisibleInHmd()
{
	return true;
}

void OgreNoVRRender::handleIPDChange()
{}

OgreNoVRRender::~OgreNoVRRender()
{
	noVRself = nullptr;
	rttTexture.setNull();
}