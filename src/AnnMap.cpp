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
    //Dessalocate the map.
    //Since all theses objects are dessalocated from the AnnEngine class we have 2 chices :
    //1) Leave them allocated and let AnnEngine clean memory after us
    //2) Supress object from the scene.
    //
    //Since the engine is pretty limited and dont permit to remove object from a scene, we cannot use solution n°2.
    //
    //Therfore we temporary use solution n°1 ... 
    //So in fact we do nothing here

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
    fileLoaded = true;
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
    tmpLight = NULL;
}

void AnnMap::process(std::string descLine)
{
    stringstream desc(descLine);
    string word;

    float x,y,z,w,mass;
    while(desc >> word)
    {
        //put to zero all buffer variables
        x = 0;
        y = 0;
        z = 0;
        w = 1;
        mass = 0;

        if(word == "Object") //Supose we give an entity name here.
        {
            desc >> word;
            if(tmpObject)
            {
                //tmpObject not NULL : syntax error on the current object construction :
                //We ask to create a new object but we are already creating one. Further description 
                //act for the new object. We have to abort the current object construction because it's an
                //unfinished one.
                

                //There is a problem here : We haven't finish to add an object to the map. But the file description
                //ask to create a new object. At this stade, the object is allready on the scene. If we drop that object
                //and free the memory, the AnnEngine class know a pointer to that memory adress, and will try to access it.
                //We need to destroy that object via the AnnEngine class. But that fonctionality doesn't exist (yet)
                //We will just asume for the moment that we push_back the object to the Map Vector.
                //This behaviour is not documented and not normal. But that the only temporary thing we can 
                //avoid the rist to cause a future segfault
                
                content.push_back(tmpObject);
                tmpObject = NULL;

                //normal thing to do : 
                //  remove from game engine the avorted object
                // delete tmpObject;
                // tmpObject = NULL;
                //
                //!!will be patched later !!
            }
                tmpObject = m_engine->createGameObject(word.c_str()); //try to create an AnnGameObject from that entity
        }
        
        else if(word == "Pos") //loading coorinates as 3 float 
        {
            desc >> x; 
            desc >> y;
            desc >> z;
            
            if(tmpLight)
            {
            }
            
            else if(tmpObject)
                tmpObject->setPos(x,y,z);
            else continue; //syntax error here
        }

        else if(word == "Orient")
        {
            desc >> x;
            desc >> y;
            desc >> z;
            desc >> w;

            if(tmpObject)
                tmpObject->setOrientation(w,x,y,z);
            else 
            {
            continue; //syntax error here 
            }
        }

        else if (word == "Scale")
        {
            desc >> x;
            desc >> y;
            desc >> z;
            if(tmpObject)
                tmpObject->node()->scale(x,y,z);
        }

        else if (word == "PhysicShape")
        {
            desc >> word;

            phyShapeType shape;
            if(word == "STATIC") 
                shape = staticShape;
            else if (word == "CONVEX")
                shape = convexShape;
            else if (word == "BOX")
                shape = boxShape;
            else if (word == "CYLINDER")
                shape = cylinderShape;
            else if (word == "CAPSULE")
                shape = capsuleShape;
            else continue; //abort physics if syntax error;

            std::cerr << shape;
            desc >> mass;
            
            if(tmpObject)
                tmpObject->setUpBullet(mass,shape);
        }
        
        else if(word == "EndObject") //End of description of an object
        {
            content.push_back(tmpObject); //Add that object form the scene description 
            tmpObject = NULL; //make that pointer available 
        }

        else if (word == "Light")
        {
           if(tmpLight)
            {
                //same thing, se =="Object" above...
                
            }
           
        }

        else continue;
    }
}
