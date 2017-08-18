#include "stdafx.h"
#ifdef _WIN32
#include "OculusInterface.hpp"
#include "AnnLogger.hpp"
#include "AnnException.hpp"

using namespace std;
using namespace OVR;
using namespace Annwvyn;

void OculusInterface::abortOnFailure()
{
	session = {};
	luid = {};
	hmdDesc = {};

	//Notify user
	AnnDebug() << "Error: Cannot create Oculus Session";
	//Debug HMD is now handled by the configuration utility and the runtime.
	AnnDebug() << "Please make sure Oculus Home is installed on your system and "
		"please check if you have correctly plugged HDMI and USB on the Rift and Tracker";

	displayWin32ErrorMessage(L"Error: Cannot create Oculus Session!",
		L"Please make sure Oculus Home is installed on your system\n"
		L"and check HDMI and USB connection to your Rift and Tracker");

	//Cleanup
	ovr_Shutdown();
	//Return an error
	AnnDebug("Unable to get a session from the Oculus Runtime. Closing program and returning 0xDEAD60D error");
	//Stop program
	throw AnnInitializationError((ANN_ERR_CRITIC), "Unable to create an Oculus session");
}

OculusInterface::OculusInterface()
{
	AnnDebug() << "Init Oculus Interface object";
	//Init Oculus Virtual Reality library
	ovr_Initialize(nullptr);

	//Declare this client to the Oculus service
	stringstream clientIentifier;
	clientIentifier << "EngineName: Annwvyn\n";
	clientIentifier << "EngineVersion: " << AnnEngine::getAnnwvynVersion();
	AnnDebug() << "Identifier string sent to the Oculus Service : \n" << clientIentifier.str();
	ovr_IdentifyClient(clientIentifier.str().c_str());

	//Attempt to create OVR session
	if (ovr_Create(&session, &luid) != ovrSuccess)
		abortOnFailure();

	//Fill the hmdDesc structure
	hmdDesc = ovr_GetHmdDesc(session);

	//Print to log all known information about the headset
	customReport();
}

OculusInterface::~OculusInterface()
{
	//Set the performance HUD to Off
	ovr_SetInt(getSession(), "PerfHudMode", ovrPerfHud_Off);

	AnnDebug() << "Shutdown OculusInterface object";
	ovr_Destroy(getSession());
	ovr_Shutdown();
	AnnDebug("LibOVR Shutdown... No longer can communicate with OculusService");
}

void OculusInterface::customReport() const
{
	//Print to the logger a bunch of information
	AnnDebug() << "========================================================";
	AnnDebug() << "OVR version " << ovr_GetVersionString();
	AnnDebug() << "Detected the following Oculus Rift VR Headset :";
	AnnDebug() << "Product name : " << hmdDesc.ProductName;
	AnnDebug() << "Serial number : " << hmdDesc.SerialNumber;
	AnnDebug() << "Manufacturer : " << hmdDesc.Manufacturer;
	AnnDebug() << "Display Resolution : " << hmdDesc.Resolution.w << "x" << hmdDesc.Resolution.h;
	AnnDebug() << "Display refresh rate : " << hmdDesc.DisplayRefreshRate << "Hz";
	AnnDebug() << "Type of HMD identifier : " << hmdDesc.Type;
	AnnDebug() << "Firmware version : " << hmdDesc.FirmwareMajor << "." << hmdDesc.FirmwareMinor;
	AnnDebug() << "========================================================";
}

ovrHmdDesc OculusInterface::getHmdDesc() const
{
	return hmdDesc;
}

ovrSession OculusInterface::getSession() const
{
	return session;
}

#endif