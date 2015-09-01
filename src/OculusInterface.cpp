#include "stdafx.h"
#include "OculusInterface.hpp"

OculusInterface::OculusInterface()
{
	initialized = false;
	firstUpdated = false;
	init();
}

OculusInterface::~OculusInterface()
{
	Ogre::LogManager::getSingleton().logMessage("Shutdown OculusInterface object");
	shutdown();
}

void OculusInterface::init()
{
#ifdef WIN32 //At the time of writing this, only windows uses "modern" version of the Oculus SDK
	ovr_Initialize(nullptr);
	ovrResult r = ovr_Create (&hmd, &luid);

	if(r != ovrSuccess)
	{
		cout << "Cannot get HMD" << endl;
		//Debug HMD is now handeled by the configuration utility and the runtime.
		cout << "Please note that if you want to use this program without a Rift pluged in, you have to activate the \"debug hmd\" setting on the runtime configuration utility" << endl;
#ifdef _WIN32
		MessageBox(NULL, L"Can't find any Oculus HMD! \n \n(Please note that if you want to use this Annwvyn application without an Oculus Rift, you NEED to activate the \"debug hmd\" setting on the Oculus runtime configuration utility)", L"Error, No Rift found!",  MB_ICONERROR);
#endif
		ovr_Shutdown();
		Ogre::LogManager::getSingleton().logMessage("Unable to get a valid HMD. Closing program and returning 0xDEAD60D error");
		delete Ogre::Root::getSingletonPtr();
		exit(ANN_ERR_CRITIC);
	}
	hmdDesc = ovr_GetHmdDesc(hmd);

	r = ovr_ConfigureTracking(hmd, //Oculus HMD
		ovrTrackingCap_Orientation |ovrTrackingCap_MagYawCorrection |ovrTrackingCap_Position, //Wanted capacities 
		0); //minial required 
#else //Linux, probably... Still using verry old Oculus...
	ovrHmd_Initialize();
	hmd = ovrHmd_Create(0);
	//No hmd
	if(!hmd)
	{
		cout << "Cannot get HMD. Create a debug HMD..." << endl;
		//Debug DK2
		hmd = ovrHmd_CreateDebug(ovr_DK2);
	}

	if(!ovrHmd_ConfigureTracking(hmd, //Oculus HMD
		ovrTrackingCap_Orientation |ovrTrackingCap_MagYawCorrection |ovrTrackingCap_Position, //Wanted capacities 
		ovrTrackingCap_Orientation)) //minial required 
	{
		std::cerr << "Unable to start sensor! The detected device by OVR is not capable to get sensor state. We cannot do anything with that..." << std::endl;
		ovrHmd_Destroy(hmd);
		ovrHmd_Shutdown();
		Ogre::LogManager::getSingleton().logMessage("Unable to get a valid HMD. Closing program and returning 0xDEAD60D error");
		delete Ogre::Root::getSingletonPtr();
		exit(ANN_ERR_CRITIC);
	}

#endif
	customReport();
	initialized = true;
}

void OculusInterface::shutdown()
{
	if(initialized)
		ovr_Destroy(hmd);
	ovr_Shutdown();
	Ogre::LogManager::getSingleton().logMessage("LibOVR Shutdown... No longer can comunicate with OculusService or oculusd...");
}

void OculusInterface::customReport()
{
	std::stringstream sout;
	sout << endl;
	sout << "================================================" << endl;
	sout << "Detected Oculus Rift device :" << endl;
	sout << "Product name : " << hmdDesc.ProductName << endl
		 << "Serial number : " << hmdDesc.SerialNumber << endl  
		 << "Manufacturer : " << hmdDesc.Manufacturer << endl
		 << "Display Resolution : " << hmdDesc.Resolution.w << "x" << hmdDesc.Resolution.h << endl 
		 << "Type of HMD identifier : " << hmdDesc.Type << endl
		 << "Firmware version : " << hmdDesc.FirmwareMajor << "." << hmdDesc.FirmwareMinor << endl
	;sout << "================================================" << endl;
	Ogre::LogManager::getSingleton().logMessage(sout.str());
}

void OculusInterface::update(double time)
{
	if(!initialized) return;
	firstUpdated = true;
	ss = ovr_GetTrackingState(hmd, time);
}

OVR::Vector3f OculusInterface::getPosition()
{
	if(initialized && firstUpdated)
		return OVR::Vector3f(ss.HeadPose.ThePose.Position);
	return OVR::Vector3f(0, 0, 0);
}

OVR::Quatf OculusInterface::getOrientation()
{
	if(initialized && firstUpdated)
		return OVR::Quatf(ss.HeadPose.ThePose.Orientation);
	return OVR::Quatf(1, 0, 0, 0);
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

	float o_y, o_p, o_r;

	q.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&o_y, &o_p, &o_r);

	cout << "Rift information : " << endl
		<< "Position : " << "(" << p.x << ", "<< p.y<< ", " << p.z<< ")" << endl
		<< "Orientation : " << "(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")"<< endl
		<< "Euler Orientation angle (yaw, pitch, roll) "<< "(" << o_y << ", " << o_p << ", " << o_r << ")" << endl
		<< endl;
}
