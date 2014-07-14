#include "OculusInterface.hpp"

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
    try
    {
        ovr_Initialize();
        hmd = ovrHmd_Create(0);

        if(hmd)
            ovrHmd_GetDesc(hmd, &hmdDesc);
        else
            throw 0;
    }
    catch(int e)
    {
        cout << "Cannot get HMD" << endl;
        //for now.
//        initialized = false; 
//        return;

        //TODO replace content of this exeption catch by creating a virtual debug HMD to run correctly

        hmd = ovrHmd_CreateDebug(ovrHmd_DK2);
        ovrHmd_GetDesc(hmd, &hmdDesc);
    }

    customReport();
    
    try
    {
        if(!ovrHmd_StartSensor(hmd,ovrSensorCap_Orientation |ovrSensorCap_YawCorrection |ovrSensorCap_Position,ovrSensorCap_Orientation)) //minial required 
            throw string("Unable to start sensor! The detected device by OVR is not capable to get sensor state. We cannot do anything with that...");
    }
    catch (string const& e)
    {
        cerr << e << endl;
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
    cout << endl << "Manual Information access :" << endl;
    cout << "Product name : " << hmdDesc.ProductName << endl
        << "Manufacturer : " << hmdDesc.Manufacturer << endl
        << "Sensor Capability bits : " << (hmdDesc.SensorCaps) << endl
        << "Distortion Capability bits" << (hmdDesc.DistortionCaps) << endl   
        << "Display Resolution : " << hmdDesc.Resolution.w << "x" << hmdDesc.Resolution.h << endl 
        << "Type of HMD identifier : " << hmdDesc.Type << endl
        << "HMD 2D virtual position : " << hmdDesc.WindowsPos.x << "x" << hmdDesc.WindowsPos.y << endl;
}

void OculusInterface::update(double time)
{
    if(!initialized) return;
    firstUpdated = true;
    ss = ovrHmd_GetSensorState(hmd, time);
}

OVR::Vector3f OculusInterface::getPosition()
{
    if(initialized && firstUpdated)
        return OVR::Vector3f(ss.Predicted.Pose.Position);
    return OVR::Vector3f(0,0,0);
}

OVR::Quatf OculusInterface::getOrientation()
{
    if(initialized && firstUpdated)
        return OVR::Quatf(ss.Predicted.Pose.Orientation);
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
