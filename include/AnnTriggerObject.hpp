#ifndef ANNTRIGGEROBJECT
#define ANNTRIGGEROBJECT

#include "systemMacro.h"

#include <Ogre.h>


namespace Annwvyn
{
    ///Anticipated declaration of AnnEngine class 
    class AnnEngine;
    
    class DLL AnnTriggerObject
    {
        public:
            ///Class constructor
            AnnTriggerObject();

			///Class destructor
			virtual ~AnnTriggerObject(){}

            ///Set position form Vector 3D
            void setPosition(Ogre::Vector3 pos);

            ///Set position form Variables
            void setPosition(float x, float y, float z);
            
            ///Set contact information
            void setThreshold(float threshold);

            ///Get position
            Ogre::Vector3 getPosition();

            ///Get contact information
            bool getContactInformation();

            ///GetThreshold distance
            float getThreshold();

        private:	
            ///For engine : Set contact state 
            void setContactInformation(bool contact);
            
            ///Make AnnEngine class friend to permit acces to setContactInformation(bool)
            friend class Annwvyn::AnnEngine;

        private:
			///Position of the object
            Ogre::Vector3 m_position;
			///Distance where the trigger is triggered
            float m_threshold;
			///True if trigger triggerd
            bool m_contactWithPlayer;

        public:
            ///When contact happened
            virtual void atContact() {return;}
			///After initialization
            virtual void postInit() {return;}
    };
}

#endif


