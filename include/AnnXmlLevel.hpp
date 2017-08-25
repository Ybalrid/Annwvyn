/*!
* \file AnnXmlLevel.hpp
* \brief Load a level from an XML file
* \author A. Brainville (Ybalrid)
*/

#ifndef ANN_XML_LEVEL
#define ANN_XML_LEVEL
#include "systemMacro.h"
#include "AnnLevel.hpp"
#include <tinyxml2.h>
#include "AnnTypes.h"
#include <string>

namespace Annwvyn
{
	///A level loaded from an XML file
	class AnnDllExport AnnXmlLevel : public AnnLevel
	{
	public:
		static phyShapeType getShapeTypeFromString(std::string str);
		///Construct an AnnXmlLevel
		/// \param path
		AnnXmlLevel(std::string path);

		///load the level by reading the XML file
		void load() override;

		///Run the logic on this level. You can overload this at will ! *\o/*
		void runLogic() override;

	private:
		///Path of the XML file
		std::string xmlFilePath;

		///Resource location already added to the resource group manager
		bool resourceLocAdded;
	};
}

#endif //ANN_XML_LEVEL