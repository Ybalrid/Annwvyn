// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <OgreSceneNode.h>
#include <OgreCamera.h>
#include <Compositor/OgreCompositorManager2.h>
#include <Compositor/OgreCompositorWorkspaceDef.h>
#include <Compositor/OgreCompositorWorkspace.h>
#include <Compositor/OgreCompositorNode.h>
#include <Compositor/OgreCompositorNodeDef.h>
#include <Compositor/Pass/PassClear/OgreCompositorPassClearDef.h>
#include <OgreMaterialManager.h>
#include <OgreMaterial.h>
#include <OgreTechnique.h>
#include <OgrePass.h>
#include <Hlms/Pbs/OgreHlmsPbs.h>
#include <Hlms/Unlit/OgreHlmsUnlit.h>
#include <OgreHlmsManager.h>
#include <OgreHlms.h>

#include "AnnOgreVRRenderer.hpp"
#include "AnnGetter.hpp"
#include "AnnLogger.hpp"
#include "Annwvyn.h"

#ifdef __linux__
#include <X11/Xlib.h>
#define GLFW_EXPOSE_NATIVE_X11
#endif

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NAVIVE_WGL
#endif

#include <GLFW/glfw3native.h>

//Windows multimedia and sound libraries
#ifdef _WIN32
#include <mmsystem.h>
#include <dsound.h>
#endif
using namespace Annwvyn;

uint8_t AnnOgreVRRenderer::AALevel { 4 };
AnnOgreVRRenderer* AnnOgreVRRenderer::self { nullptr };

void AnnOgreVRRenderer::setAntiAliasingLevel(const uint8_t AA)
{
	AALevel = AA;
	if(self)
		return self->changedAA();
}

AnnOgreVRRenderer::AnnOgreVRRenderer(const std::string& windowName) :
 numberOfThreads(std::thread::hardware_concurrency()),
 glMajor { 4 },
 glMinor { 3 },
 glfwWindow { nullptr },
 windowW { 0 },
 windowH { 0 },
 smgr { nullptr },
 root { nullptr },
 window { nullptr },
 updateTime { 0 },
 then { 0 },
 now { 0 },
 nearClippingDistance { 0.01f },
 farClippingDistance { 500.0f },
 feetPosition { 0, 0, 10 },
 bodyOrientation { Ogre::Quaternion::IDENTITY },
 name { windowName },
 gameplayCharacterRoot { nullptr },
 monoCam(nullptr),
 cameraRig { nullptr },
 frameCounter { 0 },
 rttEyesCombined { nullptr },
 pauseFlag { false },
 hideHands { false },
 compositorLoaded { false },
 hlmsLoaded { false }
{
	if(self)
	{
		displayWin32ErrorMessage("Fatal Error", "Fatal error with renderer initialization. OgreOculusRender object already created.");
		throw AnnInitializationError(ANN_ERR_CRITIC, "Cannot create more than one OgreVRRenderer object!");
	}
	self = this;

	for(auto& eyeCamera : eyeCameras)
		eyeCamera = nullptr;
	for(auto& handController : handControllers)
		handController = nullptr;
	for(auto& compositor : compositorWorkspaces)
		compositor = nullptr;
}

AnnOgreVRRenderer::~AnnOgreVRRenderer()
{
	Ogre::LogManager::getSingleton().logMessage("AnnOgreVRRenderer::~AnnOgreVRRenderer()");

	//For good measure : destroy the Ogre window
	root->destroyRenderTarget(window);

	//If font manager still loaded
	if(auto fontManager = Ogre::FontManager::getSingletonPtr())
	{
		AnnDebug() << "Will delete FontManager";
		fontManager->removeAll();
		OGRE_DELETE fontManager;
	}

	//We need to stop OGRE before destroying the GLFW window.
	root.reset();

	//Clean GLFW
	glfwDestroyWindow(glfwWindow);
	glfwTerminate();

	//Reset the singleton
	self = nullptr;
}

Ogre::SceneManager* AnnOgreVRRenderer::getSceneManager() const
{
	return smgr;
}

Ogre::Root* AnnOgreVRRenderer::getRoot() const
{
	return root.get();
}

Ogre::RenderWindow* AnnOgreVRRenderer::getWindow() const
{
	return window;
}

Ogre::SceneNode* AnnOgreVRRenderer::getCameraInformationNode() const
{
	return gameplayCharacterRoot;
}

Ogre::Timer* AnnOgreVRRenderer::getTimer() const
{
	return root->getTimer();
}

double AnnOgreVRRenderer::getUpdateTime() const
{
	return updateTime;
}

void AnnOgreVRRenderer::initOgreRoot(const std::string& loggerName)
{
	//Create the ogre root with standards Ogre configuration file
	root = std::make_unique<Ogre::Root>("", "ogre.cfg", loggerName.c_str());

	//Set the log verbosity to "bore me"
	Ogre::LogManager::getSingleton().setLogDetail(Ogre::LoggingLevel::LL_BOREME);
}

void AnnOgreVRRenderer::getOgreConfig() const
{
	//Ogre as to be initialized
	if(!root) throw AnnInitializationError(ANN_ERR_NOTINIT, "Need to initialize Ogre::Root before loading system configuration");

	//Load OgrePlugins
	root->loadPlugin(PluginRenderSystemGL3Plus);

	root->loadPlugin(Ogre_glTF_Plugin);

	//Set the classic OpenGL render system
	root->setRenderSystem(root->getRenderSystemByName(GLRenderSystem3Plus));
	//	root->getRenderSystem()->setConfigOption("FSAA", std::to_string(AALevel));
	root->getRenderSystem()->setConfigOption("sRGB Gamma Conversion", "Yes");
	root->initialise(false);
}

std::array<std::shared_ptr<AnnHandController>, MAX_CONTROLLER_NUMBER> AnnOgreVRRenderer::getHandControllerArray() const
{
	return handControllers;
}

size_t AnnOgreVRRenderer::getHanControllerArraySize()
{
	return MAX_CONTROLLER_NUMBER;
}

void AnnOgreVRRenderer::changedAA() const
{
	root->getRenderSystem()->setConfigOption("FSAA", std::to_string(AALevel));
	window->setFSAA(AALevel, "");
	auto textureManager = Ogre::TextureManager::getSingletonPtr();
	auto texture		= textureManager->getByName(rttTextureName);
	if(texture)
	{
		texture->setFSAA(AALevel, "", false);
	}
	else
	{
		for(auto i { 0 }; i < 2; i++)
		{
			texture = textureManager->getByName(rttTextureName + std::to_string(i));
			if(texture) texture->setFSAA(AALevel, "", false);
		}
	}
}

void AnnOgreVRRenderer::setNearClippingDistance(float distance)
{
	nearClippingDistance = distance;
	updateEyeCameraFrustrum();
}

void AnnOgreVRRenderer::setFarClippingDistance(float distance)
{
	farClippingDistance = distance;
	updateEyeCameraFrustrum();
}

void AnnOgreVRRenderer::detachCameraFromParent(Ogre::Camera* camera)
{
	if(auto parent = camera->getParentSceneNode())
		parent->detachObject(camera);
}

void AnnOgreVRRenderer::initCameras()
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

void AnnOgreVRRenderer::applyCameraRigPose(AnnPose pose) const
{
	cameraRig->setPosition(static_cast<Ogre::Vector3>(pose.position));
	cameraRig->setOrientation(static_cast<Ogre::Quaternion>(pose.orientation));
}

void AnnOgreVRRenderer::syncGameplayBody()
{
	//Get current camera base information
	feetPosition	= gameplayCharacterRoot->getPosition();
	bodyOrientation = gameplayCharacterRoot->getOrientation();
}

void AnnOgreVRRenderer::calculateTimingFromOgre()
{
	then	   = now;
	now		   = getTimer()->getMilliseconds() / 1000.0;
	updateTime = now - then;
}

void AnnOgreVRRenderer::loadOpenGLFunctions()
{
	AnnDebug() << "Init GL Extension Wrangler";
	//The version of OpenGL feature set that will be "wrangled" by GLEW depend of the current OpenGL context.
	//The context version depend on who created the context. Here we used GLFW and hinted for OpenGL 4.3
	const auto err = glewInit();
	if(err != GLEW_OK)
	{
		AnnDebug(Log::Important) << "Failed to glewTnit(), error : " << glewGetString(err);
		throw AnnInitializationError(ANN_ERR_RENDER, "Cannot load OpenGL functions");
	}

	//Just print some informations about GLEW
	AnnDebug() << "Using GLEW version : "
			   << glewGetString(GLEW_VERSION);

	//Check if the function pointer behind glCopyImageSubData is not null
	if(glCopyImageSubData)
	{
		AnnDebug() << "glCopyImageSubData is available!";
	}
}

void AnnOgreVRRenderer::updateTracking()
{
	syncGameplayBody();
	getTrackingPoseAndVRTiming();
	applyCameraRigPose(trackedHeadPose);
}

void AnnOgreVRRenderer::initPipeline()
{
	getOgreConfig();
	rendererName.find("NoVR") != std::string::npos ? createWindow(1280, 720, true) : createWindow();
	initScene();
	initCameras();
	loadCompositor();
	initRttRendering();
	updateEyeCameraFrustrum();
	loadHLMSLibrary();
}

GLuint AnnOgreVRRenderer::createCombinedRenderTexture(unsigned int w, unsigned int h)
{
	GLuint glid;
	auto rttTextureCombined = Ogre::TextureManager::getSingleton().createManual(rttTextureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, w, h, 0, Ogre::PF_R8G8B8A8, Ogre::TU_RENDERTARGET, nullptr, true, AALevel);
	rttTextureCombined->getCustomAttribute("GLID", &glid);
	rttEyesCombined = rttTextureCombined->getBuffer()->getRenderTarget();
	return glid;
}

GLuintPair AnnOgreVRRenderer::createSeparatedRenderTextures(const combinedTextureSizeArray& textureSizes)
{
	GLuintPair glid = { { 0 } };
	AnnDebug() << "Creating separated render textures " << textureSizes[0][0] << "x" << textureSizes[0][1] << " " << textureSizes[1][0] << "x" << textureSizes[1][1];
	std::array<Ogre::TexturePtr, 2> rttTexturesSeparated;
	for(auto i : { 0u, 1u })
	{
		rttTexturesSeparated[i] = Ogre::TextureManager::getSingleton().createManual(std::string(rttTextureName) + std::to_string(i),
																					Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
																					Ogre::TEX_TYPE_2D,
																					Ogre::uint(textureSizes[i][0]),
																					Ogre::uint(textureSizes[i][1]),
																					0,
																					Ogre::PF_R8G8B8A8,
																					Ogre::TU_RENDERTARGET,
																					nullptr,
																					true,
																					AALevel);
		rttTexturesSeparated[i]->getCustomAttribute("GLID", &glid[i]);
		rttEyeSeparated[i] = rttTexturesSeparated[i]->getBuffer(0)->getRenderTarget(0);
	}
	return glid;
}

std::tuple<Ogre::TexturePtr, unsigned int> AnnOgreVRRenderer::createAdditionalRenderBuffer(unsigned int w, unsigned int h, std::string additionalTextureName) const
{
	static int counter;
	if(additionalTextureName.empty())
		additionalTextureName = "additionalTexture" + std::to_string(counter++);

	auto texture { Ogre::TextureManager::getSingleton().createManual(additionalTextureName,
																	 Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
																	 Ogre::TEX_TYPE_2D,
																	 w,
																	 h,
																	 9,
																	 Ogre::PF_R8G8B8A8) };

	unsigned int glid;
	texture->getCustomAttribute("GLID", &glid);
	return std::tie(texture, glid);
}

void AnnOgreVRRenderer::createWindow(unsigned int w, unsigned int h, bool vsync)
{
	windowW = w, windowH = h;
	auto winName = rendererName + " : " + name + " - monitor output";

	AnnDebug() << "Initializing GLFW";
	AnnDebug() << "GLFW version: " << GLFW_VERSION_MAJOR << "." << GLFW_VERSION_MINOR << "." << GLFW_VERSION_REVISION;
	glfwInit();

	//Specify OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, AALevel);

	AnnDebug() << "Set OpenGL context version " << glMajor << "." << glMinor;

	//Create a window (and an opengl context with it)
	glfwWindow = glfwCreateWindow(w, h, winName.c_str(), nullptr, nullptr);

	//Make the created context current
	glfwMakeContextCurrent(glfwWindow);
	Ogre::NameValuePairList options;

#ifdef _WIN32
	//Get the hwnd and the context :
	auto context					= wglGetCurrentContext();
	auto handle						= glfwGetWin32Window(glfwWindow);
	options["externalWindowHandle"] = std::to_string(size_t(handle));
#elif __linux__
	Window handle				  = {};
	void* context				  = nullptr;
	handle						  = glfwGetX11Window(glfwWindow);
	options["parentWindowHandle"] = std::to_string(size_t(handle));
#endif

	options["externalGLContext"] = std::to_string(size_t(context));
	options["FSAA"]				 = std::to_string(AALevel);
	options["top"]				 = "0";
	options["left"]				 = "0";
	options["gamma"]			 = "true";
	options["vsync"]			 = vsync ? "true" : "false";

	window = root->createRenderWindow(winName, w, h, false, &options);
}

std::string AnnOgreVRRenderer::getName() const
{
	return rendererName;
}

bool AnnOgreVRRenderer::handControllersAvailable() const
{
	return handControllers[left].get() && handControllers[right].get();
}

void AnnOgreVRRenderer::makeValidPath(std::string& path)
{
	if(path.empty())
		path = "./";
	else if(path[path.size() - 1] != '/')
		path += "/";
}

void AnnOgreVRRenderer::loadHLMSLibrary(std::string hlmsFolder)
{
	//The hlmsFolder can come from a configuration file where it could be "empty" or set to "." or lacking the trailing "/"
	makeValidPath(hlmsFolder);

	//Get the hlmsManager (not a singleton by itself, but accessible via Root)
	auto hlmsManager = root->getHlmsManager();

	//Define the shader library to use for HLMS
	//For retrieval of the paths to the different folders needed
	Ogre::String dataFolderPath;
	Ogre::StringVector libraryFoldersPaths;

	//Get the path to all the subdirectories used by HlmsUnlit
	Ogre::HlmsUnlit::getDefaultPaths(dataFolderPath, libraryFoldersPaths);

	//Create the Ogre::Archive objects needed
	auto archiveUnlit = Ogre::ArchiveManager::getSingletonPtr()->load(hlmsFolder + dataFolderPath, "FileSystem", true);
	Ogre::ArchiveVec archiveUnlitLibraryFolders;
	for(const auto& libraryFolderPath : libraryFoldersPaths)
	{
		auto archiveLibrary = Ogre::ArchiveManager::getSingletonPtr()->load(hlmsFolder + libraryFolderPath, "FileSystem", true);
		archiveUnlitLibraryFolders.push_back(archiveLibrary);
	}

	//Create and register the unlit Hlms
	auto hlmsUnlit = OGRE_NEW Ogre::HlmsUnlit(archiveUnlit, &archiveUnlitLibraryFolders);
	hlmsUnlit->setDebugOutputPath(false, false);
	hlmsManager->registerHlms(hlmsUnlit);

	//Do the same for HlmsPbs:
	Ogre::HlmsPbs::getDefaultPaths(dataFolderPath, libraryFoldersPaths);
	auto archivePbs = Ogre::ArchiveManager::getSingletonPtr()->load(hlmsFolder + dataFolderPath, "FileSystem", true);

	//Get the library archive(s)
	Ogre::ArchiveVec archivePbsLibraryFolders;
	for(const auto& libraryFolderPath : libraryFoldersPaths)
	{
		auto archiveLibrary = Ogre::ArchiveManager::getSingletonPtr()->load(hlmsFolder + libraryFolderPath, "FileSystem", true);
		archivePbsLibraryFolders.push_back(archiveLibrary);
	}

	//Create and register
	auto hlmsPbs = OGRE_NEW Ogre::HlmsPbs(archivePbs, &archivePbsLibraryFolders);
	hlmsManager->registerHlms(hlmsPbs);

	//Set the best shadows we can do by default
	hlmsPbs->setShadowSettings(Ogre::HlmsPbs::ShadowFilter::PCF_4x4);
	hlmsPbs->setDebugOutputPath(false, false);

	hlmsLoaded = true;
}

void AnnOgreVRRenderer::loadCompositor(const std::string& path, const std::string& type)
{
	auto compositorFolder = path;
	makeValidPath(compositorFolder);
	auto resourceGroupManager = Ogre::ResourceGroupManager::getSingletonPtr();
	//TODO ISSUE maybe package the compositor differently
	resourceGroupManager->addResourceLocation(compositorFolder, type, RESOURCE_GROUP_COMPOSITOR);
	resourceGroupManager->initialiseResourceGroup(RESOURCE_GROUP_COMPOSITOR, false);

	compositorLoaded = true;
}

void AnnOgreVRRenderer::setSkyColor(Ogre::ColourValue skyColor, float multiplier, const char* renderingNodeName) const
{
	auto compositor		  = root->getCompositorManager2();
	auto renderingNodeDef = compositor->getNodeDefinitionNonConst(renderingNodeName);
	auto targetDef		  = renderingNodeDef->getTargetPass(0);
	auto& passDefs		  = targetDef->getCompositorPasses();
	for(auto pass : passDefs)
		if(pass->getType() == Ogre::PASS_CLEAR)
		{
			auto clearDef = dynamic_cast<Ogre::CompositorPassClearDef*>(pass);
			if(clearDef)
			{
				clearDef->mColourValue = skyColor * multiplier;
			}
		}
}

void AnnOgreVRRenderer::setExposure(float exposure, float minAuto, float maxAuto, const char* postProcessMaterial) const
{
	auto materialManager = Ogre::MaterialManager::getSingletonPtr();
	auto material		 = materialManager->load(postProcessMaterial, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME).staticCast<Ogre::Material>();

	auto pass	 = material->getTechnique(0)->getPass(0);
	auto psParams = pass->getFragmentProgramParameters();

	const Ogre::Vector3 exposureParams {
		1024.0f * expf(exposure - 2.0f),
		7.5f - maxAuto,
		7.5f - minAuto
	};

	psParams->setNamedConstant("exposure", exposureParams);
}

void AnnOgreVRRenderer::createMainSmgr()
{
	AnnDebug() << "Detected " << numberOfThreads << " hardware threads.";

	smgr = root->createSceneManager(Ogre::ST_GENERIC, numberOfThreads > 4 ? 4 : numberOfThreads, Ogre::INSTANCING_CULLING_THREADED, "ANN_MAIN_SMGR");

	AnnDebug() << "Setting the shadow distances to 500m";
	smgr->setShadowDirectionalLightExtrusionDistance(500.0f);
	smgr->setShadowFarDistance(500.0f);
}

bool AnnOgreVRRenderer::shouldPauseFlag() const
{
	return pauseFlag;
}

bool AnnOgreVRRenderer::shouldHideHands() const
{
	return hideHands;
}

///Wrap annoying OpenGL call to something humanly acceptable
void AnnOgreVRRenderer::glEasyCopy(GLuint source, GLuint dest, GLuint width, GLuint height)
{
	glCopyImageSubData(source,
					   GL_TEXTURE_2D,
					   0,
					   0,
					   0,
					   0,
					   dest,
					   GL_TEXTURE_2D,
					   0,
					   0,
					   0,
					   0,
					   width,
					   height,
					   1);
}

void AnnOgreVRRenderer::_resetOgreTimer() const
{
	root->getTimer()->reset();
}

bool AnnOgreVRRenderer::isCompositorLoaded() const
{
	return compositorLoaded;
}

bool AnnOgreVRRenderer::isHlmsLibLoaded() const
{
	return hlmsLoaded;
}

std::string AnnOgreVRRenderer::getAudioDeviceNameFromGUID(GUID guid)
{
#ifdef _WIN32

	//Container to associate audio device names with their GUID
	struct AudioOutputDescriptor
	{
		AudioOutputDescriptor(LPCSTR str, LPGUID pguid) :
		 name(pguid ? str : "NO_GUID"), //Sometimes pguid is nullptr, in that case the descriptor is not valid
		 guid(pguid ? *pguid : GUID())
		{} //Obviously, dereferencing a nullptr is a bad idea, prevent that.
		const std::string name;
		const GUID guid;
	};

	//Create a vector of the "audio output descriptor" declared above
	using AudioOutputDescriptorVect = std::vector<AudioOutputDescriptor>;
	AudioOutputDescriptorVect descriptors;

	//Fill the "descriptors" vector :

	DirectSoundEnumerateA([](LPGUID pguid, LPCSTR descr, LPCSTR modname, LPVOID ctx) //using a non capturing lambda function as a callback
						  {
							  (void)modname;
							  auto outputDescriptors = static_cast<AudioOutputDescriptorVect*>(ctx); //get an usable pointer
							  outputDescriptors->emplace_back(descr, pguid);						 //create a usable descriptor and add it to the list
							  return TRUE;
						  },
						  &descriptors); //Pointer to the vector as "context" given to the callback

	//Try to find the one we need
	const auto result = std::find_if(begin(descriptors),
									 end(descriptors),
									 [&](const AudioOutputDescriptor& descriptor) {
										 return descriptor.guid == guid;
									 });

	//Set the return string
	if(result != descriptors.end())
	{
		const auto descriptor = *result;
		AnnDebug() << "Found " << descriptor.name << " that match Oculus given DirectSound GUID";
		return descriptor.name;
	}
	return "use windows default";
#else
	return "not valid for non-windows platform";
#endif
}

void AnnOgreVRRenderer::setBloomThreshold(float minThreshold, float fullColorThreshold, const char* brightnessPassMaterial)
{
	auto material = Ogre::MaterialManager::getSingleton().load(
															 brightnessPassMaterial,
															 Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME)
						.staticCast<Ogre::Material>();

	auto pass = material->getTechnique(0)->getPass(0);

	auto psParams = pass->getFragmentProgramParameters();
	psParams->setNamedConstant("brightThreshold",
							   Ogre::Vector4(
								   minThreshold,
								   1.0f / (fullColorThreshold - minThreshold),
								   0,
								   0));
}

void AnnOgreVRRenderer::doStereoRender()
{
	static auto deactivateWorkspaces = [&] { for(auto& w : compositorWorkspaces) w->setEnabled(false); };

	for(size_t i = 0; i < 3; ++i)
	{
		deactivateWorkspaces();
		compositorWorkspaces[i]->setEnabled(true);
		root->renderOneFrame();
	}
}

void AnnOgreVRRenderer::handleWindowMessages()
{
	//Do the message pumping from the OS
	Ogre::WindowEventUtilities::messagePump();
	glfwPollEvents();

	//handle resizable window
	static int w, h;
	glfwGetWindowSize(glfwWindow, &w, &h);

	if(windowW != w || windowH != h)
	{
		windowW = w;
		windowH = h;
		//For some reason, windowMovedOrResized() is bugging on linux and doesn't do anything
#ifndef __linux__
		window->windowMovedOrResized();
#else
		window->resize(w, h);
#endif
	}
}

void AnnOgreVRRenderer::setShadowFiltering(ShadowFiltering level) const
{
	if(auto pbs = static_cast<Ogre::HlmsPbs*>(root->getHlmsManager()->getHlms(Ogre::HLMS_PBS)))
	{
		switch(level)
		{
			case ShadowFiltering::low:
				pbs->setShadowSettings(Ogre::HlmsPbs::ShadowFilter::PCF_2x2);
				break;
			default:
			case ShadowFiltering::medium:
				pbs->setShadowSettings(Ogre::HlmsPbs::ShadowFilter::PCF_3x3);
				break;
			case ShadowFiltering::high:
				pbs->setShadowSettings(Ogre::HlmsPbs::ShadowFilter::PCF_4x4);
				break;
		}
	}
}
