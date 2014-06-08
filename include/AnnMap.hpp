#ifndef ANN_MAP
#define ANN_MAP

#undef DLL
//windows DLL
#ifdef DLLDIR_EX
#define DLL  __declspec(dllexport)   // export DLL information
#else
#define DLL  __declspec(dllimport)   // import DLL information
#endif

//bypass on linux
#ifdef __gnu_linux__
#undef DLL
#define DLL
#endif

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
            
            AnnGameObjectVect getContent(){return content;}

        private:
            void init(Annwvyn::AnnEngine* engine);
            void process(std::string descLine);

            AnnEngine* m_engine;
            AnnGameObjectVect content;

            std::vector<AnnLightObject*> Lights;
            
            bool fileLoaded;
            
            AnnGameObject* tmpObject;
            AnnLightObject* tmpLight;

    };
}
#endif //ANN_MAP 

