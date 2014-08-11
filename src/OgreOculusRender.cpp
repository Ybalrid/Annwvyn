#include "OgreOculusRender.hpp"
OgreOculusRender::OgreOculusRender(std::string winName)
{

    name = winName;
    root = NULL;
    window = NULL;
    smgr = NULL;
    for(size_t i(0); i < 2; i++)
    {
        cams[i] = NULL;
        rtts[i] = NULL;
        vpts[i] = NULL;
    }

    oc = NULL;
    CameraNode = NULL;
    cameraPosition = Ogre::Vector3(0,0,10);
    cameraOrientation = Ogre::Quaternion::IDENTITY;
    this->nearClippingDistance = 0.05;
    this->lastOculusPosition = cameraPosition;
    this->lastOculusOrientation = cameraOrientation;
    this->updateTime = 0;
}

OgreOculusRender::~OgreOculusRender()
{
    delete oc;
}

void OgreOculusRender::loadReseourceFile(const char path[])
{
    /*from ogre wiki : load the given resource file*/
    Ogre::ConfigFile configFile;
    configFile.load(path);

    Ogre::ConfigFile::SectionIterator seci = configFile.getSectionIterator();
    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                    archName, typeName, secName);
        }
    }  
}

void OgreOculusRender::initAllResources()
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void OgreOculusRender::initLibraries()
{
    //Create the ogre root
    root = new Ogre::Root("plugins.cfg","ogre.cfg","Ogre.log");
    //Class to get basic information from the Rift. Initialize the RiftSDK 
    oc = new OculusInterface();
}

void OgreOculusRender::initialize()
{
    //init libraries;
    initLibraries();

    assert(root != NULL && oc != NULL);

    //Get configuration via ogre.cfg OR via the config dialog.
    getOgreConfig();
    
    //Create the render window with the given sice from the Oculus 
    createWindow();
    
    //Load resources from the resources.cfg file
    loadReseourceFile("resources.cfg");
    initAllResources();

    //Create scene manager
    initScene();
    
    //Create cameras and handeling nodes
    initCameras();

    //Create rtts and viewports on them
    initRttRendering();
    
    //Init the oculus rendering
    initOculus();
}

void OgreOculusRender::getOgreConfig()
{
    assert(root != NULL);
    if(!root->restoreConfig())
        if(!root->showConfigDialog())
            abort();
}

void OgreOculusRender::createWindow()
{
    assert(root != NULL && oc != NULL);
    Ogre::NameValuePairList misc;
    misc["border"]="none";
    window = root->initialise(false, name);
    window = root->createRenderWindow(name, oc->getHmdDesc().Resolution.w, oc->getHmdDesc().Resolution.h, false, &misc);
    window->reposition(oc->getHmdDesc().WindowsPos.x,oc->getHmdDesc().WindowsPos.y);
}

void OgreOculusRender::initCameras()
{
    assert(smgr != NULL);
    cams[left] = smgr->createCamera("lcam");
    cams[right] = smgr->createCamera("rcam");
    for(int i = 0; i < 2; i++)
    {
        cams[i]->setPosition(cameraPosition);
        cams[i]->setAutoAspectRatio(true);
        cams[i]->setNearClipDistance(1);
        cams[i]->setFarClipDistance(1000);
    }
    //do NOT attach camera to this node... 
    CameraNode =  smgr->getRootSceneNode()->createChildSceneNode();

}

void OgreOculusRender::setCamerasNearClippingDistance(float distance)
{
    nearClippingDistance = distance;
}

void OgreOculusRender::initScene()
{
    assert(root != NULL);
    smgr = root->createSceneManager("OctreeSceneManager","OSM_SMGR");
}

void OgreOculusRender::initRttRendering()
{
    //get texture sice from ovr with default fov
    texSizeL = ovrHmd_GetFovTextureSize(oc->getHmd(), ovrEye_Left, oc->getHmdDesc().DefaultEyeFov[0], 1.0f);
    texSizeR = ovrHmd_GetFovTextureSize(oc->getHmd(), ovrEye_Right, oc->getHmdDesc().DefaultEyeFov[1], 1.0f);

    //Create texture
    Ogre::TexturePtr rtt_textureL = Ogre::TextureManager::getSingleton().createManual("RttTexL", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, texSizeL.w, texSizeL.h, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);

    Ogre::TexturePtr rtt_textureR = Ogre::TextureManager::getSingleton().createManual("RttTexR", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, texSizeR.w, texSizeR.h, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);
    //Create Render Texture 
    Ogre::RenderTexture* rttEyeLeft = rtt_textureL->getBuffer(0,0)->getRenderTarget();
    Ogre::RenderTexture* rttEyeRight = rtt_textureR->getBuffer(0,0)->getRenderTarget();

    //Create and bind a viewport to the texture
    Ogre::Viewport* vptl = rttEyeLeft->addViewport(cams[left]);  
    vptl->setBackgroundColour(Ogre::ColourValue(0.3,0.3,0.9));
    Ogre::Viewport* vptr = rttEyeRight->addViewport(cams[right]);  
    vptr->setBackgroundColour(Ogre::ColourValue(0.3,0.3,0.9));
    
    //Store viewport pointer
    vpts[left] = vptl;
    vpts[right] = vptr;

    //Pupulate textures with an initial render
    rttEyeLeft->update();
    rttEyeRight->update();
    
    rttEyeLeft->writeContentsToFile("debug_init_cam_left.png");

    //Store rtt textures pointer
    rtts[left] = rttEyeLeft;
    rtts[right] = rttEyeRight;
}

void OgreOculusRender::initOculus()
{
    //Get FOV
    EyeFov[left] = oc->getHmdDesc().DefaultEyeFov[left];
    EyeFov[right] = oc->getHmdDesc().DefaultEyeFov[right];

    //Set OpenGL configuration 
    ovrGLConfig cfg;
    cfg.OGL.Header.API = ovrRenderAPI_OpenGL;
    cfg.OGL.Header.Multisample = 1;
    cfg.OGL.Header.RTSize = oc->getHmdDesc().Resolution;
    
    ///////////////////// LINUX SPECIFIC 
    //TODO define to protect that and to put Windows equivalent 
    size_t wID;
    window->getCustomAttribute("WINDOW", &wID);
    std::cout << "Wid : " << wID << endl;
    cfg.OGL.Win = wID;
    
    Display* display;
    window->getCustomAttribute("DISPLAY",&display);
    cfg.OGL.Disp = display;
    /////////////////////////////////////////////

    if(!ovrHmd_ConfigureRendering(
                oc->getHmd(),
                &cfg.Config,oc->getHmdDesc().DistortionCaps,
                EyeFov,
                EyeRenderDesc))
        abort();
    
    EyeTexture[left].OGL.Header.API = ovrRenderAPI_OpenGL;
    EyeTexture[left].OGL.Header.TextureSize = texSizeL;
    EyeTexture[left].OGL.Header.RenderViewport.Pos.x = 0;
    EyeTexture[left].OGL.Header.RenderViewport.Pos.y = 0; 
    EyeTexture[left].OGL.Header.RenderViewport.Size = texSizeL; 
    
    Ogre::GLTexture* gl_rtt_l = static_cast<Ogre::GLTexture*>(Ogre::GLTextureManager::getSingleton().getByName("RttTexL").get());
    EyeTexture[left].OGL.TexId = gl_rtt_l->getGLID();
    
    EyeTexture[right] = EyeTexture[0];
    EyeTexture[right].OGL.Header.TextureSize = texSizeR;
    EyeTexture[right].OGL.Header.RenderViewport.Size = texSizeR;
    
    Ogre::GLTexture* gl_rtt_r = static_cast<Ogre::GLTexture*>(Ogre::GLTextureManager::getSingleton().getByName("RttTexR").get());
    EyeTexture[right].OGL.TexId = gl_rtt_r->getGLID();
}

void OgreOculusRender::RenderOneFrame()
{
    //get some info
    cameraPosition = this->CameraNode->getPosition();
    cameraOrientation = this->CameraNode->getOrientation();
    //Begin frame
    ovrFrameTiming hmdFrameTiming = ovrHmd_BeginFrame(oc->getHmd(), 0);
    //Tell ogre that Frame started
    root->_fireFrameStarted();
    

    for (Ogre::SceneManagerEnumerator::SceneManagerIterator it = root->getSceneManagerIterator(); it.hasMoreElements(); it.moveNext())
        it.peekNextValue()->_handleLodEvents();


    //essage pump events 
    Ogre::WindowEventUtilities::messagePump();
    for(int eyeIndex = 0; eyeIndex < ovrEye_Count; eyeIndex++)
    {
        cout << "eye index = " << eyeIndex << endl;

        //Get the correct eye to render
        ovrEyeType eye = oc->getHmdDesc().EyeRenderOrder[eyeIndex];

        cout << "eye = " << eye << endl;

        //Set the Ogre render target to the texture
        root->getRenderSystem()->_setRenderTarget(rtts[eye]);
        

        //Get the eye pose 
        ovrPosef eyePose = ovrHmd_BeginEyeRender(oc->getHmd(), eye);
        
        //Get the hmd orientation
        OVR::Quatf camOrient = eyePose.Orientation;
        
        //Get the projection matrix
        OVR::Matrix4f proj = ovrMatrix4f_Projection(EyeRenderDesc[eye].Fov,static_cast<float>(nearClippingDistance), 10000.0f, true);

        //Convert it to Ogre matrix
        Ogre::Matrix4 OgreProj;
        for(int x(0); x < 4; x++)
            for(int y(0); y < 4; y++)
                OgreProj[x][y] = proj.M[x][y];
        
        
        //Set the matrix
        cams[eye]->setCustomProjectionMatrix(true, OgreProj);
        cams[eye]->setNearClipDistance(this->nearClippingDistance);
        //Set the orientation
        cams[eye]->setOrientation(cameraOrientation * Ogre::Quaternion(camOrient.w,camOrient.x,camOrient.y,camOrient.z));

        //Set Position
        cams[eye]->setPosition( cameraPosition + 
                (cams[eye]->getOrientation() *
                -Ogre::Vector3(
                    EyeRenderDesc[eye].ViewAdjust.x,
                    EyeRenderDesc[eye].ViewAdjust.y,
                    EyeRenderDesc[eye].ViewAdjust.z))
                );

        cout << "VIEW ADJUST VECTOR" << endl
            << "left : (x, y, z) : ("
            << EyeRenderDesc[0].ViewAdjust.x << ", "
            << EyeRenderDesc[0].ViewAdjust.y << ", "
            << EyeRenderDesc[0].ViewAdjust.z << ")" << endl
            << "right : (x, y, z) : ("
            << EyeRenderDesc[1].ViewAdjust.x << ", "
            << EyeRenderDesc[1].ViewAdjust.y << ", "
            << EyeRenderDesc[1].ViewAdjust.z << ")" << endl;

        if(eye == left) //get an eye pos/orient for game logic
        {
            this->lastOculusPosition = cams[eye]->getPosition();
            this->lastOculusOrientation = cams[eye]->getOrientation();
        }

        root->_fireFrameRenderingQueued();
        rtts[eye]->update();
        ovrHmd_EndEyeRender(oc->getHmd(), eye, eyePose, &EyeTexture[eye].Texture);
    }
    Ogre::Root::getSingleton().getRenderSystem()->_setRenderTarget(window); 
    this->updateTime = hmdFrameTiming.DeltaSeconds;

    //Tell Ogre that frame ended
    ovrHmd_EndFrame(oc->getHmd());
    root->_fireFrameEnded();

    debugPrint();
}

