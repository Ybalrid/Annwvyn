#include "AnnJoystickController.hpp"
using namespace Annwvyn;

/**
 * Axes are called WALK, STRAFF and ROTATE.
 * They corespond to the level of each of theses 3 movement.
 * Each axes can be reversed if your controller need it.
 * Each axes is configurable, you can specify any axis ID your controller have (int)
 * Please make sure the controller ID is a valid Axis before setting it here. 
 * The only security check made by this class is to verify if a joystick is plugged.
 */

AnnJoystickController::AnnJoystickController(AnnEngine* engine)
{
    m_engine = engine;
    
    setWalkAxis();
    setStraffAxis();
    setRotateAxis();
    
    setDeadzone();

    reverse[WALK] = true;
    reverse[STRAFF] = false;
    reverse[ROTATE] = true;

    if(m_engine->getOISJoyStick())
        MAX = m_engine->getOISJoyStick()->MAX_AXIS;
}

//setters to change axis used for each action
void AnnJoystickController::setWalkAxis(int axisID)
{
    axesID[WALK] = axisID;
}
void AnnJoystickController::setStraffAxis(int axisID)
{
    axesID[STRAFF] = axisID;
}
void AnnJoystickController::setRotateAxis(int axisID)
{
    axesID[ROTATE] = axisID;
}

//setters to tell if you have to reverse each axis
void AnnJoystickController::setReverseWalk(bool state)
{
    reverse[WALK] = state;
}

void AnnJoystickController::setReverseStraff(bool state)
{
    reverse[STRAFF] = state;
}

void AnnJoystickController::setReverseRotate(bool state)
{
    reverse[ROTATE] = state;
}


//update the joystick controll
void AnnJoystickController::update()
{
    //check if joystick exist
    if(!m_engine->getOISJoyStick()) return; //if no joystick, cut here
    
    //update values from sticks
    updateJoystickValues();

    //update body speeds from internal values
    updateVirtualBodyValues();
}


//Get the axis value. float between -1 and 1 (aproximatly) 
float AnnJoystickController::getAxisValue(int axisID)
{
    return trimToFitDeadzone(static_cast<float>(m_engine->
            getOISJoyStick()->getJoyStickState().mAxes[
            axesID[axisID]
            ].abs)/static_cast<float>(MAX));
}

//updates valules
void AnnJoystickController::updateJoystickValues()
{
    //fetch each values
    walk = getAxisValue(WALK);
    straff = getAxisValue(STRAFF);
    rotate = getAxisValue(ROTATE);

    //apply reversement
    if(reverse[WALK]) walk *= -1;
    if(reverse[STRAFF]) straff *= -1;
    if(reverse[ROTATE]) rotate *= -1;

}

void AnnJoystickController::updateVirtualBodyValues()
{
    //get a pointer to the body
    bodyParams* virtualBody (m_engine->getBodyParams());

    //calculate speed form stick values
    float forward(virtualBody->walkSpeed * walk);
    float side(virtualBody->walkSpeed * straff);
    float turn(virtualBody->turnSpeed * rotate); //angular rad/sec

    //get the Y velocity of the body. we do not change that value
    float Yvel(virtualBody->Body->getLinearVelocity().y());
    
    //calculate new orientation (euler based)
    float yaw(virtualBody->Orientation.getYaw().valueRadians() + turn*m_engine->getTime()*1000);
    float pitch(virtualBody->Orientation.getPitch().valueRadians());
    float roll(virtualBody->Orientation.getRoll().valueRadians());

    //apply orientation
    virtualBody->Orientation = Ogre::Euler(yaw, pitch, roll);

    //calculate and apply the speed vector "V"
    m_engine->setPhysicBodyLinearSpeed(virtualBody->Orientation.toQuaternion()*Ogre::Vector3(side, Yvel, -forward));
}

//print stick value
void AnnJoystickController::debug()
{
    cerr << "WALK : " << walk << endl;
    cerr << "STRAFF : " << straff << endl;
    cerr << "ROTATE : " << rotate << endl;

}

//set stick deadzone (between 0 & 1)
void AnnJoystickController::setDeadzone(float threshold)
{
    if(threshold>= 0 && threshold <= 1)
    deadzone = threshold;
}

//replace value by 0 if too low (consirder axis unmouved)
float AnnJoystickController::trimToFitDeadzone(float value)
{
    if(abs(value) > deadzone) return value;
    return 0.0f;
}

//get absolute value
float AnnJoystickController::abs(float value)
{
    if(value < 0) return -value;
    return value;
}

void AnnJoystickController::loadConfigFile(const char path[])
{
	ifstream file(path);
	if(!file.is_open())
		return;
	string buffer;
	while(getline(file,buffer))
	{
		std::stringstream line(buffer);
		string word;
		string garbage;

		while(line >> word)
		{
			line >> garbage;
			int mapping;
			line >> mapping;
			if(word == "straffAxis")
			{
				setStraffAxis(mapping);
			}
			else if(word == "walkAxis")
			{
				setWalkAxis(mapping);
			}
			else if(word == "rotateAxis")
			{
				setRotateAxis(mapping);
			}
			else if(word == "straffReverse")
			{
				if(mapping)
					setReverseStraff(true);
				else
					setReverseStraff(false);
			}
			else if(word == "walkReverse")
			{
				if(mapping)
					setReverseWalk(true);
				else
					setReverseWalk(false);
			}
			else if(word == "rotateReverse")
			{
				if(mapping)
					setReverseRotate(true);
				else
					setReverseRotate(false);
			}
		}
	}

	file.close();
}

void AnnJoystickController::saveConfigFile(const char path[])
{
	//That's why I Love C++ streams : simplicity and clarity 

	ofstream file(path);
	if(!file.is_open())
		return;

	file << "straffAxis = " << axesID[STRAFF] << endl;
	file << "walkAxis = " << axesID[WALK] << endl;
	file << "rotateAxis = " << axesID[ROTATE] << endl;
	file << "straffReverse = " << reverse[STRAFF] << endl;
	file << "walkReverse = " << reverse[WALK] << endl;
	file << "rotateReverse = " << reverse[ROTATE] << endl;

	file.close();
}
