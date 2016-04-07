/**
* \file AnnVect3.hpp
* \brief A 3D Vector
* \author A. Brainville (Ybalrid) 
*/

#ifndef ANNVECT3
#define ANNVECT3
#include "systemMacro.h"
#include <OgreVector3.h>
#include <LinearMath/btVector3.h>

namespace Annwvyn
{
	///A 3D Vector
	class DLL AnnVect3 : public Ogre::Vector3
	{
		public:
			//Call of Ogre::Vector3 constructors 
			 AnnVect3();
			 AnnVect3(const Ogre::Vector3& v);
			 AnnVect3(const float cx, const float cy, const float cz);
			 AnnVect3(const int coord[3]);
			 AnnVect3(const float coord[3]);	
			 AnnVect3(float* const r);
			 AnnVect3(const float s);
			 AnnVect3(bool validState);
			
			//Custom Annwvyn part
			AnnVect3(btVector3& v);
			btVector3 getBtVector();
			 
			bool isValid();
	private:
		void init();
		bool valid;
	};
}

#endif //ANNVECT3
