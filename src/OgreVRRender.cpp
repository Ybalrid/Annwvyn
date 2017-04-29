#include "stdafx.h"
#include "OgreVRRender.hpp"
#include "AnnGetter.hpp"
#include "AnnLogger.hpp"
#include "Annwvyn.h"

#ifdef __linux__
#include <X11/Xlib.h>
#endif

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NAVIVE_WGL
#endif

#ifdef __linux__
#define GLFW_EXPOSE_NATIVE_X11
#endif

#include <GLFW/glfw3native.h>

auto logToOgre = [](const std::string& str) {Ogre::LogManager::getSingleton().logMessage(str); };

uint8_t OgreVRRender::AALevel{ 4 };
OgreVRRender* OgreVRRender::self{ nullptr };
bool OgreVRRender::UseSSAA{ false };

void OgreVRRender::setAntiAliasingLevel(const uint8_t AA)
{
	static const std::array<const uint8_t, 5> AvailableAALevel{ 0,2,4,8,16 };
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
	numberOfThreads(std::thread::hardware_concurrency()),
	glMajor{ 4 },
	glMinor{ 3 },
	smgr{ nullptr },
	root{ nullptr },
	window{ nullptr },
	updateTime{ 0 },
	then{ 0 },
	now{ 0 },
	nearClippingDistance{ 0.01f },
	farClippingDistance{ 500.0f },
	feetPosition{ 0, 0, 10 },
	bodyOrientation{ Ogre::Quaternion::IDENTITY },
	name{ windowName },
	gameplayCharacterRoot{ nullptr },
	cameraRig{ nullptr },
	frameCounter{ 0 },
	rttEyesCombined{ nullptr }
{
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
	Ogre::LogManager::getSingleton().logMessage("OgreVRRender::~OgreVRRender()");

	//For good measure : destroy the Ogre window
	root->destroyRenderTarget(window);

	//We need to stop OGRE before destroying the GLFW window.
	root.reset();

	//Clean GLFW
	glfwDestroyWindow(glfwWindow);
	glfwTerminate();

	//Reset the singleton
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
	return root->getTimer();
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
	root->loadPlugin(PluginRenderSystemGL3Plus);

	//Set the classic OpenGL render system
	root->setRenderSystem(root->getRenderSystemByName(GLRenderSystem3Plus));
	//	root->getRenderSystem()->setConfigOption("FSAA", std::to_string(AALevel));
	root->getRenderSystem()->setConfigOption("sRGB Gamma Conversion", "Yes");
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

void OgreVRRender::changedAA() const
{
	root->getRenderSystem()->setConfigOption("FSAA", std::to_string(AALevel));
	window->setFSAA(AALevel, "");
	auto textureManager = Ogre::TextureManager::getSingletonPtr();
	auto texture = textureManager->getByName(rttTextureName);
	if (texture)
	{
		texture->setFSAA(AALevel, "", false);
	}
	else
	{
		for (auto i{ 0 }; i < 2; i++)
		{
			texture = textureManager->getByName(rttTextureName + std::to_string(i));
			if (texture) texture->setFSAA(AALevel, "", false);
		}
	}
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
	cameraRig = smgr->getRootSceneNode()->createChildSceneNode();
	cameraRig->setName("CameraRig");

	eyeCameras[left] = smgr->createCamera("lcam");
	eyeCameras[left]->setNearClipDistance(nearClippingDistance);
	eyeCameras[left]->setFarClipDistance(farClippingDistance);
	detachCameraFromParent(eyeCameras[left]);
	cameraRig->attachObject(eyeCameras[left]);

	eyeCameras[right] = smgr->createCamera("rcam");
	eyeCameras[right]->setNearClipDistance(nearClippingDistance);
	eyeCameras[right]->setFarClipDistance(farClippingDistance);
	detachCameraFromParent(eyeCameras[right]);
	cameraRig->attachObject(eyeCameras[right]);

	monoCam = smgr->createCamera("mcam");
	monoCam->setNearClipDistance(nearClippingDistance);
	monoCam->setFarClipDistance(farClippingDistance);
	monoCam->setFOVy(Ogre::Degree(90));
	monoCam->setAutoAspectRatio(true);
	detachCameraFromParent(monoCam);
	cameraRig->attachObject(monoCam);

	//do NOT attach camera to this node...
	gameplayCharacterRoot = smgr->getRootSceneNode()->createChildSceneNode();
}

void OgreVRRender::applyCameraRigPose(OgrePose pose) const
{
	cameraRig->setPosition(static_cast<Ogre::Vector3>(pose.position));
	cameraRig->setOrientation(static_cast<Ogre::Quaternion>(pose.orientation));
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

	if (glCopyImageSubData)
	{
		Annwvyn::AnnDebug() << "glCopyImageSubData is available!";
	}
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
	loadCompositor();
	initRttRendering();
	updateProjectionMatrix();
	loadHLMSLibrary();
}

GLuint OgreVRRender::createCombinedRenderTexture(float w, float h)
{
	GLuint glid;
	auto rttTextureCombined = Ogre::TextureManager::getSingleton().createManual(rttTextureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D, w, h, 0, Ogre::PF_R8G8B8A8, Ogre::TU_RENDERTARGET, nullptr, true, AALevel);
	rttTextureCombined->getCustomAttribute("GLID", &glid);
	rttEyesCombined = rttTextureCombined->getBuffer()->getRenderTarget();
	return glid;
}

std::array<GLuint, 2> OgreVRRender::createSeparatedRenderTextures(const std::array<std::array<size_t, 2>, 2>& dimentions)
{
	std::array <GLuint, 2> glid;
	Annwvyn::AnnDebug() << "Creating separated render textures " << dimentions[0][0] << "x" << dimentions[0][1] << " " << dimentions[1][0] << "x" << dimentions[1][1];
	std::array<Ogre::TexturePtr, 2> rttTexturesSeparated;
	for (size_t i : {0u, 1u})
	{
		rttTexturesSeparated[i] = Ogre::TextureManager::getSingleton().createManual(std::string(rttTextureName) + std::to_string(i),
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
			Ogre::uint(dimentions[i][0]), Ogre::uint(dimentions[i][1]), 0, Ogre::PF_R8G8B8A8, Ogre::TU_RENDERTARGET, nullptr, true, AALevel);
		rttTexturesSeparated[i]->getCustomAttribute("GLID", &glid[i]);
		rttEyeSeparated[i] = rttTexturesSeparated[i]->getBuffer(0)->getRenderTarget(0);
	}
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
	windowW = w, windowH = h;
	auto winName = rendererName + " : " + name + " - monitor output";

	logToOgre("call glfwInit()");
	glfwInit();
	//Specify OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, AALevel);

	logToOgre("Set OpenGL context version " + std::to_string(glMajor) + "." + std::to_string(glMinor));

	//Create a window (and an opengl context with it)
	glfwWindow = glfwCreateWindow(w, h, winName.c_str(), nullptr, nullptr);

	//Make the created context current
	glfwMakeContextCurrent(glfwWindow);

	Ogre::NameValuePairList options;

#ifdef _WIN32
	HGLRC context = {};
	HWND handle = {};
	//Get the hwnd and the context :
	context = wglGetCurrentContext();
	handle = glfwGetWin32Window(glfwWindow);
	options["externalWindowHandle"] = std::to_string(size_t(handle));
	options["externalGLContext"] = std::to_string(size_t(context));
#endif

#ifdef __linux__
	Window handle = {};
	void* context = nullptr;
	handle = glfwGetX11Window(glfwWindow);
	options["parentWindowHandle"] = std::to_string(size_t(handle));
	options["externalGLContext"] = std::to_string(size_t(context));
#endif

	options["FSAA"] = std::to_string(AALevel);
	options["top"] = "0";
	options["left"] = "0";
	options["gamma"] = "true";
	options["vsync"] = vsync ? "true" : "false";

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

void OgreVRRender::makeValidPath(std::string& path)
{
	if (path.empty()) path = "./";
	else if (path[path.size() - 1] != '/') path += "/";
}

void OgreVRRender::loadHLMSLibrary(const std::string& path)
{
	auto hlmsFolder = path;

	//The hlmsFolder can come from a configuration file where it could be "empty" or set to "." or lacking the trailing "/"
	makeValidPath(hlmsFolder);

	//Get the hlmsManager (not a singleton by itself, but accessible via Root)
	auto hlmsManager = root->getHlmsManager();

	//Define the shader library to use for HLMS
	auto library = Ogre::ArchiveVec();
	auto archiveLibrary = Ogre::ArchiveManager::getSingletonPtr()->load(hlmsFolder + "Hlms/Common/" + SL, "FileSystem", true);
	auto archiveLibraryAny = Ogre::ArchiveManager::getSingletonPtr()->load(hlmsFolder + "Hlms/Common/Any", "FileSystem", true);
	auto archivePbsLibraryAny = Ogre::ArchiveManager::getSingletonPtr()->load(hlmsFolder + "Hlms/Pbs/Any", "FileSystem", true);
	library.push_back(archiveLibrary);
	library.push_back(archiveLibraryAny);
	library.push_back(archivePbsLibraryAny);

	//Define "unlit" and "PBS" (physics based shader) HLMS
	auto archiveUnlit = Ogre::ArchiveManager::getSingletonPtr()->load(hlmsFolder + "Hlms/Unlit/" + SL, "FileSystem", true);
	auto archivePbs = Ogre::ArchiveManager::getSingletonPtr()->load(hlmsFolder + "Hlms/Pbs/" + SL, "FileSystem", true);
	auto hlmsUnlit = OGRE_NEW Ogre::HlmsUnlit(archiveUnlit, &library);
	auto hlmsPbs = OGRE_NEW Ogre::HlmsPbs(archivePbs, &library);
	hlmsManager->registerHlms(hlmsUnlit);
	hlmsManager->registerHlms(hlmsPbs);

	//Set the best shadows we can do by default
	hlmsPbs->setShadowSettings(Ogre::HlmsPbs::ShadowFilter::PCF_4x4);
}

void OgreVRRender::loadCompositor(const std::string& path, const std::string& type)
{
	auto compositorFolder = path;
	makeValidPath(compositorFolder);
	auto resourceGroupManager = Ogre::ResourceGroupManager::getSingletonPtr();
	//TODO maybe package the compositor differently
	resourceGroupManager->addResourceLocation(compositorFolder, type, RESOURCE_GROUP_COMPOSITOR);
	resourceGroupManager->initialiseResourceGroup(RESOURCE_GROUP_COMPOSITOR);
}

void OgreVRRender::setSkyColor(Ogre::ColourValue skyColor, float multiplier, const char* renderingNodeName) const
{
	auto compositor = root->getCompositorManager2();
	auto renderingNodeDef = compositor->getNodeDefinitionNonConst(renderingNodeName);
	auto targetDef = renderingNodeDef->getTargetPass(0);
	auto& passDefs = targetDef->getCompositorPasses();
	for (auto pass : passDefs) if (pass->getType() == Ogre::PASS_CLEAR)
	{
		auto clearDef = dynamic_cast<Ogre::CompositorPassClearDef*>(pass);
		if (clearDef)
		{
			clearDef->mColourValue = skyColor * multiplier;
		}
	}
}

void OgreVRRender::setExposure(float exposure, float minAuto, float maxAuto, const char* postProcessMaterial) const
{
	auto materialManager = Ogre::MaterialManager::getSingletonPtr();
	auto material = materialManager->load(postProcessMaterial, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME).staticCast<Ogre::Material>();

	auto pass = material->getTechnique(0)->getPass(0);
	auto psParams = pass->getFragmentProgramParameters();

	const Ogre::Vector3 exposureParams
	{
		1024.0f * expf(exposure - 2.0f),
		7.5f - maxAuto,
		7.5f - minAuto
	};

	psParams->setNamedConstant("exposure", exposureParams);
}

void OgreVRRender::createMainSmgr()
{
	std::ostringstream outstream;
	outstream << "Detected " << numberOfThreads << " hardware threads.";
	logToOgre(outstream.str());

	smgr = root->createSceneManager(Ogre::ST_GENERIC, numberOfThreads > 4 ? 4 : numberOfThreads, Ogre::INSTANCING_CULLING_THREADED, "ANN_MAIN_SMGR");

	logToOgre("Setting the shadow distances to 500m");
	smgr->setShadowDirectionalLightExtrusionDistance(500.0f);
	smgr->setShadowFarDistance(500.0f);
}

void OgreVRRender::setBloomThreshold(float minThreshold, float fullColorThreshold, const char* brightnessPassMaterial)
{
	auto material = Ogre::MaterialManager::getSingleton().load(
		brightnessPassMaterial,
		Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME).
		staticCast<Ogre::Material>();

	auto pass = material->getTechnique(0)->getPass(0);

	auto psParams = pass->getFragmentProgramParameters();
	psParams->setNamedConstant("brightThreshold",
		Ogre::Vector4(
			minThreshold,
			1.0f / (fullColorThreshold - minThreshold),
			0, 0));
}

void OgreVRRender::handleWindowMessages()
{
	Ogre::WindowEventUtilities::messagePump();
	glfwPollEvents();

	static int w, h;
	glfwGetWindowSize(glfwWindow, &w, &h);

	if (windowW != w || windowH != h)
	{
		windowW = w;
		windowH = h;
#ifndef __linux__
		window->windowMovedOrResized();
#else
		window->resize(w, h);
#endif
	}
}

void OgreVRRender::setShadowFiltering(ShadowFiltering level)
{
	if (auto pbs = static_cast<Ogre::HlmsPbs*>
		(root->getHlmsManager()->getHlms(Ogre::HLMS_PBS)))
	{
		switch (level)
		{
		case ShadowFiltering::low:
			pbs->setShadowSettings(Ogre::HlmsPbs::ShadowFilter::PCF_2x2);
			break;
		case ShadowFiltering::medium:
			pbs->setShadowSettings(Ogre::HlmsPbs::ShadowFilter::PCF_3x3);
			break;
		case ShadowFiltering::high:
			pbs->setShadowSettings(Ogre::HlmsPbs::ShadowFilter::PCF_4x4);
			break;
		}
	}
}