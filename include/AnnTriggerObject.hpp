#ifndef ANNTRIGGEROBJECT
#define ANNTRIGGEROBJECT

#include <Ogre.h>

namespace Annwvyn
{
	class AnnTriggerObject
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