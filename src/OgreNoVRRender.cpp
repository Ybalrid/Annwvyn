#include "stdafx.h"
#include "OgreNoVRRender.hpp"

OgreNoVRRender* OgreNoVRRender::noVRself(nullptr);

OgreNoVRRender::OgreNoVRRender(std::string name) : OgreVRRender(name),
noVRCam(nullptr)
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
	root->initialise(false);

	float w(1920 / 2), h(1080 / 2);

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
	return;
}

void OgreNoVRRender::updateTracking()
{
	noVRCam->setPosition(feetPosition);
	noVRCam->setOrientation(bodyOrientation);
}

void OgreNoVRRender::renderAndSubmitFrame()
{
	Ogre::WindowEventUtilities::messagePump();

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

void OgreNoVRRender::setCamerasNearClippingDistance(float distance)
{
	if (distance <= 0) return;
	nearClippingDistance = distance;
	noVRCam->setNearClipDistance(distance);
}

void OgreNoVRRender::setCameraFarClippingDistance(float distance)
{
	if (distance <= 0) return;
	farClippingDistance = distance;
	noVRCam->setFarClipDistance(distance);
}

void OgreNoVRRender::showDebug(DebugMode mode)
{
	return;
}
