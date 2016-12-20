#ifndef OGREVRRENDER
#define OGREVRRENDER

#include "systemMacro.h"

#include <string>
#include <array>

#include <Ogre.h>
#include "AnnErrorCode.hpp"
#include "AnnHandController.hpp"

constexpr const size_t MAX_CONTROLLER_NUMBER = 2;

///A pose refer to the combination of a position and an orientation. It permit to define the placement of an object with 6DOF
struct OgrePose
{
	///A 3D vector representing a position
	Ogre::Vector3 position;

	///A quaternion representing an orientation
	Ogre::Quaternion orientation;
};

///Abstract class for VR Renderers. Serve as interface between Annwvyn, and "OgreHMDRender" object.
class DLL OgreVRRender
{
public:
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
	Ogre::SceneManager* getSceneManager();

	///Get the Ogre::Root object
	Ogre::Root* getRoot();

	///Get the RenderWidow that should display a debug render the VR view
	Ogre::RenderWindow* getWindow();

	///Get the node that is the player's head anchor point
	Ogre::SceneNode* getCameraInformationNode();

	///Get Ogre's internal timer
	Ogre::Timer* getTimer();

	///Get frame update time from the VR renderer
	double getUpdateTime();

	///Configure the Ogre root engine. Will load all the ogre Plug-ins and components we need.
	void getOgreConfig();

	///Init Ogre, please provide the name of the output log file
	void initOgreRoot(std::string loggerName);

	///Init the VR rendering pipeline
	virtual void initPipeline() = 0;

	///Init the VR client library
	virtual void initVrHmd() = 0;

	///Create the render window
	virtual void createWindow() = 0;

	///Create the scene(s) manager(s) needed for the rendering
	virtual void initScene() = 0;

	///Create the pair of cameras for the stereo render;
	virtual void initCameras() = 0;

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
	virtual void updateTracking() = 0;

	///Render frame internally inside Ogre, and submit it to the VR client
	virtual void renderAndSubmitFrame() = 0;

	///Put the current position as the center of tracking
	virtual void recenter() = 0;

	///Change the color of the pixels when there is *nothing*
	/// \param color Color to use as background
	virtual void changeViewportBackgroundColor(Ogre::ColourValue color) = 0;

	///(Optional) Cycle through the client debug display if available.
	virtual void cycleDebugHud() {};

	///Set the distance from the viewpoint to the near clipping distance plane
	void setNearClippingDistance(float distance);

	///Set the distance from the viewpoint to the far clipping distance plane
	void setFarClippingDistance(float distance);

	///The projection matrix is generally given by the underlying VR api, generally, using the near/far clipping distances set in this class
	/// \note this method is called by the set{Near/Far}ClippingDistance() automatically.
	virtual void updateProjectionMatrix() = 0;

	///(Optional) return true if audio has to come out from a specific audio device
	virtual bool usesCustomAudioDevice() { return false; }

	///(Optional) return the sub string to search on the audio device list to get the correct one
	virtual std::string getAudioDeviceIdentifierSubString() { return ""; }

	///The current position of the head center defined by the client library projected in World Space
	OgrePose returnPose;

	///She the asked debug view
	virtual void showDebug(DebugMode mode) = 0;

	///Get a naked array of hand controllers
	std::array<std::shared_ptr<Annwvyn::AnnHandController>, MAX_CONTROLLER_NUMBER> getHandControllerArray();

	///Get the size of the controller array
	static size_t getHanControllerArraySize();

	///Return the number of non nullptr handControllers. Hand controllers are dynamically allocated by the VRRenderer if presents.
	size_t getRecognizedControllerCount();

protected:

	///Called if AA level has been updated
	void changedAA();

	///Singleton pointer
	static OgreVRRender* self;

	///SceneManager of the VR world
	Ogre::SceneManager* smgr;

	///Ogre root object
	Ogre::Root* root;

	///Render window. VR isn't drawn to this window. A window is mandatory to init the RenderSystem.
	Ogre::RenderWindow* window;

	///Update Time
	double updateTime;

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
	Ogre::SceneNode* headNode;
	Ogre::SceneNode* roomNode;

	///background color of viewports
	Ogre::ColourValue backgroundColor;

	///Cameras that have to be put where the user's eye is
	std::array<Ogre::Camera*, 2> eyeCameras;

	///Counter of frames
	unsigned long long int frameCounter;

	///Render target that serve as intermediate buffer for the eyeCameras
	Ogre::RenderTexture* rttEyes;

	///The texture that is used to hold the render target
	Ogre::TexturePtr rttTexture;

	///Level of anti aliasing to use.
	static uint8_t AALevel;

	///Array of hand controller
	std::array<std::shared_ptr<Annwvyn::AnnHandController>, MAX_CONTROLLER_NUMBER> handControllers;

	///Buffered button states
	std::array<std::vector<bool>, 2> currentControllerButtonsPressed, lastControllerButtonsPressed;

	///Dynamically sized containers for pressed/released events
	std::vector<uint8_t> pressed, released;

	///List of acceptable Anti Aliasing factors for the render buffer and window
	static constexpr std::array<const uint8_t, 5> AvailableAALevel{ 0, 2, 4, 8, 16 };

	///Name given to the texture manager for the eyeBuffer
	static constexpr const char* const rttTextureName = { "RttTex" };
};

#endif