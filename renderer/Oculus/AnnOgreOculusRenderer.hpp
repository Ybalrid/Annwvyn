/**
 * \file AnnOgreOculusRenderer.hpp
 * \brief Initialize the Oculus Rendering pipeline, then render the main scene and submit it to Oculus Compositor
 * \author A. Brainville (Ybalrid)
 */

///huge thanks to Germanunkol (aka ueczz on Oculus Forums) https://github.com/Germanunkol/OgreOculusSample
///(even if now I'm not using code from him anymore)
///Shout out to Kojack too for his post of an OgreOculus class a short time after DK1 was out.
///The website http://learnopengl.com/ for improving my (very little at the time) understanding of OpenGL

#pragma once

#ifdef _WIN32

#include "AnnOgreVRRenderer.hpp"

//Oculus Rift Lib
#include <OVR_CAPI_GL.h>
#include <OVR_CAPI_Audio.h>

//C++ SDL Includes
#include <iostream>
#include <sstream>
#include <array>

//Accessing Oculus Rift through a class :
#include "AnnOculusInterfaceHelper.hpp"

//OS Specific build macro
#include "systemMacro.h"
#ifdef _WIN32
#include <Windows.h>
#endif

#include <AnnHandController.hpp>

namespace Annwvyn
{

	///Do the initialization and graphical rendering for the Oculus Rift using Ogre
	class AnnDllExport AnnOgreOculusRenderer : public AnnOgreVRRenderer
	{
		///OgreOculusRenderEyeType : prefer to use "left" and "right" instead of 0 and 1
		enum OgreOculusEyeType {
			left,
			right
		};

	public:
		///Construct the AnnOgreOculusRenderer. Can only be called once
		AnnOgreOculusRenderer(std::string windowName = "AnnOgreOculusRenderer");

		///Class destructor
		~AnnOgreOculusRenderer();

		///Oculus runtime hint to quit
		bool shouldQuit() override;

		///Oculus runtime hint to recenter
		bool shouldRecenter() override;

		///App not visible inside Oculus
		bool isVisibleInHmd() override;

		///Cycle through all Oculus Performance HUD available
		void cycleDebugHud() override;

		///Get tracking and timing from the oculus runtime
		void getTrackingPoseAndVRTiming() override;

		///Render each texture buffer, copy content to the oculus swap texture, get mirror texture data to the OgreMirror texture and update debug window
		void renderAndSubmitFrame() override;

		///Start Oculus and Ogre libraries.
		void initVrHmd() override;

		///Initialize the SceneManager for the application.
		void initScene() override;

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

		///Show in debug window what the camera are seeing
		static void showRawView();

		///Show in the debug window what the Oculus service send as mirrored view
		static void showMirrorView();

		///Show in the debug window a monoscopic view with a default Fov of 90°
		static void showMonscopicView();

		///Get the status of the session
		ovrSessionStatus getSessionStatus();

		///The Oculus Rift has integrated audio
		bool usesCustomAudioDevice() override;

		///The Oculus Rift integrated audio playback device will have "something (Rift Audio)" as a name
		std::string getAudioDeviceIdentifierSubString() override;

		///Show the requested debug view
		void showDebug(DebugMode mode) override;

		///Move the cameras according form the hmdToEye translation vector from OVR.
		void handleIPDChange() override;

		///Print to the log the relative pose of each rendering frustum (with center of headset as origin)
		void logHeadsetGeometry();

	private:
		///Render textures sizes
		ovrSizei texSizeL, texSizeR;

		///Create the AnnHandControllerObject for this side
		void initializeHandObjects(const OgreOculusEyeType side);

		///Initialize the axisVector for given controller
		void initializeControllerAxes(const OgreOculusEyeType side, std::vector<Annwvyn::AnnHandControllerAxis>& axesVector);

		///Extract usefull data from the button state, including buffered pressed/released events
		void processButtonStates(const OgreOculusEyeType side);

		///Get the state of the touch controller and update the handController objects accordingly
		void updateTouchControllers();

		///Pointer to the renderer itself, re-casted as this class, not the parent
		static AnnOgreOculusRenderer* oculusSelf;

		///Set the Fov for the monoscopic view
		void setMonoFov(float degreeFov) const;

		///Object for getting informations from the Oculus Rift
		std::unique_ptr<AnnOculusInterfaceHelper> oculusInterface;

		///Timing in seconds
		double currentFrameDisplayTime;

		///Render descriptor for each eye. Indexes are "left" and "right"
		std::array<ovrEyeRenderDesc, 2> eyeRenderDescArray;

		///Size of texture and headset
		ovrSizei hmdSize;

		///Mirror texture
		ovrMirrorTexture mirrorTexture;

		///OpenGL Texture ID for the Oculus provided mirror texture
		GLuint oculusMirrorTextureGLID;

		///OpenGL Texture ID for the Ogre provided mirror texture
		GLuint ogreMirrorTextureGLID;

		///OpenGL Texture ID for the Oculus provided render texture
		GLuint oculusRenderTextureCombinedGLID;

		///OpenGL Texture ID for the Ogre
		GLuint ogreRenderTextureCombinedGLID;

		///Array of 2 OpenGL texutre indices
		std::array<GLuint, 2> oculusRenderTexturesSeparatedGLID, ogreRenderTexturesSeparatedGLID;

		///If true, need to copy the mirrored buffer from Oculus to Ogre
		static bool mirrorHMDView;

		///Compositing layer for the rendered scene
		ovrLayerEyeFov layer;

		///GL texture set for the rendering
		ovrTextureSwapChain textureCombinedSwapChain;

		///Array of 2 ovrTextureSwapChain
		std::array<ovrTextureSwapChain, 2> texturesSeparatedSwapChain;

		///eyeToHmdPoseOffset between render center and camera (for IPD variation)
		std::array<ovrPosef, 2> eyeToHmdPoseOffset;

		///Pose (position+orientation)
		ovrPosef pose;

		///Pose of the hands
		std::array<ovrPoseStatef, 2> handPoses;

		///State of an input peripheral from OVR
		ovrInputState inputState;

		///Array of indexes of buttons. Indexes are fixed signed 32 bits
		std::array<std::array<int32_t, 4>, 2> touchControllersButtons;

		///Tracking state
		ovrTrackingState ts;

		///Pointer to the layer to be submitted
		ovrLayerHeader* layers;

		///Status of the current session
		ovrSessionStatus sessionStatus;

		///State of the performance HUD
		int perfHudMode;

		///Index of the current texture in the textureCombinedSwapChain
		int currentCombinedIndex;

		///Array of 2 current texture swapchain index
		std::array<int, 2> currentSeparatedIndex;

		///Frame index of the current session status
		uID currentSessionStatusFrameIndex;

		///Convert an ovrVector3f to an Ogre::Vector3
		static Ogre::Vector3 oculusToOgreVect3(const ovrVector3f& v);

		///Convert an ovrQuatf to an Ogre::Quaternion
		static Ogre::Quaternion oculusToOgreQuat(const ovrQuatf& q);

		///Ogre Scene Manager
		Ogre::SceneManager* debugSmgr;

		///Additional camera objects
		Ogre::Camera* debugCam;

		///Nodes for the debug scene
		Ogre::SceneNode *debugCamNode, *debugPlaneNode;

		///Pointer to the debug plane manual material
		Ogre::MaterialPtr debugPlaneMaterial;

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
		static constexpr const std::array<const float, 2> debugPlanHalfGeometry{
			debugPlaneGeometry[0] / 2.f,
			debugPlaneGeometry[1] / 2.f
		};

		///List of the points that form the debug plane (a quad made of two triangles)
		static constexpr const std::array<const std::array<const float, 3>, 4> debugPlaneVertexBuffer{
			{
				{ -debugPlanHalfGeometry[0], debugPlanHalfGeometry[1], 0 },
				{ -debugPlanHalfGeometry[0], -debugPlanHalfGeometry[1], 0 },
				{ debugPlanHalfGeometry[0], debugPlanHalfGeometry[1], 0 },
				{ debugPlanHalfGeometry[0], -debugPlanHalfGeometry[1], 0 },
			}
		};

		///Texture coordinates to map a whole texture to the debug plane
		static constexpr const std::array<const std::array<const float, 2>, 4> debugPlaneTextureCoord{
			{ { 0, 0 },
			  { 0, 1 },
			  { 1, 0 },
			  { 1, 1 } }
		};

		///Index buffer of the debug plane
		static constexpr const std::array<const uint8_t, 4> debugPlaneIndexBuffer{ 0, 1, 2, 3 };

		///Index buffer of a quad
		static constexpr const std::array<const uint8_t, 4> quadIndexBuffer{ 0, 1, 2, 3 };

		///Preferred order to update eyes
		static constexpr const std::array<const OgreOculusEyeType, 2> eyeUpdateOrder{ { left, right } };

		///Return true if the array sizes of each buffer are constants
		static constexpr bool debugPlaneSanityCheck()
		{
			return debugPlaneIndexBuffer.size() == debugPlaneVertexBuffer.size()
				&& debugPlaneIndexBuffer.size() == debugPlaneTextureCoord.size();
		}

	public:
		///Texture unit state of the debug plane
		static Ogre::TextureUnitState* debugTexturePlane;
	};
}

extern "C" AnnDllExport Annwvyn::AnnOgreVRRenderer* AnnRendererBootstrap_Oculus(const std::string& appName);

#endif //_WIN32
