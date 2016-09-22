/**
 * \file OculusInterface.hpp
 * \brief Simple class for accessing Oculus Rift data
 * \author A. Brainville (Ybalrid)
 */

#ifndef OCULUS_INTERFACE 
#define OCULUS_INTERFACE 

#include <iostream>

//Oculus VR API
//#include <OVR.h>
#include <OVR_CAPI.h>
#include <Extras/OVR_Math.h>
#include "systemMacro.h"
#include "AnnErrorCode.hpp"
#include <Ogre.h>
#define USE_OGRE


///Comunicate with the Rift runtime (initialize OVR and get the info)
class DLL OculusInterface
{
	
public:

	enum OculusInterfaceEyeSide
	{
		left,
		right
	};

	///Construct an OculusInterface object. This create an Oculus Session for comunication with the Oculus Runtime
	OculusInterface();

	///Destructor of Oculus Interface
	~OculusInterface();

	///Return the active hmd desk object
	ovrHmdDesc getHmdDesc();

	///Return the current oculus session
	ovrSession getSession();

	///Initialize the layer
	void initOculusLayer();

private:
	///Print every known characteristics about the hardware to the log output
	void customReport();

	///Oculus session for the application
	ovrSession session;

	///Description of the HMD
	ovrHmdDesc hmdDesc;

	///Graphics device identifier (internal for the Rift SDK)
	ovrGraphicsLuid luid;

};
#endif
