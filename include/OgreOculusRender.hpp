#include <glew.h>
#ifndef _WIN32
#include <glxew.h>
#else
#include <wglew.h>
#endif
#include <iostream>
#include <sstream>

//Oculus Rift Lib
#include <OVR.h>
//CAPI
#include <OVR_CAPI.h>

//OpenGL stuff
#include <OVR_CAPI_GL.h>
#include <CAPI/GL/CAPI_GL_Util.h>

#ifdef _WIN32
//Direct3D stuff
#define OVR_D3D_VERSION 11
#include <OVR_CAPI_D3D.h>
#include <CAPI/D3D1X/CAPI_D3D1X_Util.h>
#endif

#ifndef _WIN32
#include <unistd.h>
#endif
#include <Ogre.h>
#include <OgreTexture.h>
#include <RenderSystems/GL/OgreGLRenderSystem.h>
#include <RenderSystems/GL/OgreGLRenderTexture.h>
#include <RenderSystems/GL/OgreGLTexture.h>
#include <RenderSystems/GL/OgreGLTextureManager.h>

#ifdef _WIN32
#include <RenderSystems/Direct3D11/OgreD3D11RenderSystem.h>
//#include <RenderSystems/Direct3D11/OgreD3D11RenderTexture.h>//does not exist. Apparently a "render texture" is a regular texture
#include <RenderSystems/Direct3D11/OgreD3D11Texture.h>
#include <RenderSystems/Direct3D11/OgreD3D11TextureManager.h>
#endif
#include "OculusInterface.hpp"

#include "systemMacro.h"

using namespace std;
using namespace OVR;


class DLL OgreOculusRender
{
    public:
        OgreOculusRender(std::string windowName = "OgreOculusRender");
        ~OgreOculusRender();
        void initialize();

        void initLibraries();

        void loadReseourceFile(const char path[]);
        void initAllResources();
        
        void getOgreConfig();
        
        void createWindow();
        void initScene();

        void initCameras();
        void initRttRendering();
        
        void initOculus();

        Ogre::SceneManager* getSceneManager()
        {
            return smgr;
        }
        Ogre::RenderWindow* getWindow()
        {
            return window;
        }

        void RenderOneFrame();

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
            if(rtts[0])rtts[0]->writeContentsToFile(path);
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

        void setCamerasNearClippingDistance(float distance);

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


