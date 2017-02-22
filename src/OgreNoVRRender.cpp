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
	/*
	smgr = root->createSceneManager("OctreeSceneManager", "OSMSMGR");
	smgr->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);
	*/
	
	//TODO make the amont of threads here a parameter
	smgr = root->createSceneManager(Ogre::ST_GENERIC, 4, Ogre::INSTANCING_CULLING_THREADED);

}

void OgreNoVRRender::initRttRendering()
{
	auto compositor = getRoot()->getCompositorManager2();
	if (!compositor->hasWorkspaceDefinition(monoscopicCompositor))
		compositor->createBasicWorkspaceDef(monoscopicWorkspaceName, backgroundColor);
	auto def = compositor->getWorkspaceDefinition(monoscopicCompositor);


	compositorWorkspaces[2] = compositor->addWorkspace(smgr, window, monoCam, monoscopicCompositor, true, 0, nullptr, nullptr, nullptr, Ogre::Vector4(0, 0, 1, 1), 0x03, 0x03);
	//compositorWorkspaces[2]->get
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

	// TODO update compositor instead
	/*root->_fireFrameRenderingQueued();
	noVRViewport->update();
	window->update();
	*/
	//Sleep for one ms

	root->renderOneFrame();
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void OgreNoVRRender::recenter()
{}

void OgreNoVRRender::changeViewportBackgroundColor(Ogre::ColourValue color)
{
	backgroundColor = color;

	//TODO AFAIK, changing the viewport clear color is a bit tricky. Investigate
	//noVRViewport->setBackgroundColour(color);
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
	root->destroySceneManager(smgr);
	noVRself = nullptr;
	rttTexture.setNull();
}