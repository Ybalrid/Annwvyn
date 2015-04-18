#include "OculusInterface.hpp"
/*Some of the commented code is commented because I'm to lazy to reimplement it, and it's not used inside of Annwvyn so... #useless*/

OculusInterface::OculusInterface()
{
    initialized = false;
    firstUpdated = false;
    init();
}

OculusInterface::~OculusInterface()
{
    shutdown();
}

void OculusInterface::init()
{
        ovr_Initialize();
        hmd = ovrHmd_Create(0);

        if(!hmd)
		{
			cout << "Cannot get HMD" << endl;
			hmd = ovrHmd_CreateDebug(ovrHmd_DK2);
		}

		customReport();

        if(!ovrHmd_ConfigureTracking(hmd, //Oculus HMD
			ovrTrackingCap_Orientation |ovrTrackingCap_MagYawCorrection |ovrTrackingCap_Position, //Wanted capacities 
			ovrTrackingCap_Orientation)) //minial required 
			{
				std::cerr << "Unable to start sensor! The detected device by OVR is not capable to get sensor state. We cannot do anything with that..." << std::endl;
				ovrHmd_Destroy(hmd);
				ovr_Shutdown();
				abort();
			}
    initialized = true;
}

void OculusInterface::shutdown()
{
    if(initialized)
        ovrHmd_Destroy(hmd);
    ovr_Shutdown();
}

void OculusInterface::customReport()
{
   /* cout << endl << "Manual Information access :" << endl;
    cout << "Product name : " << hmdDesc.ProductName << endl
        << "Manufacturer : " << hmdDesc.Manufacturer << endl
        << "Sensor Capability bits : " << (hmdDesc.SensorCaps) << endl
        << "Distortion Capability bits" << (hmdDesc.DistortionCaps) << endl   
        << "Display Resolution : " << hmdDesc.Resolution.w << "x" << hmdDesc.Resolution.h << endl 
        << "Type of HMD identifier : " << hmdDesc.Type << endl
        << "HMD 2D virtual position : " << hmdDesc.WindowsPos.x << "x" << hmdDesc.WindowsPos.y << endl;*/
}

void OculusInterface::update(double time)
{
    if(!initialized) return;
    firstUpdated = true;
    ss = ovrHmd_GetTrackingState(hmd, time);
}

OVR::Vector3f OculusInterface::getPosition()
{
	/*
    if(initialized && firstUpdated)
        return OVR::Vector3f(ss.HeadPose.ThePose.Orientation);*/
	//TODO : get real data
    return OVR::Vector3f(0,0,0);
}

OVR::Quatf OculusInterface::getOrientation()
{
	/*
    if(initialized && firstUpdated)
        return OVR::Quatf(ss.Predicted.Pose.Orientation);*/
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
