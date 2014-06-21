#ifndef ANNTRIGGEROBJECT
#define ANNTRIGGEROBJECT
#undef DLL
//windows DLL
#ifdef DLLDIR_EX
#define DLL  __declspec(dllexport)   // export DLL information
#else
#define DLL  __declspec(dllimport)   // import DLL information
#endif

//bypass on linux
#ifdef __gnu_linux__
#undef DLL
#define DLL
#endif

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
            Ogre::Vector3 m_position;
            float m_threshold;
            bool m_contactWithPlayer;

        public:
            ///When contact happend
            virtual void atContact() {return;}
            virtual void postInit() {return;}
    };
}

#endif


