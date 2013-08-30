#ifndef ANNTRIGGEROBJECT
#define ANNTRIGGEROBJECT

//windows DLL
#ifdef DLLDIR_EX
   #define DLL  __declspec(dllexport)   // export DLL information
#else
   #define DLL  __declspec(dllimport)   // import DLL information
#endif

//bypass on linux
#ifdef __gnu_linux__
#define DLL
#endif

#include <Ogre.h>

namespace Annwvyn
{
	class DLL AnnTriggerObject
	{
	public:
		AnnTriggerObject();

		void setPosition(Ogre::Vector3 pos);
		void setPosition(float x, float y, float z);

		Ogre::Vector3 getPosition();

		bool getContactInformation();
		void setContactInformation(bool contact);
		float getThreshold();

	private:
		Ogre::Vector3 m_position;
		float m_threshold;
		bool m_contactWithPlayer;
	
	};
}

#endif