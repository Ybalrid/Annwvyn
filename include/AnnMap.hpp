#ifndef ANN_MAP
#define ANN_MAP


#include "systemMacro.h"

#include <iostream>
#include <fstream>
#include <vector>
#include "AnnEngine.hpp"
#include "AnnGameObject.hpp"
#include "AnnTypes.h"

namespace Annwvyn
{
	///Object that represent a map
    class DLL AnnMap
    {
        public:
			///Map loader base constructor
            AnnMap(Annwvyn::AnnEngine* engine);
			///Map loader that directly load a map file
            AnnMap(Annwvyn::AnnEngine* engine, const char mapFile[]);
			///Destroy the map
            ~AnnMap();
			///Load a map file (only once)
            void loadFile(const char mapFile[]);
            
			///Get the content of the map
            AnnGameObjectVect getObjects(){return content;}
			///Get the lights sources of the map
			AnnLightVect getLights(){return contentLights;}

        private:
			///Initalize the map object
            void init(Annwvyn::AnnEngine* engine);
			///Execute the command from a line of the map object
            void process(std::string descLine);

			///Pointer to the engine
            AnnEngine* m_engine;
			///Objects of the map
            AnnGameObjectVect content;
			///Lighting of the map
            AnnLightVect contentLights;
            
			///True if a map file is loaded
            bool fileLoaded;
            
			///Temporary object used while processing the map file
            AnnGameObject* tmpObject;
			///Same for a light source
            AnnLightObject* tmpLight;
    };
}
#endif //ANN_MAP 
