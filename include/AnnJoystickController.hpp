#ifndef ANN_JOYSTICK_CONTROLLER
#define ANN_JOYSTICK_CONTROLLER


#include "systemMacro.h"

#include <iostream>
#include <fstream>

#include "AnnEngine.hpp"
#include "AnnTypes.h"
#include "euler.h"
using namespace std;
using namespace OIS;
using namespace Annwvyn;

namespace Annwvyn
{
    class DLL AnnJoystickController
    {
        private: //datatypes 
            enum axisName {
                WALK,
                STRAFF,
                ROTATE
            };

        public:
            AnnJoystickController(AnnEngine* engine);

			///Set the waking axis. Walk is foward-backward
            void setWalkAxis(int axisID = 1);
			///Set straff axis. Straff is left-right
            void setStraffAxis(int axisID = 0);
			///Set rotate axis. Rotatie is clockwise-anticlockwise
            void setRotateAxis(int axisID = 2);
			
			///Reversing the axis ?
            void setReverseWalk(bool state = true);
			///Reversing the axis ?
            void setReverseStraff(bool state = false);
			///Reversing the axis ?
            void setReverseRotate(bool state = true);

			///Set the deadzone %
            void setDeadzone(float threshold = 0.15);

			///Update mouvement from joystick state
            void update();

			///Load a joystick configuration file 
			void loadConfigFile(const char path[]);
			///Save a joystick configuration file
			void saveConfigFile(const char path[]);

        private:

            //private methods :
            void updateJoystickValues();
            void updateVirtualBodyValues(); 
            float getAxisValue(int axisID);
            float trimToFitDeadzone(float value);
            void debug(void);
            //private atributes :
            AnnEngine* m_engine;
            //axes values
            float walk;
            float straff;
            float rotate;
        
            float deadzone;

            int axesID[3];
            bool reverse[3];

            int MAX;
            float abs(float value);
    };
}

#endif
