/**
 * \file OgreOculusRenderer.hpp
 * \brief Initialize the Oculus Rendering pipeline, then render the main scene and submit it to Oculus Compositor
 * \author A. Brainville (Ybalrid)
 */

///huge thanks to Germanunkol (aka ueczz on Oculus Forums) https://github.com/Germanunkol/OgreOculusSample
///(even if now I'm not using code from him anymore)
///Shout out to Kojack too for his post of an OgreOculus class a short time after DK1 was out.
///The website http://learnopengl.com/ for improving my (verry little at the time) understanding of OpenGL 

#ifndef OGRE_OCULUS_RENDERER
#define OGRE_OCULUS_RENDERER

#include "OgreVRRender.hpp"

//Oculus Rift Lib
//#include <OVR.h>
#include <OVR_CAPI_GL.h>

//C++ SDL Includes
#include <iostream>
#include <sstream>

//Accessing Oculus Rift through a class : 
#include "OculusInterface.hpp"

//OS Specific build macro 
#include "systemMacro.h"
#ifdef _WIN32
#include <Windows.h>
#include <glew.h>
#endif

//We need to get low level access to Ogre's RenderSystem_GL 
#include <RenderSystems/GL/OgreGLTextureManager.h>
#include <RenderSystems/GL/OgreGLRenderSystem.h>
#include <RenderSystems/GL/OgreGLTexture.h>

#include "AnnErrorCode.hpp"
#include "AnnTypes.h"

///Do the initialization and graphical rendering for the Oculus Rift using Ogre
class DLL OgreOculusRender : public OgreVRRender
{
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

		bool shouldQuit();
		bool shouldRecenter();
		bool isVisibleInHmd();

		///Cycle through all Oculus Performance HUD available
		void cycleDebugHud();
		
		///Get the timing and tracking state form the oculus runtime and moves the cameras according to the reference and the tracked data
		void updateTracking();

		///Render each texture buffer, copy contne to the oculus swap texture, get mirror texture data to the OgreMirror texture and update debug window
		void renderAndSubmitFrame();

		///Set the near Z clipping plane distance from the POV. Used to calculate Projections matricies
		void setCamerasNearClippingDistance(float distance = 0.15f);
		void setCameraFarClippingDistance(float distance);


		///Start Oculus and Ogre libraries.
        void initVrHmd();
        
		///Get Configuration from ogre.cfg or display a Dialog. The Resolution and FullScreen settings will be ignored. 
		void getOgreConfig();
        
		///Create the RenderWindow
        void createWindow();

		///Initialize the SceneManager for the aplication. 
        void initScene();

		///Initialise the camera for Stereo Render
        void initCameras();

		///Initialise the RTT Rendering System. Create two textures, two viewports and bind cameras on them.
        void initRttRendering();
        
		///Init the Rift rendering. Configure Oculus SDK to use the two RTT textures created.
		void initClientHmdRendering();
		
		///Print various informations about the cameras
        void debugPrint();

		///Get the timer
        Ogre::Timer* getTimer();

		///Recenter rift to default position.
		void recenter();

		///Compute from OVR the correct projection matrix for the given clipping distance
		void calculateProjectionMatrix();

		///change main viewport background color
		void changeViewportBackgroundColor(Ogre::ColourValue color);

		///Show in debug window what the camera are seeing
		static void showRawView();

		///Show in the debug window what the Oculus service send as mirrored view
		static void showMirrorView();

		///Show in the debug window a monoscopic view with a default Fov of 90°
		static void showMonscopicView();

		///Get the status of the session
		ovrSessionStatus getSessionStatus();

		///Init the rendering pipeline
		void initPipeline();

    private://Methods
		///Set the Fov for the monoscopic view
		void setMonoFov(float degreeFov);

	private://Attributes
		static OgreOculusRender* OculusSelf;
		///Save content of the RenderTexture to the specified file. This verry slow operation is only usefull for debuging the renderer itself
        void debugSaveToFile(const char path[]);
		
		///Object for getting informations from the Oculus Rift
        OculusInterface* Oculus; 


		///Ogre Scene Manager
        Ogre::SceneManager* debugSmgr;	

		///Stereoscopic camera array. Indexes are "left" and "right" + debug view cam
        Ogre::Camera* eyeCameras[2], * debugCam, * monoCam; 
		
		///Nodes for the debug scene
		Ogre::SceneNode* debugCamNode, * debugPlaneNode; 

		///Vewports on textures. Textures are separated. One vieport for each textures
        Ogre::Viewport* vpts[2], *debugViewport;

		///The Z axis clipping planes distances
        Ogre::Real nearClippingDistance, farClippingDistance;

		///Timing in seconds 
		double currentFrameDisplayTime, lastFrameDisplayTime;

		///Render descriptor for each eye. Indexes are "left" and "right"
        ovrEyeRenderDesc EyeRenderDesc[2];

		///Size of texture and headset
        ovrSizei bufferSize, hmdSize;

		///Mirror texture 
		ovrMirrorTexture mirrorTexture;

		///OpenGL Texture ID of the render buffers
		GLuint oculusMirrorTextureGLID, ogreMirrorTextureGLID, oculusRenderTextureGLID, renderTextureGLID;

		///Pointer to the debug plane manual material
		Ogre::MaterialPtr DebugPlaneMaterial;

		///If true, need to copy the mirrored buffer from Oculus to Ogre
		static bool mirrorHMDView;

		///Compositing layer for the rendered scene
		ovrLayerEyeFov layer;

		///GL texture set for the rendering
		ovrTextureSwapChain textureSwapChain; 

		///offset between render center and camera (for IPD variation)
		ovrVector3f offset[2];

		///Pose (position+orientation) 
		ovrPosef pose;

		///Tracking state
		ovrTrackingState ts;

		///Orientation of the headset
		ovrQuatf oculusOrient;

		///Position of the headset
		ovrVector3f oculusPos;

		///Pointer to the layer to be submited
		ovrLayerHeader* layers;

		///Staus of the current session
		ovrSessionStatus sessionStatus;

		///State of the performance HUD
		int perfHudMode;

		///Index of the current texture in the textureSwapChain
		int currentIndex;

    public:
		///Position of the rift at the last frame
        Ogre::Vector3 lastOculusPosition;

		///Orientation of the rift at the last frame
        Ogre::Quaternion lastOculusOrientation;

		///Pose to be returned
		OgrePose returnPose;

		///Texture unit state of the debug plane
		static Ogre::TextureUnitState* debugTexturePlane;

};
#endif //OGRE_OCULUS_RENDERER
