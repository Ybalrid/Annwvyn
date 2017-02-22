#include "stdafx.h"
#include "OgreVRRender.hpp"
#include "AnnGetter.hpp"
#include "AnnLogger.hpp"
#include "Annwvyn.h"


auto logToOgre = [] (const std::string& str) {Ogre::LogManager::getSingleton().logMessage(str); };

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
	rttEyes{ nullptr },
	glMajor{ 4 },
	glMinor{ 3 },
	monoscopicCompositor(monoscopicWorkspaceName),
	stereoscopicCompositor(stereoscopicWorkspaceName)
{
	rttTexture.setNull();
	if (self)
	{
		displayWin32ErrorMessage(L"Fatal Error", L"Fatal error with renderer initialization. OgreOculusRender object already created.");
		throw Annwvyn::AnnInitializationError(ANN_ERR_CRITIC, "Cannot create more than one OgreVRRenderer object!");
	}
	self = this;

	for (auto& eyeCamera : eyeCameras)
		eyeCamera = nullptr;
	for (auto& handController : handControllers)
		handController = nullptr;
	for (auto& compositor : compositorWorkspaces)
		compositor = nullptr;
}

OgreVRRender::~OgreVRRender()
{
	self = nullptr;
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
	if (!root) throw Annwvyn::AnnInitializationError(ANN_ERR_NOTINIT, "Need to initialize Ogre::Root before loading system configuration");

	//Load OgrePlugins
	root->loadPlugin(PluginRenderSystemGL);
	//root->loadPlugin(PluginOctreeSceneManager);

	//Set the classic OpenGL render system
	root->setRenderSystem(root->getRenderSystemByName(GLRenderSystem));
	//root->getRenderSystem()->setFixedPipelineEnabled(true); //NO MORE FIXED PIPELINE
	//root->getRenderSystem()->setConfigOption("RTT Preferred Mode", "FBO");
	root->getRenderSystem()->setConfigOption("FSAA", std::to_string(AALevel));


	root->initialise(false);
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
	//TODO look into FSAA 
	//if (rttTexture.getPointer() && !UseSSAA) rttTexture->setFSAA(AALevel);
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

void OgreVRRender::detachCameraFromParent(Ogre::Camera* camera)
{
	if (auto parent = camera->getParentSceneNode())
		parent->detachObject(camera);
}

void OgreVRRender::initCameras()
{
	//TODO name on node?
	cameraRig = smgr->getRootSceneNode()->createChildSceneNode();

	eyeCameras[left] = smgr->createCamera("lcam");
	eyeCameras[left]->setAutoAspectRatio(true);
	detachCameraFromParent(eyeCameras[left]);
	cameraRig->attachObject(eyeCameras[left]);

	eyeCameras[right] = smgr->createCamera("rcam");
	eyeCameras[right]->setAutoAspectRatio(true);
	detachCameraFromParent(eyeCameras[right]);
	cameraRig->attachObject(eyeCameras[right]);

	monoCam = smgr->createCamera("mcam");
	monoCam->setAspectRatio(16.0 / 9.0);
	monoCam->setAutoAspectRatio(false);
	monoCam->setNearClipDistance(nearClippingDistance);
	monoCam->setFarClipDistance(farClippingDistance);
	monoCam->setFOVy(Ogre::Degree(90));
	detachCameraFromParent(monoCam);
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
	loadHLMSLibrary();
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

	logToOgre("call glfwInit()");
	glfwInit();
	//Specify OpenGL version 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	logToOgre("Set OpenGL context version " + std::to_string(glMajor) + "." + std::to_string(glMinor));

	//Create a window (and an opengl context with it)
	auto winName = rendererName + " : " + name + " - monitor output";
	glfwWindow = glfwCreateWindow(w, h, winName.c_str(), nullptr, nullptr);


	//Make the created context current
	glfwMakeContextCurrent(glfwWindow);

	//Get the hwnd and the context :
	HGLRC context{ wglGetCurrentContext() };
	HWND handle{ glfwGetWin32Window(glfwWindow) };

	Ogre::NameValuePairList options;
	options["externalWindowHandle"] = std::to_string(size_t(handle));
	options["externalGLContext"] = std::to_string(size_t(context));
	options["FSAA"] = std::to_string(AALevel);
	options["top"] = "0";
	options["left"] = "0";
	if (vsync)
		options["vsync"] = "true";
	else
		options["vsync"] = "false";

	window = root->createRenderWindow(winName, w, h, false, &options);
}

std::string OgreVRRender::getName() const
{
	return rendererName;
}

bool OgreVRRender::handControllersAvailable() const
{
	return handControllers[left].get() && handControllers[right].get();
}

void OgreVRRender::loadHLMSLibrary(const std::string& path)
{
	auto hlmsFolder = path;

	//The hlmsFolder can come from a configuration file where it could be "empty" or set to "." or lacking the trailing "/"
	if (hlmsFolder.empty()) hlmsFolder = "./";
	else if (hlmsFolder[hlmsFolder.size() - 1] != '/') hlmsFolder += "/";

	//Get the hlmsManager (not a singleton by itself, but accessible via Root)
	auto hlmsManager = Ogre::Root::getSingleton().getHlmsManager();

	//Define the shader library to use for HLMS
	auto library = Ogre::ArchiveVec();
	auto archiveLibrary = Ogre::ArchiveManager::getSingletonPtr()->load(hlmsFolder + "Hlms/Common/" + SL, "FileSystem", true);
	library.push_back(archiveLibrary);

	//Define "unlit" and "PBS" (physics based shader) HLMS
	auto archiveUnlit = Ogre::ArchiveManager::getSingletonPtr()->load(hlmsFolder + "Hlms/Unlit/" + SL, "FileSystem", true);
	auto archivePbs = Ogre::ArchiveManager::getSingletonPtr()->load(hlmsFolder + "Hlms/Pbs/" + SL, "FileSystem", true);
	auto hlmsUnlit = OGRE_NEW Ogre::HlmsUnlit(archiveUnlit, &library);
	auto hlmsPbs = OGRE_NEW Ogre::HlmsPbs(archivePbs, &library);
	hlmsManager->registerHlms(hlmsUnlit);
	hlmsManager->registerHlms(hlmsPbs);
}
