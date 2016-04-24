#include "stdafx.h"
#include "AnnXmlLevel.hpp"
#include "AnnLogger.hpp"

using namespace tinyxml2;
using namespace Annwvyn;

inline phyShapeType AnnXmlLevel::getShapeTypeFromString(std::string str)
{
	if (str == "static")
		return staticShape;
	if (str == "convex")
		return convexShape;
	if (str == "box")
		return boxShape;
	if (str == "cylinder")
		return cylinderShape;
	if (str == "capsule")
		return capsuleShape;
	if (str == "sphere")
		return sphereShape;
	return phyShapeType(0);
}

AnnXmlLevel::AnnXmlLevel(std::string path) : constructLevel(),
	xmlFilePath(path),
	resourceLocAdded(false)
{
}

void AnnXmlLevel::load()
{
	//Get the parent directory of the file (all file path are in "unix style")
	std::string dirPath;
	const size_t last_slash = xmlFilePath.rfind('/');
	if(std::string::npos != last_slash) dirPath = xmlFilePath.substr(0, last_slash);
	AnnDebug() << "Working directory of the level file : " << dirPath;

	//Start reading the XML file
	XMLDocument xmlInFile;
	//open the file
	if(xmlInFile.LoadFile(xmlFilePath.c_str()) != XML_SUCCESS) 
	{
		AnnDebug() << "Cant load XML level : " << xmlFilePath;
		exit(ANN_ERR_INFILE);
	}
	AnnDebug() << "XML Level : " << xmlFilePath << " loaded on XML parser";

	//get the root node of the XML DOM
	XMLNode* level(xmlInFile.FirstChild());
	if(!level)
	{
		AnnDebug() << "Cant get 1st XML Node from " << xmlFilePath;
		exit(ANN_ERR_INFILE);
	}

	//Get the name of the level
	XMLElement* element(level->FirstChildElement("Name"));
	if(!element)
	{
		AnnDebug() << "Cant get Level name from " << xmlFilePath;
		exit(ANN_ERR_INFILE);
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
					Ogre::ResourceGroupManager::getSingleton().addResourceLocation(dirPath + "/" + path, type, name);
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
		exit(ANN_ERR_INFILE);
	}

	XMLElement* gameObject = element->FirstChildElement("Object");
	if(gameObject) do //Iterate through all game objects 
	{
		std::string entityName;
		AnnDebug() << "Fond object to load";
		float x, y, z, w;
		std::string ID(gameObject->Attribute("ID"));
		AnnDebug() << "Registred ID : " << ID;
		XMLElement* gameObjectData = gameObject->FirstChildElement("Entity");
		if(gameObjectData)
		entityName = (gameObjectData->Attribute("EntityName"));
		
		AnnGameObject* constructedGameObject;
		if(!ID.empty() && !entityName.empty()) constructedGameObject = addGameObject(entityName, ID);
		
		gameObjectData = gameObject->FirstChildElement("Position");
		if(gameObjectData)
		{
			gameObjectData->QueryFloatAttribute("X", &x);
			gameObjectData->QueryFloatAttribute("Y", &y);
			gameObjectData->QueryFloatAttribute("Z", &z);
			AnnDebug() << "Object at position : " << "(" << x << "," << y << "," << z << ")";
			constructedGameObject->setPosition(x, y, z);
		}

		gameObjectData = gameObject->FirstChildElement("Orientation");
		if(gameObjectData)
		{
			gameObjectData->QueryFloatAttribute("X", &x);
			gameObjectData->QueryFloatAttribute("Y", &y);
			gameObjectData->QueryFloatAttribute("Z", &z);
			gameObjectData->QueryFloatAttribute("W", &w);
			constructedGameObject->setOrientation(w, x, y, z);
		}

		gameObjectData = gameObject->FirstChildElement("Scale");
		if(gameObjectData)
		{
			gameObjectData->QueryFloatAttribute("X", &x);
			gameObjectData->QueryFloatAttribute("Y", &y);
			gameObjectData->QueryFloatAttribute("Z", &z);
			constructedGameObject->setScale(x, y, z);
		}
	
		XMLElement* physics = gameObject->FirstChildElement("Physics");
		if(!physics) continue; //no physics section. not mandatory. just ignore
		XMLElement* state = physics->FirstChildElement("Enabeled");
		if(!state) continue;
		bool phy; state->QueryBoolText(&phy); if(!phy) continue;

		float mass(0); std::string shape;

		XMLElement* phyInfo = physics->FirstChildElement("Mass");
		if(!phyInfo) continue;
		phyInfo->QueryFloatText(&mass);
		phyInfo = nullptr;

		phyInfo = physics->FirstChildElement("Shape");
		if(!phyInfo) continue;
		shape = phyInfo->GetText();
		constructedGameObject->setUpPhysics(mass, getShapeTypeFromString(shape));

		levelContent.push_back(constructedGameObject);
		
	} while(gameObject = gameObject->NextSiblingElement());
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

			AnnLightObject* lightSource = addLightObject();
			lightSource->setPosition(AnnVect3(x, y, z));
		}while (source = source->NextSiblingElement());
	}

	element = level->FirstChildElement("Player");
	if(element)
	{
		XMLElement* playerElement = element->FirstChildElement("Position");
		if(playerElement)
		{
			float x,y,z;
			playerElement->QueryFloatAttribute("X",&x);
			playerElement->QueryFloatAttribute("Y",&y);
			playerElement->QueryFloatAttribute("Z",&z);
			AnnDebug() << "Player starting position : (" << x << ", " << y << ", " << z << ")";

			AnnEngine::Instance()->getPlayer()->setPosition(AnnVect3(x,y,z));
		} else AnnEngine::Instance()->getPlayer()->setPosition(DEFAULT_STARTING_POS);

		playerElement = element->FirstChildElement("Orientation");
		if(playerElement)
		{
			float yaw;
			playerElement->QueryFloatAttribute("Yaw", &yaw);
			AnnDebug() << "Player Yaw : " << yaw;
			AnnEngine::Instance()->getPlayer()->setOrientation(Ogre::Euler(Ogre::Degree(yaw).valueRadians()));
		} else AnnEngine::Instance()->getPlayer()->setOrientation(DEFAULT_STARTING_ORIENT);
	}
	else
	{
		AnnEngine::Instance()->getPlayer()->setPosition(DEFAULT_STARTING_POS);
		AnnEngine::Instance()->getPlayer()->setOrientation(DEFAULT_STARTING_ORIENT);
	}
	AnnEngine::Instance()->resetPlayerPhysics();
}

void AnnXmlLevel::runLogic(){}
