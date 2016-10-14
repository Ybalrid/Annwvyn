#include "stdafx.h"
#include "OgreVRRender.hpp"

OgreVRRender* OgreVRRender::self = nullptr;

OgreVRRender::OgreVRRender(std::string windowName) :
	root(nullptr),
	smgr(nullptr),
	window(nullptr),
	updateTime(0),
	feetPosition(0, 0, 10),
	bodyOrientation(Ogre::Quaternion::IDENTITY),
	nearClippingDistance(0.1f),
	farClippingDistance(500.0f),
	headNode(nullptr),
	backgroundColor(0, 0.56f, 1),
	name(windowName),
	frameCounter(0),
	AALevel(16U)
{
	if (self)
	{
		displayWin32ErrorMessage(L"Fatal Error", L"Fatal error with renderer initialization. OgreOculusRender object already created.");
		exit(ANN_ERR_RENDER);
	}
	self = this;

	eyeCameras[0] = nullptr;
	eyeCameras[1] = nullptr;

	handControllers[0] = nullptr;
	handControllers[1] = nullptr;
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

Ogre::SceneNode* OgreVRRender::getCameraInformationNode()
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

void OgreVRRender::getOgreConfig()
{
	//Ogre as to be initialized
	if (!root) exit(ANN_ERR_NOTINIT);

	//Load OgrePlugins
	root->loadPlugin("RenderSystem_GL");
	root->loadPlugin("Plugin_OctreeSceneManager");

	//Set the classic OpenGL render system
	root->setRenderSystem(root->getRenderSystemByName("OpenGL Rendering Subsystem"));
	root->getRenderSystem()->setFixedPipelineEnabled(true);
	root->getRenderSystem()->setConfigOption("RTT Preferred Mode", "FBO");
	root->getRenderSystem()->setConfigOption("FSAA", std::to_string(AALevel));
}

std::shared_ptr<Annwvyn::AnnHandController>* OgreVRRender::getHandControllerArray()
{
	return handControllers;
}

size_t OgreVRRender::getHanControllerArraySize()
{
	return MAX_CONTROLLER_NUMBER;
}

void OgreVRRender::setNearClippingDistance(float distance)
{
	nearClippingDistance = distance;
	updateProjectionMatrix();
}

void OgreVRRender::setFarClippingDistance(float distance)
{
	farClippingDistance = distance;
	updateProjectionMatrix();
}