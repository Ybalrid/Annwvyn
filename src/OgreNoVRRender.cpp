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
	//TODO make the amont of threads here a parameter
	smgr = root->createSceneManager(Ogre::ST_GENERIC, 4, Ogre::INSTANCING_CULLING_THREADED);
	smgr->setShadowDirectionalLightExtrusionDistance(500.0f);
	smgr->setShadowFarDistance(500.0f);
}

///TODO move that to the Scenery Manager
void OgreNoVRRender::initRttRendering()
{
	auto compositor = getRoot()->getCompositorManager2();			
	float multiplier = 60.0f;
	auto skyColor = backgroundColor;
	auto renderingNodeName = "MyHdrRenderingNode";

	compositor->createBasicWorkspaceDef(monoscopicWorkspaceName, backgroundColor);
	//compositorWorkspaces[2] = compositor->addWorkspace(smgr, window, monoCam, monoscopicCompositor, true, 0, nullptr, nullptr, nullptr, Ogre::Vector4(0, 0, 1, 1), 0x03, 0x03);
	//We loaded the HDR workspace from file earlier already
	compositorWorkspaces[2] = compositor->addWorkspace(smgr, window, monoCam, "MyHdrWorkspace", true, 0, nullptr, nullptr, nullptr, Ogre::Vector4(0, 0, 1, 1), 0x03, 0x03);

	setSkyColor(skyColor, multiplier, renderingNodeName);
	setExposure(0, -1, 2.5f);

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