/**
 * \file OgreOculusRenderer.hpp
 * \brief Initialize rendering for the rift with SDK post traitement (OpenGL ONLY)
 * \author A. Brainville (Ybalrid)
 */

#ifndef OGRE_OCULUS_RENDERER
#define OGRE_OCULUS_RENDERER

#include <glew.h>
#ifndef _WIN32 //NOT ON WINDOWS (Assuming Linux. Usinig X Window System as display server
#include <glxew.h> //Need GLX glew on X window system
#else
#include <wglew.h> //Need wgelw
#endif


//Oculus Rift Lib
#include <OVR.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <OVR_CAPI_GL.h>

//C++ SDL Includes
#include <iostream>
#include <sstream>

/*
#ifdef _WIN32
//Direct3D CAPI OVR. Forcing version 11
#define OVR_D3D_VERSION 11
#include <OVR_CAPI_D3D.h>
#include <CAPI/D3D1X/CAPI_D3D1X_Util.h>
#endif
*/

#ifndef _WIN32 //Assuming Linux  here. Any Unix based have that header includable
#include <unistd.h>
#endif

//Ogre
#include <Ogre.h>

//Texture manipulation
#include <OgreTexture.h>

//OpenGL Classes. Some low level access required
#include <RenderSystems/GL/OgreGLRenderSystem.h>
#include <RenderSystems/GL/OgreGLRenderTexture.h>
#include <RenderSystems/GL/OgreGLTexture.h>
#include <RenderSystems/GL/OgreGLTextureManager.h>

/*
#ifdef _WIN32 //Possibility of adding D3D11 compatibility
#include <RenderSystems/Direct3D11/OgreD3D11RenderSystem.h>
#include <RenderSystems/Direct3D11/OgreD3D11Texture.h>
#include <RenderSystems/Direct3D11/OgreD3D11TextureManager.h>
#endif
*/
//Accessing Oculus Rift through a class : 
#include "OculusInterface.hpp"

//OS Specific build macro 
#include "systemMacro.h"

using namespace std;
using namespace OVR;

struct OgrePose
{
	Ogre::Vector3 position;
	Ogre::Quaternion orientation;
};

class DLL OgreOculusRender
{
    public:
        OgreOculusRender(std::string windowName = "OgreOculusRender");
        ~OgreOculusRender();

		///Calculate, time and present a frame on the Rift display
		void RenderOneFrame();

		///Set the near Z clipping plane distance from the POV. Used to calculate Projections matricies
		void setCamerasNearClippingDistance(float distance);

		///Automatic initialization of the renderer.
        void initialize();

		///Start Oculus and Ogre libraries.
        void initLibraries();

		///Load the given 'resource.cfg' file. See Ogre Help for referene here.
        void loadReseourceFile(const char path[]);

		//Init all resource groups loaded on resource group manager. 
        void initAllResources();
        
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
		///Overload of initOculus(); Permit to specify the Full Screen mode (or not)
		void initOculus(bool fullscreenState = true);

		///Set fullscreen. Value only used at window creation
		void setFullScreen(bool fs = true);


		///Return true if fullscreen set.
		bool isFullscreen();


		///Get the scene manager.
        Ogre::SceneManager* getSceneManager();


		///Get the RenderWindow
        Ogre::RenderWindow* getWindow();


		///Print various informations about the cameras
        void debugPrint();


		///Save content of 'left eye' RenderTexture to the specified file. Please use a valid extentsion of a format handeled by FreeImage
        void debugSaveToFile(const char path[]);


		///Get a node representing the camera. NOTE: Camera isn"t attached.
        Ogre::SceneNode* getCameraInformationNode();

		///Get the timer
        Ogre::Timer* getTimer();


		///Get time between frames
        float getUpdateTime();

		///Recenter rift to default position.
		void recenter();

		///Get to know if the Health and Safety warning dissmiss has be requested
		bool IsHsDissmissed();

		///Request the dissmiss of the Health and Safety warning
		void dissmissHS();

    private:
        enum 
        {
            left = 0,
            right = 1
        };

		///If true, window will be created in full screen mode
		bool fullscreen;

		///background color of viewports
		Ogre::ColourValue backgroundColor;

        ///Name of the Window
        string name;
        ///Ogre Root instance
        Ogre::Root* root;
		///Ogre Render Window
        Ogre::RenderWindow* window;
		///Ogre Scene Manager
        Ogre::SceneManager* smgr;
		///Stereoscopic camera array. Indexes are "left" and "right"
        Ogre::Camera* cams[2];
	
        Ogre::SceneNode* CameraNode;

		///Textures used for RTT Rendering. Indexes are "left" and "right"
		Ogre::RenderTexture* rtts[2];
		///Vewports on textures. Textures are separated. One vieport for each textures
        Ogre::Viewport* vpts[2];
		///The Z axis near clipping plane distance
        Ogre::Real nearClippingDistance;
		///Time betwenn frames in seconds
        float updateTime;

        ///Object for getting informations from the Oculus Rift
        OculusInterface* oc;
		///Fov descriptor for each eye. Indexes are "left" and "right"
        ovrFovPort EyeFov[2];
		///Render descriptor for each eye. Indexes are "left" and "right"
        ovrEyeRenderDesc EyeRenderDesc[2];
		///OpenGL Configuration
        ovrGLConfig cfg;
		///OpenGL Textures
        ovrGLTexture EyeTexture[2];
/*
#ifdef _WIN32
		///D3D11 Configuration
		ovrD3D11Config D3D11cfg;
		
		///D3D11 Textures
		ovrD3D11Texture D3D11EyeTexture[2];
		
		///On Windows only : true if RenderSystem Direct3D 11 is loaded instead of GL RenderSystem.
		bool direct3D;

#endif
		*/
		///Size of left eye texture
        ovrSizei texSizeL;
		///Size of right eye texture
        ovrSizei texSizeR;

		///Position of the camera.
        Ogre::Vector3 cameraPosition;
		///Orientation of the camera.
        Ogre::Quaternion cameraOrientation;

		bool hsDissmissed;
	

    public:
        Ogre::Vector3 lastOculusPosition;
        Ogre::Quaternion lastOculusOrientation;
		OgrePose returnPose;
};


#endif //OGRE_OCULUS_RENDERER
