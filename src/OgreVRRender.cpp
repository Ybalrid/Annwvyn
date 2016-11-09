#include "stdafx.h"
#include "OgreVRRender.hpp"

uint8_t OgreVRRender::AALevel{ 4 };
OgreVRRender* OgreVRRender::self{ nullptr };
bool OgreVRRender::UseSSAA{ false };

void OgreVRRender::setAntiAliasingLevel(const uint8_t AA)
{
	for (const auto& possibleAALevel : AvailableAALevel)
		if (possibleAALevel == AA)
		{
			AALevel = AA;
			if (self)
				return self->changedAA();
			break;
		}
}

OgreVRRender::OgreVRRender(std::string windowName) :
	smgr(nullptr),
	root(nullptr),
	window(nullptr),
	updateTime(0),
	nearClippingDistance(0.1f),
	farClippingDistance(500.0f),
	feetPosition(0, 0, 10),
	bodyOrientation(Ogre::Quaternion::IDENTITY),
	name(windowName),
	headNode(nullptr),
	backgroundColor(0, 0.56f, 1),
	frameCounter(0),
	rttEyes(nullptr)
{
	rttTexture.setNull();
	if (self)
	{
		displayWin32ErrorMessage(L"Fatal Error", L"Fatal error with renderer initialization. OgreOculusRender object already created.");
		exit(ANN_ERR_RENDER);
	}
	self = this;

	for (auto& eyeCamera : eyeCameras)
		eyeCamera = nullptr;
	for (auto& handController : handControllers)
		handController = nullptr;
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

std::array<std::shared_ptr<Annwvyn::AnnHandController>, MAX_CONTROLLER_NUMBER> OgreVRRender::getHandControllerArray()
{
	return handControllers;
}

size_t OgreVRRender::getHanControllerArraySize()
{
	return MAX_CONTROLLER_NUMBER;
}

size_t OgreVRRender::getRecognizedControllerCount()
{
	auto count = size_t{ 0 };
	for (auto handController : handControllers)
		if (handController)
			count++;
	return count;
}

void OgreVRRender::changedAA()
{
	if (rttTexture.getPointer() && !UseSSAA) rttTexture->setFSAA(AALevel, "");
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