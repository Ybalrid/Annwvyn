/**
 * \file OgreOculusRender.hpp
 * \brief Initialize the Oculus Rendering pipeline, then render the main scene and submit it to Oculus Compositor
 * \author A. Brainville (Ybalrid)
 */

///huge thanks to Germanunkol (aka ueczz on Oculus Forums) https://github.com/Germanunkol/OgreOculusSample
///(even if now I'm not using code from him anymore)
///Shout out to Kojack too for his post of an OgreOculus class a short time after DK1 was out.
///The website http://learnopengl.com/ for improving my (very little at the time) understanding of OpenGL

#ifndef OGRE_OCULUS_RENDERER
#define OGRE_OCULUS_RENDERER

#include "OgreVRRender.hpp"

//Oculus Rift Lib
//#include <OVR.h>
#include <OVR_CAPI_GL.h>

//C++ SDL Includes
#include <iostream>
#include <sstream>
#include <array>

//Accessing Oculus Rift through a class :
#include "OculusInterface.hpp"

//OS Specific build macro
#include "systemMacro.h"
#ifdef _WIN32
#include <Windows.h>
#include <glew.h>
#endif

#include "AnnErrorCode.hpp"
#include "AnnTypes.h"

///Do the initialization and graphical rendering for the Oculus Rift using Ogre
class DLL OgreOculusRender : public OgreVRRender
{
	///OgreOculusRenderEyeType : prefer to use "left" and "right" instead of 0 and 1
	enum oorEyeType
	{
		left,
		right
	};

public:
	///Construct the OgreOculusRender. Can only be called once
	OgreOculusRender(std::string windowName = "OgreOculusRender");

	///Class destructor
	~OgreOculusRender();

	///Oculus runtime hint to quit
	bool shouldQuit() override;

	///Oculus runtime hint to recenter
	bool shouldRecenter() override;

	///App not visible inside Oculus
	bool isVisibleInHmd() override;

	///Cycle through all Oculus Performance HUD available
	void cycleDebugHud() override;

	///Get the timing and tracking state form the oculus runtime and moves the cameras according to the reference and the tracked data
	void updateTracking() override;

	///Render each texture buffer, copy content to the oculus swap texture, get mirror texture data to the OgreMirror texture and update debug window
	void renderAndSubmitFrame() override;

	///Start Oculus and Ogre libraries.
	void initVrHmd() override;

	///Create the RenderWindow
	void createWindow() override;

	///Initialize the SceneManager for the application.
	void initScene() override;

	///Initialize the camera for Stereo Render
	void initCameras() override;

	///Initialize the RTT Rendering System. Create two textures, two viewports and bind cameras on them.
	void initRttRendering() override;

	///Init the Rift rendering. Configure Oculus SDK to use the two RTT textures created.
	void initClientHmdRendering() override;

	///Print various informations about the cameras
	void debugPrint();

	///Recenter rift to default position.
	void recenter() override;

	///Compute from OVR the correct projection matrix for the given clipping distance
	void updateProjectionMatrix() override;

	///change main viewport background color
	void changeViewportBackgroundColor(Ogre::ColourValue color) override;

	///Show in debug window what the camera are seeing
	static void showRawView();

	///Show in the debug window what the Oculus service send as mirrored view
	static void showMirrorView();

	///Show in the debug window a monoscopic view with a default Fov of 90°
	static void showMonscopicView();

	///Get the status of the session
	ovrSessionStatus getSessionStatus();

	///Init the rendering pipeline
	void initPipeline() override;

	///The Oculus Rift has integrated audio
	bool usesCustomAudioDevice() override;

	///The Oculus Rift integrated audio playback device will have "something (Rift Audio)" as a name
	std::string getAudioDeviceIdentifierSubString() override;

	///Show the requested debug view
	void showDebug(DebugMode mode) override;
	void handleIPDChange() override;
private:

	void initializeHandObjects(const OgreOculusRender::oorEyeType side);
	void initializeControllerAxes(const OgreOculusRender::oorEyeType side, std::vector<Annwvyn::AnnHandControllerAxis>& axesVector);
	void ProcessButtonStates(const OgreOculusRender::oorEyeType side);
	void updateTouchControllers();

	///With of the unused pixel-band between the two eyes
	int frontierWidth;

	///Pointer to the renderer itself, re-casted as this class, not the parent
	static OgreOculusRender* OculusSelf;

	///Set the Fov for the monoscopic view
	void setMonoFov(float degreeFov);

	///Save content of the RenderTexture to the specified file. This very slow operation is only useful for debugging the renderer itself
	static void debugSaveToFile(const char path[]);

	///Object for getting informations from the Oculus Rift
	OculusInterface* Oculus;

	///Viewports on textures. Textures are separated. One viewport for each textures
	std::array<Ogre::Viewport*, 2> vpts;

	///Timing in seconds
	double currentFrameDisplayTime, lastFrameDisplayTime;

	///Render descriptor for each eye. Indexes are "left" and "right"
	std::array<ovrEyeRenderDesc, 2> EyeRenderDesc;

	///Size of texture and headset
	ovrSizei bufferSize, hmdSize;

	///Mirror texture
	ovrMirrorTexture mirrorTexture;

	///OpenGL Texture ID of the render buffers
	GLuint oculusMirrorTextureGLID, ogreMirrorTextureGLID, oculusRenderTextureGLID, renderTextureGLID;

	///If true, need to copy the mirrored buffer from Oculus to Ogre
	static bool mirrorHMDView;

	///Compositing layer for the rendered scene
	ovrLayerEyeFov layer;

	///GL texture set for the rendering
	ovrTextureSwapChain textureSwapChain;

	///offset between render center and camera (for IPD variation)
	std::array<ovrVector3f, 2> offset;

	///Pose (position+orientation)
	ovrPosef pose;
	std::array<ovrPoseStatef, 2> handPoses;
	ovrInputState inputState;

	std::array<std::array < int32_t, 4>, 2> touchControllersButtons;

	///Tracking state
	ovrTrackingState ts;

	///Pointer to the layer to be submitted
	ovrLayerHeader* layers;

	///Status of the current session
	ovrSessionStatus sessionStatus;

	///State of the performance HUD
	int perfHudMode;

	///Index of the current texture in the textureSwapChain
	int currentIndex;

	///Frame index of the current session status
	unsigned long long int currentSessionStatusFrameIndex;

	///Convert an ovrVector3f to an Ogre::Vector3
	static Ogre::Vector3 oculusToOgreVect3(const ovrVector3f& v);

	///Convert an ovrQuatf to an Ogre::Quaternion
	static Ogre::Quaternion oculusToOgreQuat(const ovrQuatf& q);

	///Viewport for the debug window
	Ogre::Viewport* debugViewport;

	///Ogre Scene Manager
	Ogre::SceneManager* debugSmgr;

	///Additional camera objects
	Ogre::Camera* debugCam;

	///Nodes for the debug scene
	Ogre::SceneNode* debugCamNode, *debugPlaneNode;

	///Pointer to the debug plane manual material
	Ogre::MaterialPtr DebugPlaneMaterial;

	/*
	* This is a 4 vertices quad with a size of 16x9 units with it's origin in the center
	* The quad got mapped square texture coordinates at each corner, covering the whole UV map
	*
	* The debugPlane is a perfect rectangle drawn by 2 polygons (triangles). The position in object-space are defined as following
	* on the "points" array :
	*  0 +---------------+ 2
	*    |           /   |
	*    |        /      |
	*    |     /         |
	*    |  /            |
	*  1 +----------------+ 3
	* Texture coordinates are also mapped.
	*/

	///Dimensions of the debug plane
	static constexpr const std::array<const float, 2> debugPlaneGeometry{ 16, 9 };

	///Size of the debug plane, divided by 2
	static constexpr const std::array<const float, 2> debugPlanHalfGeometry
	{
		debugPlaneGeometry[0] / 2.f,
		debugPlaneGeometry[1] / 2.f
	};

	///List of the points that form the debug plane (a quad made of two triangles)
	static constexpr const std::array<const std::array<const float, 3>, 4> debugPlaneVertexBuffer
	{
		{
			{-debugPlanHalfGeometry[0], debugPlanHalfGeometry[1], 0},
			{-debugPlanHalfGeometry[0], -debugPlanHalfGeometry[1], 0},
			{debugPlanHalfGeometry[0], debugPlanHalfGeometry[1], 0},
			{debugPlanHalfGeometry[0], -debugPlanHalfGeometry[1], 0},
		}
	};

	///Texture coordinates to map a whole texture to the debug plane
	static constexpr const std::array<const std::array< const float, 2>, 4> debugPlaneTextureCoord
	{
		{
			{0, 0},
			{0, 1},
			{1, 0},
			{1, 1}
		}
	};

	///Index buffer of the debug plane
	static constexpr const std::array<const uint8_t, 4>debugPlaneIndexBuffer{ 0, 1, 2, 3 };
	static constexpr const std::array<const uint8_t, 4>quadIndexBuffer{ 0, 1, 2, 3 };

	static constexpr const std::array<const oorEyeType, 2> eyeUpdateOrder{ {left, right} };

	///Return true if the array sizes of each buffer are constants
	static constexpr bool debugPlaneSanityCheck()
	{
		return debugPlaneIndexBuffer.size() == debugPlaneVertexBuffer.size()
			&& debugPlaneIndexBuffer.size() == debugPlaneTextureCoord.size();
	}

public:
	///Position of the rift at the last frame
	Ogre::Vector3 lastOculusPosition;

	///Orientation of the rift at the last frame
	Ogre::Quaternion lastOculusOrientation;

	///Texture unit state of the debug plane
	static Ogre::TextureUnitState* debugTexturePlane;
};
#endif //OGRE_OCULUS_RENDERER