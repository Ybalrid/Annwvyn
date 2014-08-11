#include <glew.h>
#include <glxew.h>
#include <iostream>
#include <OVR.h>
#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>
#include <CAPI/GL/CAPI_GL_Util.h>

#include <unistd.h>

#include <Ogre.h>
#include <OgreTexture.h>
#include <RenderSystems/GL/OgreGLRenderSystem.h>
#include <RenderSystems/GL/OgreGLRenderTexture.h>
#include <RenderSystems/GL/OgreGLTexture.h>
#include <RenderSystems/GL/OgreGLTextureManager.h>

#include "OculusInterface.hpp"

using namespace std;
using namespace OVR;


class OgreOculusRender
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
        ovrSizei texSizeL;
        ovrSizei texSizeR;

        Ogre::Vector3 cameraPosition;
        Ogre::Quaternion cameraOrientation;

    public:
        Ogre::Vector3 lastOculusPosition;
        Ogre::Quaternion lastOculusOrientation;
};

