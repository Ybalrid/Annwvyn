#include "stdafx.h"
#include "OgreVRRender.hpp"

OgreVRRender* OgreVRRender::self = nullptr;

OgreVRRender::OgreVRRender(std::string windowName) :
	root(nullptr),
	smgr(nullptr),
	window(nullptr),
	updateTime(0),
	headPosition(0, 0, 10),
	headOrientation(Ogre::Quaternion::IDENTITY),
	nearClippingDistance(0.5f),
	farClippingDistance(4000.0f),
	headNode(nullptr),
	backgroundColor(0, 0.56f, 1),
	name(windowName),
	frameCounter(0),
	AALevel(16U)
{
	if (self)
	{
		MessageBox(NULL, L"Fatal error with renderer initialisation. OgreOculusRender object allready created.", L"Fatal Error", MB_ICONERROR);
		exit(ANN_ERR_RENDER);
	}
	self = this;

	eyeCameras[0] = nullptr;
	eyeCameras[1] = nullptr;
}

OgreVRRender::~OgreVRRender()
{
}

Ogre::SceneManager* OgreVRRender::getSceneManager()
{
	return smgr;
}

Ogre::Root* OgreVRRender::getRoot()
{
	return root;
}

Ogre::RenderWindow* OgreVRRender::getWindow()
{
	return window;
}

Ogre::SceneNode * OgreVRRender::getCameraInformationNode()
{
	return headNode;
}

Ogre::Timer * OgreVRRender::getTimer()
{
	if (root) return root->getTimer();
	return nullptr;
}

double OgreVRRender::getUpdateTime()
{
	return updateTime;
}

void OgreVRRender::initOgreRoot(std::string loggerName)
{
	//Create the ogre root with standards Ogre configuration file
	root = new Ogre::Root("", "ogre.cfg", loggerName.c_str());

	//Set the log verbosity to "bore me" 
	Ogre::LogManager::getSingleton().setLogDetail(Ogre::LoggingLevel::LL_BOREME);
}


