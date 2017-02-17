#include "stdafx.h"
#include "OgreVRRender.hpp"
#include "AnnGetter.hpp"
#include "AnnLogger.hpp"

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
	smgr{ nullptr },
	root{ nullptr },
	window{ nullptr },
	updateTime{ 0 },
	then{ 0 },
	now{ 0 },
	nearClippingDistance{ 0.1f },
	farClippingDistance{ 500.0f },
	feetPosition{ 0, 0, 10 },
	bodyOrientation{ Ogre::Quaternion::IDENTITY },
	name{ windowName },
	gameplayCharacterRoot{ nullptr },
	backgroundColor{ 0, 0.56f, 1 },
	cameraRig{ nullptr },
	frameCounter{ 0 },
	rttEyes{ nullptr }
{
	rttTexture.setNull();
	if (self)
	{
		displayWin32ErrorMessage(L"Fatal Error", L"Fatal error with renderer initialization. OgreOculusRender object already created.");
		throw std::runtime_error("Error : " + std::to_string(ANN_ERR_CRITIC) + "Cannot create more than one OgreVRRenderer object!");
	}
	self = this;

	for (auto& eyeCamera : eyeCameras)
		eyeCamera = nullptr;
	for (auto& handController : handControllers)
		handController = nullptr;
}

OgreVRRender::~OgreVRRender()
{
	self = nullptr;
	root->unloadPlugin(PluginOctreeSceneManager);
}

Ogre::SceneManager* OgreVRRender::getSceneManager() const
{
	return smgr;
}

Ogre::Root* OgreVRRender::getRoot() const
{
	return root.get();
}

Ogre::RenderWindow* OgreVRRender::getWindow() const
{
	return window;
}

Ogre::SceneNode* OgreVRRender::getCameraInformationNode() const
{
	return gameplayCharacterRoot;
}

Ogre::Timer * OgreVRRender::getTimer() const
{
	if (root) return root->getTimer();
	return nullptr;
}

double OgreVRRender::getUpdateTime() const
{
	return updateTime;
}

void OgreVRRender::initOgreRoot(std::string loggerName)
{
	//Create the ogre root with standards Ogre configuration file
	root = std::make_unique<Ogre::Root>("", "ogre.cfg", loggerName.c_str());

	//Set the log verbosity to "bore me"
	Ogre::LogManager::getSingleton().setLogDetail(Ogre::LoggingLevel::LL_BOREME);
}

void OgreVRRender::getOgreConfig() const
{
	//Ogre as to be initialized
	if (!root) throw std::runtime_error("Error : " + std::to_string(ANN_ERR_NOTINIT) + "Need to initialize Ogre::Root before loading system configuration");

	//Load OgrePlugins
	root->loadPlugin(PluginRenderSystemGL);
	root->loadPlugin(PluginOctreeSceneManager);

	//Set the classic OpenGL render system
	root->setRenderSystem(root->getRenderSystemByName(GLRenderSystem));
	root->getRenderSystem()->setFixedPipelineEnabled(true);
	root->getRenderSystem()->setConfigOption("RTT Preferred Mode", "FBO");
	root->getRenderSystem()->setConfigOption("FSAA", std::to_string(AALevel));
}

std::array<std::shared_ptr<Annwvyn::AnnHandController>, MAX_CONTROLLER_NUMBER> OgreVRRender::getHandControllerArray() const
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

void OgreVRRender::changedAA() const
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

void OgreVRRender::initCameras()
{
	cameraRig = smgr->getRootSceneNode()->createChildSceneNode("CameraRig");

	eyeCameras[left] = smgr->createCamera("lcam");
	eyeCameras[left]->setAutoAspectRatio(true);
	cameraRig->attachObject(eyeCameras[left]);

	eyeCameras[right] = smgr->createCamera("rcam");
	eyeCameras[right]->setAutoAspectRatio(true);
	cameraRig->attachObject(eyeCameras[right]);

	monoCam = smgr->createCamera("mcam");
	monoCam->setAspectRatio(16.0 / 9.0);
	monoCam->setAutoAspectRatio(false);
	monoCam->setNearClipDistance(nearClippingDistance);
	monoCam->setFarClipDistance(farClippingDistance);
	monoCam->setFOVy(Ogre::Degree(90));
	cameraRig->attachObject(monoCam);

	//do NOT attach camera to this node...
	gameplayCharacterRoot = smgr->getRootSceneNode()->createChildSceneNode();
}

void OgreVRRender::applyCameraRigPose(OgrePose pose) const
{
	cameraRig->setPosition(pose.position);
	cameraRig->setOrientation(pose.orientation);
}

void OgreVRRender::syncGameplayBody()
{
	//Get current camera base information
	feetPosition = gameplayCharacterRoot->getPosition();
	bodyOrientation = gameplayCharacterRoot->getOrientation();
}

void OgreVRRender::calculateTimingFromOgre()
{
	then = now;
	now = getTimer()->getMilliseconds() / 1000.0;
	updateTime = now - then;
}

void OgreVRRender::loadOpenGLFunctions()
{
	const auto err = glewInit();
	if (err != GLEW_OK)
	{
		Annwvyn::AnnDebug() << "Failed to glewTnit(), error : "
			<< glewGetString(err);
		exit(ANN_ERR_RENDER);
	}
	Annwvyn::AnnDebug() << "Using GLEW version : "
		<< glewGetString(GLEW_VERSION);
}

void OgreVRRender::updateTracking()
{
	syncGameplayBody();
	getTrackingPoseAndVRTiming();
	applyCameraRigPose(trackedHeadPose);
}

void OgreVRRender::initPipeline()
{
	getOgreConfig();
	if (rendererName.find("NoVR") != std::string::npos)
		createWindow(1280, 720, true);
	else
		createWindow();
	initScene();
	initCameras();
	initRttRendering();
	updateProjectionMatrix();
}

GLuint OgreVRRender::createRenderTexture(float w, float h)
{
	GLuint glid;
	rttTexture = Ogre::TextureManager::getSingleton().createManual(rttTextureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D, w, h, 0, Ogre::PF_R8G8B8A8, Ogre::TU_RENDERTARGET, nullptr, false, AALevel);
	rttTexture->getCustomAttribute("GLID", &glid);
	rttEyes = rttTexture->getBuffer()->getRenderTarget();
	return glid;
}

std::tuple<Ogre::TexturePtr, unsigned int> OgreVRRender::createAdditionalRenderBuffer(float w, float h, std::string additionalTextureName) const
{
	static int counter;
	if (additionalTextureName.empty())
		additionalTextureName = "additionalTexture" + std::to_string(counter++);

	auto texture{ Ogre::TextureManager::getSingleton().createManual
		(additionalTextureName,
					 Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
					 Ogre::TEX_TYPE_2D, w, h, 9,
					 Ogre::PF_R8G8B8A8) };

	unsigned int glid;
	texture->getCustomAttribute("GLID", &glid);
	return std::tie(texture, glid);
}

void OgreVRRender::createWindow(unsigned int w, unsigned int h, bool vsync)
{
	Ogre::NameValuePairList options;
	options["FSAA"] = std::to_string(AALevel);
	options["top"] = "0";
	options["left"] = "0";
	if (vsync)
		options["vsync"] = "true";
	else
		options["vsync"] = "false";

	root->initialise(false);
	window = root->createRenderWindow(rendererName + " : " + name + " - monitor output",
		w, h, false, &options);
}

std::string OgreVRRender::getName() const
{
	return rendererName;
}

bool OgreVRRender::handControllersAvailable() const
{
	return handControllers[left].get() && handControllers[right].get();
}