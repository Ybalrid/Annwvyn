/// Copyright (C) 2013 Kojack
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
/// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
/// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
/// DEALINGS IN THE SOFTWARE.

/// Modified 05 Auggust 2013 by Ybalrid

#include "OgreOculus.h"
#include <OVR.h>
#include <Ogre.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreCompositorManager.h>
#include <OgreCompositorInstance.h>
#include <OgreCompositionTargetPass.h>
#include <OgreCompositionPass.h>


using namespace OVR;

namespace
{
	const float g_defaultNearClip = 0.01f;
	const float g_defaultFarClip = 10000.0f;
	const float g_defaultIPD = 0.064f;
	const Ogre::ColourValue g_defaultViewportColour(97/255.0f, 97/255.0f, 200/255.0f);
	const float g_defaultProjectionCentreOffset = 0.14529906f;
	const float g_defaultDistortion[4] = {1.0f, 0.22f, 0.24f, 0};
}


Oculus::Oculus(void):m_sensorFusion(0),
					 m_stereoConfig(0),
					 m_hmd(0),
					 m_deviceManager(0),
					 m_oculusReady(false),
					 m_ogreReady(false),
					 m_driftCorrection(false),
					 m_sensor(0),
					 m_centreOffset(g_defaultProjectionCentreOffset),
					 m_window(0),
					 m_sceneManager(0),
					 m_cameraNode(0)
{
	for(int i=0;i<2;++i)
	{
		m_cameras[i] = 0;
		m_viewports[i] = 0;
		m_compositors[i] = 0;
	}
}

Oculus::~Oculus(void)
{
	shutDownOgre();
	shutDownOculus();
}

void Oculus::shutDownOculus()
{
	m_oculusReady = false;

	delete m_stereoConfig;
	m_stereoConfig = 0;
	delete m_sensorFusion;
	m_sensorFusion = 0;

	if(m_sensor)
	{
		m_sensor->Release();
	}
	if(m_hmd)
	{
		m_hmd->Release();
		m_hmd = 0;
	}
	if(m_deviceManager)
	{
		m_deviceManager->Release();
		m_deviceManager = 0;
	}

	System::Destroy();
}

void Oculus::shutDownOgre()
{
	m_ogreReady = false;
	for(int i=0;i<2;++i)
	{
		if(m_compositors[i])
		{
			Ogre::CompositorManager::getSingleton().removeCompositor(m_viewports[i], "Oculus");
			m_compositors[i] = 0;
		}
		if(m_viewports[i])
		{
			m_window->removeViewport(i);
			m_viewports[i] = 0;
		}
		if(m_cameras[i])
		{
			m_cameras[i]->getParentSceneNode()->detachObject(m_cameras[i]);
			m_sceneManager->destroyCamera(m_cameras[i]);
			m_cameras[i] = 0;
		}
	}
	if(m_cameraNode)
	{
		m_cameraNode->getParentSceneNode()->removeChild(m_cameraNode);
		m_sceneManager->destroySceneNode(m_cameraNode);
		m_cameraNode = 0;
	}
	m_window = 0;
	m_sceneManager = 0;
}

bool Oculus::isOculusReady() const
{
	return m_oculusReady;
}

bool Oculus::isOgreReady() const
{
	return m_ogreReady;
}

bool Oculus::setupOculus()
{
	if(m_oculusReady)
	{
		Ogre::LogManager::getSingleton().logMessage("Oculus: Already Initialised");
		return true;
	}
	Ogre::LogManager::getSingleton().logMessage("Oculus: Initialising system");
	System::Init(Log::ConfigureDefaultLog(LogMask_All));
	m_deviceManager = DeviceManager::Create();
	if(!m_deviceManager)
	{
		Ogre::LogManager::getSingleton().logMessage("Oculus: Failed to create Device Manager");
		return false;
	}
	Ogre::LogManager::getSingleton().logMessage("Oculus: Created Device Manager");
	m_stereoConfig = new Util::Render::StereoConfig();
	if(!m_stereoConfig)
	{
		Ogre::LogManager::getSingleton().logMessage("Oculus: Failed to create StereoConfig");
		return false;
	}
	m_centreOffset = m_stereoConfig->GetProjectionCenterOffset();
	Ogre::LogManager::getSingleton().logMessage("Oculus: Created StereoConfig");
	m_hmd = m_deviceManager->EnumerateDevices<HMDDevice>().CreateDevice();
	if(!m_hmd)
	{
		Ogre::LogManager::getSingleton().logMessage("Oculus: Failed to create HMD");
		return false;
	}
	Ogre::LogManager::getSingleton().logMessage("Oculus: Created HMD");
	HMDInfo devinfo;
	m_hmd->GetDeviceInfo(&devinfo);
	m_stereoConfig->SetHMDInfo(devinfo);
	m_sensor = m_hmd->GetSensor();
	if(!m_sensor)
	{
		Ogre::LogManager::getSingleton().logMessage("Oculus: Failed to create sensor");
		return false;
	}
	Ogre::LogManager::getSingleton().logMessage("Oculus: Created sensor");
	m_sensorFusion = new SensorFusion();
	if(!m_sensorFusion)
	{
		Ogre::LogManager::getSingleton().logMessage("Oculus: Failed to create SensorFusion");
		return false;
	}
	Ogre::LogManager::getSingleton().logMessage("Oculus: Created SensorFusion");
	m_sensorFusion->AttachToSensor(m_sensor);
	m_oculusReady = true;
	Ogre::LogManager::getSingleton().logMessage("Oculus: Oculus setup completed successfully");

	return true;
}

bool Oculus::setupOgre(Ogre::SceneManager *sm, Ogre::RenderWindow *win, Ogre::SceneNode *parent)
{
	m_window = win;
	m_sceneManager = sm;
	Ogre::LogManager::getSingleton().logMessage("Oculus: Setting up Ogre");
	if(parent)
		m_cameraNode = parent->createChildSceneNode("StereoCameraNode");
	else
		m_cameraNode = sm->getRootSceneNode()->createChildSceneNode("StereoCameraNode");

	m_cameras[0] = sm->createCamera("CameraLeft");
	m_cameras[1] = sm->createCamera("CameraRight");
	
	Ogre::MaterialPtr matLeft = Ogre::MaterialManager::getSingleton().getByName("Ogre/Compositor/Oculus");
	Ogre::MaterialPtr matRight = matLeft->clone("Ogre/Compositor/Oculus/Right");
	Ogre::GpuProgramParametersSharedPtr pParamsLeft = matLeft->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
	Ogre::GpuProgramParametersSharedPtr pParamsRight = matRight->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
	Ogre::Vector4 hmdwarp;
	if(m_stereoConfig)
	{
		hmdwarp = Ogre::Vector4(m_stereoConfig->GetDistortionK(0),
								m_stereoConfig->GetDistortionK(1),
								m_stereoConfig->GetDistortionK(2),
								m_stereoConfig->GetDistortionK(3));
	}
	else
	{
		hmdwarp = Ogre::Vector4(g_defaultDistortion[0],
								g_defaultDistortion[1],
								g_defaultDistortion[2],
								g_defaultDistortion[3]);
	}
	pParamsLeft->setNamedConstant("HmdWarpParam", hmdwarp);
	pParamsRight->setNamedConstant("HmdWarpParam", hmdwarp);
	pParamsLeft->setNamedConstant("LensCentre", 0.5f+(m_stereoConfig->GetProjectionCenterOffset()/2.0f));
	pParamsRight->setNamedConstant("LensCentre", 0.5f-(m_stereoConfig->GetProjectionCenterOffset()/2.0f));

	Ogre::CompositorPtr comp = Ogre::CompositorManager::getSingleton().getByName("OculusRight");
	comp->getTechnique(0)->getOutputTargetPass()->getPass(0)->setMaterialName("Ogre/Compositor/Oculus/Right");

	for(int i=0;i<2;++i)
	{
		m_cameraNode->attachObject(m_cameras[i]);
		if(m_stereoConfig)
		{
			// Setup cameras.
			m_cameras[i]->setNearClipDistance(m_stereoConfig->GetEyeToScreenDistance());
			m_cameras[i]->setFarClipDistance(g_defaultFarClip);
			m_cameras[i]->setPosition((i * 2 - 1) * m_stereoConfig->GetIPD() * 0.5f, 0, 0);
            std::cerr << "--------------- IPD -------------- " << m_stereoConfig->GetIPD() << std::endl;
			m_cameras[i]->setAspectRatio(m_stereoConfig->GetAspect());
			m_cameras[i]->setFOVy(Ogre::Radian(m_stereoConfig->GetYFOVRadians()));
			
			// Oculus requires offset projection, create a custom projection matrix
			Ogre::Matrix4 proj = Ogre::Matrix4::IDENTITY;
			float temp = m_stereoConfig->GetProjectionCenterOffset();
			proj.setTrans(Ogre::Vector3(-m_stereoConfig->GetProjectionCenterOffset() * (2 * i - 1), 0, 0));
			m_cameras[i]->setCustomProjectionMatrix(true, proj * m_cameras[i]->getProjectionMatrix());
		}
		else
		{
			m_cameras[i]->setNearClipDistance(g_defaultNearClip);
			m_cameras[i]->setFarClipDistance(g_defaultFarClip);
			m_cameras[i]->setPosition((i*2-1) * g_defaultIPD * 0.5f, 0, 0);
		}
		m_viewports[i] = win->addViewport(m_cameras[i], i, 0.5f*i, 0, 0.5f, 1.0f);
		m_viewports[i]->setBackgroundColour(g_defaultViewportColour);
		m_compositors[i] = Ogre::CompositorManager::getSingleton().addCompositor(m_viewports[i],i==0?"OculusLeft":"OculusRight");
		m_compositors[i]->setEnabled(true);
	}

	m_ogreReady = true;
	Ogre::LogManager::getSingleton().logMessage("Oculus: Oculus setup completed successfully");
	return true;
}

void Oculus::update()
{
	if(m_ogreReady)
	{
		m_cameraNode->setOrientation(getOrientation());
	}
}

Ogre::SceneNode *Oculus::getCameraNode()
{
	return m_cameraNode;
}

Ogre::Quaternion Oculus::getOrientation() //const 
{
	if(m_oculusReady)
    {
		Quatf q = m_sensorFusion->GetOrientation();
		return Ogre::Quaternion(q.w,q.x,q.y,q.z);
	}
	else
	{
		return Ogre::Quaternion::IDENTITY;
	}
}

Ogre::CompositorInstance *Oculus::getCompositor(unsigned int i)
{
	return m_compositors[i];
}

float Oculus::getCentreOffset() const
{
	return m_centreOffset;
}

void Oculus::resetOrientation()
{
	if(m_sensorFusion)
		m_sensorFusion->Reset();
}

OVR::Profile* Oculus::getProfile()
{

    return NULL;
}

void Oculus::setNearClippingDistance(float dist)
{
    for(size_t i = 0; i < 2; i++)
        m_cameras[i]->setNearClipDistance(static_cast<Ogre::Real>(dist));
}
