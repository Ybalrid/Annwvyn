#ifndef ANN_XML_LEVEL
#define ANN_XML_LEVEL
#include "AnnAbstractLevel.hpp"
#include "tinyxml2.h"
#include <string>

namespace Annwvyn
{
	class AnnXmlLevel : LEVEL
	{
	public:
		AnnXmlLevel(std::string path);

		//load the level by reading the XML file
		void load();
		void runLogic();

	private:
		std::string xmlFilePath;
		bool resourceLocAdded;
	};
}

#endif //ANN_XML_LEVEL