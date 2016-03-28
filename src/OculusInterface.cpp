#include "stdafx.h"
#include "OculusInterface.hpp"
#include "AnnLogger.hpp"

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
		Annwvyn::AnnDebug() << "Error: Cannot get HMD";
		//Debug HMD is now handeled by the configuration utility and the runtime.
		Annwvyn::AnnDebug() << "Please note that if you want to use this program without a Rift pluged\
in, you have to activate the \"debug hmd\" setting on the runtime configuration utility";
#ifdef _WIN32
		MessageBox(NULL, 
			L"Can't find any Oculus HMD!\n\n(Please note that if you want to use this Annwvyn \
application without an Oculus Rift, you NEED to activate the \"debug hmd\" setting \
on the Oculus runtime configuration utility)", 
			L"Error, No Oculus HDM found!", 
			MB_ICONERROR);
#endif
		ovr_Shutdown();
		Annwvyn::AnnDebug("Unable to get a valid HMD. Closing program and returning 0xDEAD60D error");
		delete Ogre::Root::getSingletonPtr();
		exit(ANN_ERR_CRITIC);
	}
	hmdDesc = ovr_GetHmdDesc(hmd);

	//This is useless with OVR 0.8.0.0
	/*r = ovr_ConfigureTracking(hmd, //Oculus HMD
		ovrTrackingCap_Orientation |ovrTrackingCap_MagYawCorrection |ovrTrackingCap_Position, //Wanted capacities 
		0); //minial required */

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
		ovr_Destroy(getSession());
	ovr_Shutdown();
	Annwvyn::AnnDebug("LibOVR Shutdown... No longer can comunicate with OculusService or oculusd...");
}

void OculusInterface::customReport()
{
	Annwvyn::AnnDebug() << "================================================";
	Annwvyn::AnnDebug() << "Detected Oculus Rift device :";
	Annwvyn::AnnDebug() << "Product name : " << hmdDesc.ProductName;
	Annwvyn::AnnDebug() << "Serial number : " << hmdDesc.SerialNumber;
	Annwvyn::AnnDebug() << "Manufacturer : " << hmdDesc.Manufacturer;
	Annwvyn::AnnDebug() << "Display Resolution : " << hmdDesc.Resolution.w << "x" << hmdDesc.Resolution.h;
	Annwvyn::AnnDebug() << "Type of HMD identifier : " << hmdDesc.Type;
	Annwvyn::AnnDebug() << "Firmware version : " << hmdDesc.FirmwareMajor << "." << hmdDesc.FirmwareMinor;
	Annwvyn::AnnDebug() << "================================================";
}

void OculusInterface::update(double time)
{
	if(!initialized) return;
	firstUpdated = true;
	ss = ovr_GetTrackingState(hmd, time, true);
}
/*
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
*/
ovrHmdDesc OculusInterface::getHmdDesc()
{
	return hmdDesc;
}

ovrSession OculusInterface::getSession()
{
	return hmd;
}

/*
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
*/