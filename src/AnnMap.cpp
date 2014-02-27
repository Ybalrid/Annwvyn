#include "AnnMap.hpp"

using namespace Annwvyn;
using namespace std;

AnnMap::AnnMap(AnnEngine* engine) :
    fileLoaded(false)
{
    init(engine);
}


AnnMap::AnnMap(AnnEngine* engine, const char mapFile[]) :
    fileLoaded(false)
{
    init(engine);
    loadFile(mapFile);
}

AnnMap::~AnnMap()
{
/*    for(size_t i = 0; i < content.size(); i++)
        delete content[i];*/
}

void AnnMap::loadFile(const char mapFile[])
{   
    ifstream file(mapFile);
    if(!file.is_open())
    {
        cerr << "Cannot open path " << mapFile << endl;
        exit(-1);
    }
    
    string lineBuffer;

    while(getline(file,lineBuffer))
        process(lineBuffer);

    file.close();
}

void AnnMap::init(AnnEngine* engine)
{
    if(!engine)
    {
        cerr << "AnnMap : invalid pointer or reference : engine" << endl;
        exit(-1);
    }
    m_engine = engine;

    tmpObject = NULL;

}

void AnnMap::process(std::string descLine)
{
    stringstream desc(descLine)
    string word;
    float x,y,z,w;
    while(desc >> word)
    {
        if(word == "Object")
        {
            desc >> word;
            tmpObject = m_engine->createGameObject(word);
        }
        
        else if(word == "Pos")
        {
            desc >> x;
            desc >> y;
            desc >> z;

            if(tmpObject)
                tmpObject->setPos(x,y,z);
            else continue;
        }

        else if(word == "EndObject")
        {
            content.push_back(tmpObject);
            tmpObject = NULL;
        }
        else continue;
    }
}
