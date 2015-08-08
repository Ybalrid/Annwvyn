#include "stdafx.h"
#include "AnnXmlLevel.hpp"
#include "tinyxml2.h"
#include "AnnLogger.hpp"

using namespace tinyxml2;
using namespace Annwvyn;

AnnXmlLevel::AnnXmlLevel(std::string path) : constructLevel(),
	xmlFilePath(path),
	resourceLocAdded(false)
{
}

void AnnXmlLevel::load()
{
	XMLDocument xmlInFile;
	//open the file
	if(xmlInFile.LoadFile(xmlFilePath.c_str()) != XML_SUCCESS) 
	{
		AnnDebug() << "Cant load XML level : " << xmlFilePath;
		abort();
	}
	
	AnnDebug() << "XML Level : " << xmlFilePath << " loaded on XML parser";

	//get the root node of the XML DOM
	XMLNode* level(xmlInFile.FirstChild());
	if(!level)
	{
		AnnDebug() << "Cant get 1st XML Node from " << xmlFilePath;
		abort();
	}

	//Get the name of the level
	XMLElement* element(level->FirstChildElement("Name"));
	if(!element)
	{
		AnnDebug() << "Cant get Level name from " << xmlFilePath;
		abort();
	}
	name = element->GetText();
	AnnDebug() << "Name of level : " << name;
	AnnDebug() << "This will be the resource group name for Level Specific resource location declaration";
	
	//Add resource location to the Ogre Resource Group Manager
	if(!resourceLocAdded)
	{
		element = level->FirstChildElement("ResourceLocations");
		if(!element)
		{
			AnnDebug() << xmlFilePath << "Does not apear to have a 'ResourceLocations' section";
		}
		else
		{
			XMLElement* resourceLocation = element->FirstChildElement("ResourceLocation");
			if(resourceLocation) do
			{
				std::string type(resourceLocation->Attribute("Type")), path(resourceLocation->Attribute("Path"));
				if(!type.empty() && !path.empty())
					Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path, type, name);
			}while(resourceLocation = resourceLocation->NextSiblingElement());
			Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(name);
		resourceLocAdded = true;
		}
	}

	//Get level content
	element = level->FirstChildElement("LevelContent");
	
	if(!element)
	{
		AnnDebug() << xmlFilePath << "Don't have a 'LevelContent' section. This mean the level can't be loaded";
		abort();
	}

	XMLElement* gameObject = level->FirstChildElement("Object");
	if(gameObject) do //Iterate through all game objects 
	{
		float x, y, z, w;
		std::string ID(gameObject->Attribute("ID"));
		std::string entityName(gameObject->Attribute("EntityName"));
		
		AnnGameObject* constructedGameObject;
		if(!ID.empty() && !entityName.empty()) constructedGameObject = addGameObject(entityName, ID);
		
		XMLElement* gameObjectData = gameObject->FirstChildElement("Position");
		if(gameObjectData)
		{
			gameObjectData->QueryFloatAttribute("X", &x);
			gameObjectData->QueryFloatAttribute("Y", &y);
			gameObjectData->QueryFloatAttribute("Z", &z);
			constructedGameObject->setPos(x, y, z);
		}

		gameObjectData = gameObject->FirstChildElement("Orientation");
		if(!gameObjectData)
		{
			gameObjectData->QueryFloatAttribute("X", &x);
			gameObjectData->QueryFloatAttribute("Y", &y);
			gameObjectData->QueryFloatAttribute("Z", &z);
			gameObjectData->QueryFloatAttribute("W", &w);
			constructedGameObject->setOrientation(w, x, y, z);
		}

		gameObjectData = gameObject->FirstChildElement("Scale");
		if(!gameObjectData)
		{
			gameObjectData->QueryFloatAttribute("X", &x);
			gameObjectData->QueryFloatAttribute("Y", &y);
			gameObjectData->QueryFloatAttribute("Z", &z);
			constructedGameObject->setPos(x, y, z);
		}
	
		continue; //Don't import physics yet...
		XMLElement* physics = gameObject->FirstChildElement("Physics");
		if(!physics) continue; //no physics section. not mandatory. just ignore
		XMLElement* state = physics->FirstChildElement("Enabeled");
		if(!state) continue;
		bool phy; state->QueryBoolText(&phy); if(!phy) continue;
		
	} while(gameObject = level->NextSiblingElement());
	else AnnDebug() << "No objects declared to load.";

	element = level->FirstChildElement("LevelLighting");
	if(!element) AnnDebug() << "No lights declared";
	else
	{
		float x,y,z;
		XMLElement* source = element->FirstChildElement("Source");
		if(source) do
		{
			source->QueryFloatAttribute("X", &x);
			source->QueryFloatAttribute("Y", &y);
			source->QueryFloatAttribute("Z", &z);

			AnnLightObject* lightSource = addLight();
			lightSource->setPosition(x, y, z);
		}while (source = element->NextSiblingElement());
	}

}

void AnnXmlLevel::runLogic(){}
