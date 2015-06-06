#include "stdafx.h"
#include "OculusInterface.hpp"
#define USE_OGRE

OculusInterface::OculusInterface()
{
	initialized = false;
	firstUpdated = false;
	init();
}

OculusInterface::~OculusInterface()
{
#ifdef USE_OGRE
	Ogre::LogManager::getSingleton().logMessage("Shutdown OculusInterface object");
#endif   
	shutdown();
}

void OculusInterface::init()
{
#ifdef WIN32
	ovr_Initialize(nullptr);
	ovrResult r = ovrHmd_Create(0, &hmd);


	if(r != ovrSuccess)
	{
		cout << "Cannot get HMD" << endl;
		ovrHmd_CreateDebug(ovrHmd_DK2, &hmd);
	}

	r = ovrHmd_ConfigureTracking(hmd, //Oculus HMD
		ovrTrackingCap_Orientation |ovrTrackingCap_MagYawCorrection |ovrTrackingCap_Position, //Wanted capacities 
		0); //minial required 
	/*if(r != ovrSuccess)
	{
	std::cerr << "Unable to start sensor! The detected device by OVR is not capable to get sensor state. We cannot do anything with that..." << std::endl;
	ovrHmd_Destroy(hmd);
	ovr_Shutdown();
	abort(); 
	}*/

#else
	ovr_Initialize();
	hmd = ovrHmd_Create(0);

	if(!hmd)
	{
		cout << "Cannot get HMD" << endl;
		hmd = ovrHmd_CreateDebug(ovrHmd_DK2);
	}

	if(!ovrHmd_ConfigureTracking(hmd, //Oculus HMD
		ovrTrackingCap_Orientation |ovrTrackingCap_MagYawCorrection |ovrTrackingCap_Position, //Wanted capacities 
		ovrTrackingCap_Orientation)) //minial required 
	{
		std::cerr << "Unable to start sensor! The detected device by OVR is not capable to get sensor state. We cannot do anything with that..." << std::endl;
		ovrHmd_Destroy(hmd);
		ovr_Shutdown();
		abort();
	}

#endif

	customReport();
	initialized = true;
}

void OculusInterface::shutdown()
{
	if(initialized)
		ovrHmd_Destroy(hmd);
	ovr_Shutdown();
#ifdef USE_OGRE
	Ogre::LogManager::getSingleton().logMessage("LibOVR Shutdown... No longer can comunicate with OculusService or oculusd...");
#endif
}

void OculusInterface::customReport()
{
	cout << "================================================" << endl;
	cout << endl << "Detected Oculus Rift device :" << endl;
	cout << "Product name : " << hmd->ProductName << endl
		 << "Serial number : " << hmd->SerialNumber << endl  
		 << "Manufacturer : " << hmd->Manufacturer << endl
		 << "Display Resolution : " << hmd->Resolution.w << "x" << hmd->Resolution.h << endl 
		 << "Type of HMD identifier : " << hmd->Type << endl
		 << "Firmware version : " << hmd->FirmwareMajor << "." << hmd->FirmwareMinor << endl
	;
	cout << "================================================" << endl;
}

void OculusInterface::update(double time)
{
	if(!initialized) return;
	firstUpdated = true;
	ss = ovrHmd_GetTrackingState(hmd, time);
}

OVR::Vector3f OculusInterface::getPosition()
{
	if(initialized && firstUpdated)
		return OVR::Vector3f(ss.HeadPose.ThePose.Position);
	//TODO : get real data
	return OVR::Vector3f(0,0,0);
}

OVR::Quatf OculusInterface::getOrientation()
{
	if(initialized && firstUpdated)
		return OVR::Quatf(ss.HeadPose.ThePose.Orientation);
	//TODO : get real data
	return OVR::Quatf(1,0,0,0);
}

ovrHmdDesc OculusInterface::getHmdDesc()
{
	return hmdDesc;
}

ovrHmd OculusInterface::getHmd()
{
	return hmd;
}

void OculusInterface::debugPrint()
{
	if(!(initialized && firstUpdated)) return;

	OVR::Vector3f p = this->getPosition();
	OVR::Quatf q = this->getOrientation();

	float o_y,o_p,o_r;

	q.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&o_y,&o_p,&o_r);

	cout << "Rift information : " << endl
		<< "Position : " << "(" << p.x << ", "<< p.y<< ", " << p.z<< ")" << endl
		<< "Orientation : " << "(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")"<< endl
		<< "Euler Orientation angle (yaw, pitch, roll) "<< "(" << o_y << ", " << o_p << ", " << o_r << ")" << endl
		<< endl;
}
