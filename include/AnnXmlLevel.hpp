/*!
* \file AnnXmlLevel.hpp
* \brief Load a level from an XML file
* \author A. Brainville (Ybalrid)
*/

#ifndef ANN_XML_LEVEL
#define ANN_XML_LEVEL
#include "systemMacro.h"
#include "AnnAbstractLevel.hpp"
#include "tinyxml2.h"
#include <string>

namespace Annwvyn
{
	///A level loaded from an XML file
	class DLL AnnXmlLevel : public AnnAbstractLevel
	{
	public:
		AnnXmlLevel(std::string path);

		//load the level by reading the XML file
		void load();
		virtual void runLogic();

	private:
		std::string xmlFilePath;
		bool resourceLocAdded;
	};
}

#endif //ANN_XML_LEVEL