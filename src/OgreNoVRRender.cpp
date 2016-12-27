#include "stdafx.h"
#include "OgreNoVRRender.hpp"

#include "AnnLogger.hpp"
#include "AnnGetter.hpp"

OgreNoVRRender* OgreNoVRRender::noVRself(nullptr);

OgreNoVRRender::OgreNoVRRender(std::string name) : OgreVRRender(name),
noVRViewport(nullptr),
then(0),
now(0),
running(true)
{
	noVRself = dynamic_cast<OgreNoVRRender*>(self);
}

void OgreNoVRRender::initPipeline()
{
}

void OgreNoVRRender::initVrHmd()
{
	getOgreConfig();
	createWindow();
	initScene();
	initCameras();
	initRttRendering();
}

void OgreNoVRRender::createWindow()
{
	Ogre::NameValuePairList misc;
	misc["vsync"] = "true";
	misc["top"] = "0";
	misc["left"] = "0";
	misc["FSAA"] = std::to_string(AALevel);
	root->initialise(false);

	const float w(1920 / 2), h(1080 / 2);

	window = root->createRenderWindow(name, w, h, false, &misc);
}

void OgreNoVRRender::initScene()
{
	smgr = root->createSceneManager("OctreeSceneManager", "OSMSMGR");
	smgr->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);
}

void OgreNoVRRender::initCameras()
{
	//TODO remove this method if it's useless.
	OgreVRRender::initCameras();
}

void OgreNoVRRender::initRttRendering()
{
	noVRViewport = window->addViewport(monoCam);
	noVRViewport->setBackgroundColour(backgroundColor);
}

void OgreNoVRRender::initClientHmdRendering()
{
	auto error = glewInit();
	if (error != GLEW_OK)
	{
		Annwvyn::AnnDebug() << "failed to glew init";
		throw std::runtime_error("Error : " + std::to_string(ANN_ERR_RENDER) + "Failed to load OpenGL functions with GLEW");
	}
}

bool OgreNoVRRender::shouldQuit() { return !running; }

void OgreNoVRRender::updateTracking()
{
	syncGameplayBody();
	calculateTimingFromOgre();

	trackedHeadPose.position = (feetPosition + Annwvyn::AnnGetPlayer()->getEyeTranslation());
	trackedHeadPose.orientation = (bodyOrientation);

	applyCameraRigPose(trackedHeadPose);
}

void OgreNoVRRender::renderAndSubmitFrame()
{
	Ogre::WindowEventUtilities::messagePump();
	if (window->isClosed())
	{
		running = false;
		return;
	}

	root->_fireFrameRenderingQueued();
	noVRViewport->update();
	window->update();
}

void OgreNoVRRender::recenter()
{
	return;
}

void OgreNoVRRender::changeViewportBackgroundColor(Ogre::ColourValue color)
{
	backgroundColor = color;

	noVRViewport->setBackgroundColour(color);
}

void OgreNoVRRender::showDebug(DebugMode mode)
{
	return;
}

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
{
	//No stereo, do nothing;
}