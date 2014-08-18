#include <glew.h>
#ifndef _WIN32 //NOT ON WINDOWS (Assuming Linux. Usinig X Window System as display server
#include <glxew.h> //Need GLX glew on X window system
#else
#include <wglew.h> //Need wgelw
#endif

//C++ SDL Includes
#include <iostream>
#include <sstream>

//Oculus Rift Lib
#include <OVR.h>
//CAPI
#include <OVR_CAPI.h>

//OpenGL CAPI OVR
#include <OVR_CAPI_GL.h>
#include <CAPI/GL/CAPI_GL_Util.h>

#ifdef _WIN32
//Direct3D CAPI OVR. Forcing version 11
#define OVR_D3D_VERSION 11
#include <OVR_CAPI_D3D.h>
#include <CAPI/D3D1X/CAPI_D3D1X_Util.h>
#endif

#ifndef _WIN32 //Assuming Linux  here. Any Unix based
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

#ifdef _WIN32 //Possibility of adding D3D11 compatibility
#include <RenderSystems/Direct3D11/OgreD3D11RenderSystem.h>
#include <RenderSystems/Direct3D11/OgreD3D11Texture.h>
#include <RenderSystems/Direct3D11/OgreD3D11TextureManager.h>
#endif

//Accessing Oculus Rift through a class : 
#include "OculusInterface.hpp"

//OS Specific build macro 
#include "systemMacro.h"

using namespace std;
using namespace OVR;


class DLL OgreOculusRender
{
    public:
        OgreOculusRender(std::string windowName = "OgreOculusRender");
        ~OgreOculusRender();

		void RenderOneFrame();

		void setCamerasNearClippingDistance(float distance);

        void initialize();

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
        
        void initOculus();
		void initOculus(bool fullscreenState)
		{
			setFullScreen(fullscreenState);
			initOculus();
		}

		void setFullScreen(bool fs = true)
		{
			fullscreen = fs;
		}

		bool isFullscreen()
		{
			return fullscreen;
		}


        Ogre::SceneManager* getSceneManager()
        {
            return smgr;
        }

        Ogre::RenderWindow* getWindow()
        {
            return window;
        }

        void debugPrint()
        {
            for(int i(0); i < 2; i++)
            {
               cout << "cam " << i << " " << cams[i]->getPosition() << endl;
               cout << cams[i]->getOrientation() << endl;
            }
        }

        void debugSaveToFile(const char path[])
        {
            if(rtts[0]) rtts[0]->writeContentsToFile(path);
        }

        Ogre::SceneNode* getCameraInformationNode()
        {
            return CameraNode;
        }

        Ogre::Timer* getTimer()
        {
            if(root)
                return root->getTimer();
            return NULL;
        }

        float getUpdateTime()
        {
            return updateTime;
        }

		void recenter()
		{
			ovrHmd_RecenterPose(oc->getHmd());
		}

    private:
        enum 
        {
            left = 0,
            right = 1
        };

		bool fullscreen;

        //Name of the Window
        string name;
        //Ogre
        Ogre::Root* root;
        Ogre::RenderWindow* window;
        Ogre::SceneManager* smgr;
        Ogre::Camera* cams[2];
        Ogre::SceneNode* CameraNode;
        Ogre::RenderTexture* rtts[2];
        Ogre::Viewport* vpts[2];
        Ogre::Real nearClippingDistance;
        float updateTime; //seconds

        //Oculus
        OculusInterface* oc;
        ovrFovPort EyeFov[2];
        ovrEyeRenderDesc EyeRenderDesc[2];
        ovrGLConfig cfg; //OpenGL
        ovrGLTexture EyeTexture[2];
#ifdef _WIN32
		ovrD3D11Texture D3D11EyeTexture[2];
#endif
        ovrSizei texSizeL;
        ovrSizei texSizeR;

        Ogre::Vector3 cameraPosition;
        Ogre::Quaternion cameraOrientation;

		bool direct3D;

    public:
        Ogre::Vector3 lastOculusPosition;
        Ogre::Quaternion lastOculusOrientation;
};


