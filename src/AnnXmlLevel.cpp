#include "stdafx.h"
#include "AnnXmlLevel.hpp"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"
#include "AnnException.hpp"

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
	//Get the parent directory of the file (all file path are in "UNIX style")
	std::string dirPath;
	const auto last_slash = xmlFilePath.rfind('/');
	if (std::string::npos != last_slash) dirPath = xmlFilePath.substr(0, last_slash);
	AnnDebug() << "Working directory of the level file : " << dirPath;

	//Start reading the XML file
	XMLDocument xmlInFile;
	//open the file
	if (xmlInFile.LoadFile(xmlFilePath.c_str()) != XML_SUCCESS)
	{
		AnnDebug() << "Cant load XML level : " << xmlFilePath;
		throw AnnInitializationError(ANN_ERR_INFILE, "Error while reading XML Level file");
	}
	AnnDebug() << "XML Level : " << xmlFilePath << " loaded on XML parser";

	//get the root node of the XML DOM
	XMLNode* level(xmlInFile.FirstChild());
	if (!level)
	{
		AnnDebug() << "Cant get 1st XML Node from " << xmlFilePath;
		throw AnnInitializationError(ANN_ERR_INFILE, "Error while reading XML Level file");
	}

	//Get the name of the level
	XMLElement* element(level->FirstChildElement("Name"));
	if (!element)
	{
		AnnDebug() << "Cant get Level name from " << xmlFilePath;
		throw AnnInitializationError(ANN_ERR_INFILE, "Error while reading XML Level file");
	}
	name = element->GetText();
	AnnDebug() << "Name of level : " << name;
	AnnDebug() << "This will be the resource group name for Level Specific resource location declaration";

	//Add resource location to the Ogre Resource Group Manager
	if (!resourceLocAdded)
	{
		element = level->FirstChildElement("ResourceLocations");
		if (!element)
		{
			AnnDebug() << xmlFilePath << "Does not appear to have a 'ResourceLocations' section";
		}
		else
		{
			XMLElement* resourceLocation = element->FirstChildElement("ResourceLocation");
			if (resourceLocation) do
			{
				std::string type(resourceLocation->Attribute("Type")), path(resourceLocation->Attribute("Path"));
				if (!type.empty() && !path.empty())
					Ogre::ResourceGroupManager::getSingleton().addResourceLocation(dirPath + "/" + path, type, name);
			} while ((resourceLocation = resourceLocation->NextSiblingElement()) != nullptr);
			Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(name, false);
			resourceLocAdded = true;
		}
	}

	//Get level content
	element = level->FirstChildElement("LevelContent");

	if (!element)
	{
		AnnDebug() << xmlFilePath << "Don't have a 'LevelContent' section. This mean the level can't be loaded";
		throw AnnInitializationError(ANN_ERR_INFILE, "Error while reading XML Level file");
	}

	XMLElement* gameObject = element->FirstChildElement("Object");
	if (!gameObject) AnnDebug() << "No objects declared to load.";
	else do //Iterate through all game objects
	{
		std::string entityName;
		AnnDebug() << "Fond object to load";
		float x, y, z, w;
		std::string ID(gameObject->Attribute("ID"));
		AnnDebug() << "Registered ID : " << ID;
		XMLElement* gameObjectData = gameObject->FirstChildElement("Entity");
		if (gameObjectData)
			entityName = (gameObjectData->Attribute("EntityName"));

		std::shared_ptr<AnnGameObject> constructedGameObject;
		if (!ID.empty() && !entityName.empty()) constructedGameObject = addGameObject(entityName, ID);

		gameObjectData = gameObject->FirstChildElement("Position");
		if (gameObjectData)
		{
			gameObjectData->QueryFloatAttribute("X", &x);
			gameObjectData->QueryFloatAttribute("Y", &y);
			gameObjectData->QueryFloatAttribute("Z", &z);
			AnnDebug() << "Object at position : " << "(" << x << "," << y << "," << z << ")";
			constructedGameObject->setPosition(x, y, z);
		}

		gameObjectData = gameObject->FirstChildElement("Orientation");
		if (gameObjectData)
		{
			gameObjectData->QueryFloatAttribute("X", &x);
			gameObjectData->QueryFloatAttribute("Y", &y);
			gameObjectData->QueryFloatAttribute("Z", &z);
			gameObjectData->QueryFloatAttribute("W", &w);
			constructedGameObject->setOrientation(w, x, y, z);
		}

		gameObjectData = gameObject->FirstChildElement("Scale");
		if (gameObjectData)
		{
			gameObjectData->QueryFloatAttribute("X", &x);
			gameObjectData->QueryFloatAttribute("Y", &y);
			gameObjectData->QueryFloatAttribute("Z", &z);
			constructedGameObject->setScale(x, y, z);
		}

		XMLElement* physics = gameObject->FirstChildElement("Physics");
		if (!physics) continue; //no physics section. not mandatory. just ignore
		XMLElement* state = physics->FirstChildElement("Enabled");
		if (!state) continue;
		bool phy; state->QueryBoolText(&phy); if (!phy) continue;

		float mass(0); std::string shape;

		XMLElement* phyInfo = physics->FirstChildElement("Mass");
		if (!phyInfo) continue;
		phyInfo->QueryFloatText(&mass);

		phyInfo = physics->FirstChildElement("Shape");
		if (!phyInfo) continue;
		shape = phyInfo->GetText();
		if (shape == "static") mass = 0; //this case is weird. Static stuff have always been static, even with mass. Need to see if bullet has changed stuff
		constructedGameObject->setUpPhysics(mass, getShapeTypeFromString(shape));

		levelContent.push_back(constructedGameObject);
	} while ((gameObject = gameObject->NextSiblingElement()) != nullptr);

	element = level->FirstChildElement("LevelLighting");
	if (!element) AnnDebug() << "No lights declared";
	else
	{
		float x, y, z, dx, dy, dz, r, g, b, a;
		std::string lightType;
		XMLElement* source = element->FirstChildElement("Source");
		if (source) do
		{
			std::string lightID = source->Attribute("ID");

			XMLElement* position = source->FirstChildElement("Position");
			XMLElement* color = source->FirstChildElement("Color");
			XMLElement* type = source->FirstChildElement("Type");
			XMLElement* direction = source->FirstChildElement("Direction");

			position->QueryFloatAttribute("X", &x);
			position->QueryFloatAttribute("Y", &y);
			position->QueryFloatAttribute("Z", &z);
			direction->QueryFloatAttribute("X", &dx);
			direction->QueryFloatAttribute("Y", &dy);
			direction->QueryFloatAttribute("Z", &dz);
			color->QueryFloatAttribute("R", &r);
			color->QueryFloatAttribute("G", &g);
			color->QueryFloatAttribute("B", &b);
			color->QueryFloatAttribute("A", &a);
			lightType = type->GetText();

			auto lightSource = addLightObject(lightID);
			lightSource->setPosition(AnnVect3(x, y, z));
			lightSource->setType(AnnLightObject::getLightTypeFromString(lightType));
			lightSource->setDirection(AnnVect3(dx, dy, dz));
			lightSource->setDiffuseColor(AnnColor(r, g, b, a));
		} while ((source = source->NextSiblingElement()) != nullptr);
	}

	element = level->FirstChildElement("Player");
	if (element)
	{
		XMLElement* playerElement = element->FirstChildElement("Position");
		if (playerElement)
		{
			float x, y, z;
			playerElement->QueryFloatAttribute("X", &x);
			playerElement->QueryFloatAttribute("Y", &y);
			playerElement->QueryFloatAttribute("Z", &z);
			AnnDebug() << "Player starting position : (" << x << ", " << y << ", " << z << ")";

			AnnGetPlayer()->setPosition(AnnVect3(x, y, z));
		}
		else AnnGetPlayer()->setPosition(AnnPlayer::DEFAULT_STARTING_POS);

		playerElement = element->FirstChildElement("Orientation");
		if (playerElement)
		{
			float yaw;
			playerElement->QueryFloatAttribute("Yaw", &yaw);
			AnnDebug() << "Player Yaw : " << yaw;
			AnnGetPlayer()->setOrientation(Ogre::Euler(Ogre::Degree(yaw).valueRadians()));
		}
		else AnnGetPlayer()->setOrientation(AnnPlayer::DEFAULT_STARTING_ORIENT);
	}
	else
	{
		AnnGetPlayer()->setPosition(AnnPlayer::DEFAULT_STARTING_POS);
		AnnGetPlayer()->setOrientation(AnnPlayer::DEFAULT_STARTING_ORIENT);
	}
	AnnGetPlayer()->resetPlayerPhysics();
}

void AnnXmlLevel::runLogic()
{
	return;
}
