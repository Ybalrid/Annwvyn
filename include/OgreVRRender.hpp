#ifndef OGREVRRENDER
#define OGREVRRENDER

#include "systemMacro.h"

#include <string>
#include <array>

#include <Ogre.h>
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
#include "AnnErrorCode.hpp"
#include "AnnHandController.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <algorithm>

constexpr const size_t MAX_CONTROLLER_NUMBER = 2;

///A pose refer to the combination of a position and an orientation. It permit to define the placement of an object with 6DOF
struct OgrePose
{
	///A 3D vector representing a position
	Annwvyn::AnnVect3 position;

	///A quaternion representing an orientation
	Annwvyn::AnnQuaternion orientation;
};

///Abstract class for VR Renderers. Serve as interface between Annwvyn, and "OgreHMDRender" object.
class DLL OgreVRRender
{
public:
	using combinedTextureSizeArray = std::array<std::array<unsigned int, 2>, 2>;

	void logToOgre(const std::string& str);

	///Name of the rendersystem plugin to load on Ogre
#ifndef _DEBUG
	static constexpr const char* const PluginRenderSystemGL3Plus{ "./RenderSystem_GL3Plus" };
#else
	static constexpr const char* const PluginRenderSystemGL3Plus{ "./RenderSystem_GL3Plus_d" };
#endif
	///Name of the rendersystem to initialize
	static constexpr const char* const GLRenderSystem3Plus{ "OpenGL 3+ Rendering Subsystem" };
	///Resource group to load the shaders, material and compositor script for rendering.
	static constexpr const char* const RESOURCE_GROUP_COMPOSITOR = "RG_ANN_COMPOSITOR";

	///number of hardware threads
	const size_t numberOfThreads;

	///Indexes of the compositor workspace
	enum compositorIndex : size_t
	{
		leftEyeCompositor = 0,
		rightEyeCompositor = 1,
		monoCompositor = 2,
		nbCompositor = 3,
	};

	///Liist of the levels of shadow filtering available
	enum class ShadowFiltering
	{
		low,
		medium,
		high
	};

	///Set the shadow filtering level (quality)
	void setShadowFiltering(ShadowFiltering level);

	///Put this to true to use a bigger intermediate buffer instead of a *normal* Anti Aliasing method
	static bool UseSSAA;

	///Set the anti-aliasing level to AA if AA is part of the available values
	/// \param AA the AA level to use.
	static void setAntiAliasingLevel(const uint8_t AA);

	///Type of Debug render you can do
	enum DebugMode
	{
		RAW_BUFFER,
		HMD_MIRROR,
		MONOSCOPIC,
	};

	///Construct VR Renderer
	OgreVRRender(std::string windowName);

	///Destruct VR Renderer
	virtual ~OgreVRRender();

	///Get the scene manager of the virtual world
	Ogre::SceneManager* getSceneManager() const;

	///Get the Ogre::Root object
	Ogre::Root* getRoot() const;

	///Get the RenderWidow that should display a debug render the VR view
	Ogre::RenderWindow* getWindow() const;

	///Get the node that is the player's head anchor point
	Ogre::SceneNode* getCameraInformationNode() const;

	///Get Ogre's internal timer
	Ogre::Timer* getTimer() const;

	///Get frame update time from the VR renderer
	double getUpdateTime() const;

	///Configure the Ogre root engine. Will load all the ogre Plug-ins and components we need.
	void getOgreConfig() const;

	///Init Ogre, please provide the name of the output log file
	void initOgreRoot(std::string loggerName);

	///Init the VR rendering pipeline
	virtual void initPipeline() final;

	///Init the VR client library
	virtual void initVrHmd() = 0;

	///Create the scene(s) manager(s) needed for the rendering
	virtual void initScene() = 0;

	///Create the pair of cameras for the stereo render;
	virtual void initCameras();

	///Initialize the Render To Texture rendering
	virtual void initRttRendering() = 0;

	///Init the VR client rendering
	virtual void initClientHmdRendering() = 0;

	///If true, you should cleanly quit the program from now
	virtual bool shouldQuit() = 0;

	///If true you should use the current position as the new center of tracking
	virtual bool shouldRecenter() = 0;

	///If true, the user can see the VR render inside his HMD
	virtual bool isVisibleInHmd() = 0;

	///Refresh and update the head tracking. May tell the VR client library to prepare for new frame
	virtual void updateTracking() final;

	///Get tracking from the VR system
	virtual void getTrackingPoseAndVRTiming() = 0;

	///Render frame internally inside Ogre, and submit it to the VR client
	virtual void renderAndSubmitFrame() = 0;

	///Put the current position as the center of tracking
	virtual void recenter() = 0;

	///(Optional) Cycle through the client debug display if available.
	virtual void cycleDebugHud() {};

	///Set the distance from the viewpoint to the near clipping distance plane
	void setNearClippingDistance(float distance);

	///Set the distance from the viewpoint to the far clipping distance plane
	void setFarClippingDistance(float distance);

	///Detach camera from their parent node. It seems that Ogre automatically attach new cameras to the root...
	static void detachCameraFromParent(Ogre::Camera* camera);

	///The projection matrix is generally given by the underlying VR api, generally, using the near/far clipping distances set in this class
	/// \note this method is called by the set{Near/Far}ClippingDistance() automatically.
	virtual void updateProjectionMatrix() = 0;

	///(Optional) return true if audio has to come out from a specific audio device
	virtual bool usesCustomAudioDevice() { return false; }

	///(Optional) return the sub string to search on the audio device list to get the correct one
	virtual std::string getAudioDeviceIdentifierSubString() { return ""; }

	///The current position of the head center defined by the client library projected in World Space
	OgrePose trackedHeadPose;

	///She the asked debug view
	virtual void showDebug(DebugMode mode) = 0;

	///Get a naked array of hand controllers
	std::array<std::shared_ptr<Annwvyn::AnnHandController>, MAX_CONTROLLER_NUMBER> getHandControllerArray() const;

	///Get the size of the controller array
	static size_t getHanControllerArraySize();

	///Return true if the hand controllers are available
	bool handControllersAvailable() const;

	///Make sure a string will be usable as a path for the hlms library/compositor resource group
	static void makeValidPath(std::string& hlmsFolder);

	///Called when the IPD needs to be taken into account. Translate the cameras along local X to make them match the position of your eyes
	virtual void handleIPDChange() = 0;

	///Apply the position/orientation of the pose object to the camera rig
	void applyCameraRigPose(OgrePose pose) const;

	///extract from gameplay-movable information the points used to calculate the world poses
	void syncGameplayBody();

	///For call at instant T, will set updateTime to the time between T and T-1
	///If you do this, you'll get the time from between this frame and the last one
	void calculateTimingFromOgre();

	///Load "modern" OpenGL functions for the current OpenGL context.
	static void loadOpenGLFunctions();

	///This method create a texture with the wanted Anti Aliasing level. It will set the rttTextureCombined and rttEyesCombined member of this class to the correct value, and return the GLID of the texture.
	unsigned int createCombinedRenderTexture(unsigned int w, unsigned int h);

	///This create a couple of separated render texture
	std::array<unsigned int, 2> createSeparatedRenderTextures(const combinedTextureSizeArray& textureSizes);

	///Create a render buffer with not anti aliasing. return a tuple with a TexturePtr and a GLID. use <code><pre>std::get<></pre></code>
	std::tuple<Ogre::TexturePtr, unsigned int> createAdditionalRenderBuffer(unsigned int w, unsigned int h, std::string name = "") const;

	///Create a window. Even if we do VR, the way OGRE is architectured, you need to create a window to initialize the RenderSystem
	void createWindow(unsigned int w = 1280, unsigned int h = 720, bool vsync = false);

	///Get the name of this renderer
	std::string getName() const;

	///Load the HLMS library from the file-system
	void loadHLMSLibrary(std::string path = "./hlms/");

	///Load the compositor resources that are needed to setup the pipeline
	void loadCompositor(const std::string& path = "./compositor/", const std::string& type = "FileSystem");

	///Set the color used on the "clear" pass of the compositor node given
	void setSkyColor(Ogre::ColourValue skyColor, float multiplier, const char* renderingNodeName) const;

	///Set the exposure, need the name of the post process material
	void setExposure(float exposure, float minAuto, float maxAuto, const char* postProcessMaterial = "HDR/DownScale03_SumLumEnd") const;

	///Se the bloom threshold. Need the name of the brightness pass material
	void setBloomThreshold(float minThreshold, float fullColorThreshold, const char* brightnessPassMaterial = "HDR/BrightPass_Start");

	///Create the main scene manager and set some shadow parameters
	void createMainSmgr();

	///Return true if you should not react to user inputs, according to the rendering runtime...
	bool shouldPauseFlag();

	///Wrap annoying OpenGL call to something humanly acceptable
	static void glEasyCopy(GLuint source, GLuint dest, GLuint width, GLuint height);

	///Advanced : reset ogre internal timer
	void _resetOgreTimer();

private:

	///GL version to use
	const GLuint glMajor, glMinor;

	///GL FrameWork window
	GLFWwindow* glfwWindow;

	///Shading language to use
	static constexpr const char* const SL{ "GLSL" };

	///Window size
	int windowW, windowH;

protected:

	///Handle the window messages and resize event
	void handleWindowMessages();

	///Compositor workspaces. 0 = left, 1 = right, 2 = monoscopic, plugged to the render window
	std::array<Ogre::CompositorWorkspace*, nbCompositor> compositorWorkspaces;

	///Name of the renderer object
	std::string rendererName;

	///Called if AA level has been updated
	void changedAA() const;

	///Singleton pointer
	static OgreVRRender* self;

	///SceneManager of the VR world
	Ogre::SceneManager* smgr;

	///Ogre root object
	std::unique_ptr<Ogre::Root> root;

	///Render window. VR isn't drawn to this window. A window is mandatory to init the RenderSystem.
	Ogre::RenderWindow* window;

	///Update Time
	double updateTime, then, now;

	///Distance between eyeCamera and nearClippingDistance
	Ogre::Real nearClippingDistance;

	///Distance between eyeCamera and farClippingDistance
	Ogre::Real farClippingDistance;

	///Position of the head
	Ogre::Vector3 feetPosition;

	///Orientation of the head
	Ogre::Quaternion bodyOrientation;

	///Name of the window
	std::string name;

	///Node that represent the head base. Move this in 3D to move the viewpoint
	Ogre::SceneNode* gameplayCharacterRoot;

	///Cameras that have to be put where the user's eye is
	std::array<Ogre::Camera*, 2> eyeCameras;

	///Monoscopic camera
	Ogre::Camera* monoCam;

	///Camera rig, node where all the cameras are attached
	Ogre::SceneNode* cameraRig;

	///Counter of frames
	unsigned long long int frameCounter;

	///Render target that serve as intermediate buffer for the eyeCameras
	Ogre::RenderTexture* rttEyesCombined;

	///Couple of render textures separated
	std::array <Ogre::RenderTexture*, 2> rttEyeSeparated;

	///Level of anti aliasing to use.
	static uint8_t AALevel;

	///Array of hand controller
	std::array<std::shared_ptr<Annwvyn::AnnHandController>, MAX_CONTROLLER_NUMBER> handControllers;

	///Buffered button states
	std::array<std::vector<bool>, 2> currentControllerButtonsPressed, lastControllerButtonsPressed;

	///Dynamically sized containers for pressed/released events
	std::vector<uint8_t> pressed, released;

	///Name given to the texture manager for the eyeBuffer
	static constexpr const char* const rttTextureName = { "RttTex" };

	///Store if we suggest you to pause reacting to user inputs...
	bool pauseFlag;

private:
	///left, right enums
	enum side : uint8_t { left = 0x0, right = 0x1 };
};

#endif