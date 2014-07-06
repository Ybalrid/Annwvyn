#ifndef ANN_JOYSTICK_CONTROLLER
#define ANN_JOYSTICK_CONTROLLER

#undef DLL
///windows DLL
#ifdef DLLDIR_EX
#define DLL  __declspec(dllexport)   /// export DLL information
#else
#define DLL  __declspec(dllimport)   /// import DLL information
#endif

///bypass on linux
#ifdef __gnu_linux__
#undef DLL
#define DLL
#endif

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

            void setWalkAxis(int axisID = 1);
            void setStraffAxis(int axisID = 0);
            void setRotateAxis(int axisID = 2);

            void setReverseWalk(bool state = true);
            void setReverseStraff(bool state = false);
            void setReverseRotate(bool state = true);

            void setDeadzone(float threshold = 0.15);

            void update();
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
