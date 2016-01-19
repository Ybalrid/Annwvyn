/*!
* \file AnnTypes.h
* \brief Data types definition for Annwvyn
* \author Arthur Brainville
* \version 0.0.2
*/

#ifndef ANN_XML_LEVEL
#define ANN_XML_LEVEL
#include "systemMacro.h"
#include "AnnAbstractLevel.hpp"
#include "tinyxml2.h"
#include <string>

namespace Annwvyn
{
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