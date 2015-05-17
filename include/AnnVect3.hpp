#ifndef ANNVECT3
#define ANNVECT3
#include "systemMacro.h"
#include <OgreVector3.h>
#include <LinearMath\btVector3.h>

namespace Annwvyn
{
	class DLL AnnVect3 : public Ogre::Vector3
	{
		public:
			//Call of Ogre::Vector3 constructors 
			explicit AnnVect3();
			explicit AnnVect3(const float cx, const float cy, const float cz);
			explicit AnnVect3(const int coord[3]);
			explicit AnnVect3(const float coord[3]);	
			explicit AnnVect3(float* const r);
			explicit AnnVect3(const float s);
			
			//Custom Annwvyn part
			AnnVect3(btVector3& v);
			btVector3 getBtVector();
	};
}

#endif //ANNVECT3