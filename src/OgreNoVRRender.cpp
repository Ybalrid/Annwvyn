#include "stdafx.h"
#include "OgreNoVRRender.hpp"

#include "AnnLogger.hpp"

OgreNoVRRender* OgreNoVRRender::noVRself(nullptr);

OgreNoVRRender::OgreNoVRRender(std::string name) : OgreVRRender(name),
noVRCam(nullptr),
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
	noVRCam = smgr->createCamera("no_vr_cam");
	noVRCam->setAutoAspectRatio(true);
	noVRCam->setNearClipDistance(nearClippingDistance);
	noVRCam->setFarClipDistance(farClippingDistance);
	noVRCam->setFOVy(Ogre::Degree(90));

	headNode = smgr->getRootSceneNode()->createChildSceneNode();
}

void OgreNoVRRender::initRttRendering()
{
	noVRViewport = window->addViewport(noVRCam);
	noVRViewport->setBackgroundColour(backgroundColor);
}

void OgreNoVRRender::initClientHmdRendering()
{
	auto error = glewInit();
	if (error != GLEW_OK)
	{
		Annwvyn::AnnDebug() << "failed to glew init";
		exit(ANN_ERR_RENDER);
	}
	return;
}

void OgreNoVRRender::updateTracking()
{
	feetPosition = headNode->getPosition();
	bodyOrientation = headNode->getOrientation();

	then = now;
	now = getTimer()->getMilliseconds() / 1000.0;
	updateTime = now - then;

	noVRCam->setPosition(feetPosition + Annwvyn::AnnGetPlayer()->getEyeTranslation());
	noVRCam->setOrientation(bodyOrientation);

	returnPose.position = noVRCam->getPosition();
	returnPose.orientation = noVRCam->getOrientation();
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
	if (!noVRCam) return;

	//Here we don't use a custom projection matrix. Just tell the Ogre Camera to use the current near/far clip planes
	noVRCam->setNearClipDistance(nearClippingDistance);
	noVRCam->setFarClipDistance(farClippingDistance);
}