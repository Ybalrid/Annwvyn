/*!
* \file AnnXmlLevel.hpp
* \brief Load a level from an XML file
* \author A. Brainville (Ybalrid)
*/

#ifndef ANN_XML_LEVEL
#define ANN_XML_LEVEL
#include "systemMacro.h"
#include "AnnLevel.hpp"
#include "tinyxml2.h"
#include "AnnTypes.h"
#include <string>

namespace Annwvyn
{
	///A level loaded from an XML file
	class DLL AnnXmlLevel : public AnnLevel
	{
	public:
		static phyShapeType getShapeTypeFromString(std::string str);
		///Construct an AnnXmlLevel 
		/// \param path
		AnnXmlLevel(std::string path);

		///load the level by reading the XML file
		void load();

		///Run the logic on this level. You can overload this at will ! *\o/*
		virtual void runLogic();

	private:
		///Path of the xml file
		std::string xmlFilePath;

		///Resource location allready added to the resource group manager
		bool resourceLocAdded;
	};
}

#endif //ANN_XML_LEVEL