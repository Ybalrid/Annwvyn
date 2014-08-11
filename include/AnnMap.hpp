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
    class DLL AnnMap
    {
        public:
            AnnMap(Annwvyn::AnnEngine* engine);
            AnnMap(Annwvyn::AnnEngine* engine, const char mapFile[]);
            ~AnnMap();
            void loadFile(const char mapFile[]);
            
            AnnGameObjectVect getObjects(){return content;}
			AnnLightVect getLights(){return contentLights;}

        private:
            void init(Annwvyn::AnnEngine* engine);
            void process(std::string descLine);

            AnnEngine* m_engine;
            AnnGameObjectVect content;
            AnnLightVect contentLights;
            
            bool fileLoaded;
            
            AnnGameObject* tmpObject;
            AnnLightObject* tmpLight;
    };
}
#endif //ANN_MAP 
