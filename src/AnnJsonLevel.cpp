
#include <AnnJsonLevel.hpp>
#include <json.hpp>

#include <fstream>
#include "Annwvyn.h"

//Chaiscript also exposes a json class took from "simple json".
//But the interface of "JSON for modern C++" is nicer to use
using json_t = nlohmann::json;

namespace Annwvyn
{
	//Our little pimpl
	struct AnnJsonLevel::AnnJson
	{
		json_t j;
	};

	//Type conversion are defined by overloading from_json and to_json
	void from_json(const json_t& j, AnnVect3& v)
	{
		v.x = j[0];
		v.y = j[1];
		v.z = j[2];
	}

	void from_json(const json_t& j, AnnQuaternion& q)
	{
		q.x = j[0];
		q.y = j[1];
		q.z = j[2];
		q.w = j[3];
	}

	struct phyParam
	{
		bool colideWithPlayer;
		float mass;
		phyShapeType type;
	};

	inline phyShapeType stringToShape(const std::string& str)
	{
		if(str == "static")
			return staticShape;
		if(str == "convex")
			return convexShape;
		if(str == "box")
			return boxShape;
		if(str == "cylinder")
			return cylinderShape;
		if(str == "capsule")
			return capsuleShape;
		if(str == "sphere")
			return sphereShape;
		return error;
	}

	void from_json(const json_t& j, phyParam& p)
	{
		p.mass			   = j["mass"];
		p.type			   = stringToShape(j["shape"]);
		p.colideWithPlayer = j["playerColide"];
	}

	void from_json(const json_t& j, AnnGameObjectPtr& obj)
	{
		auto GameObjectManager = AnnGetGameObjectManager();
		obj					   = GameObjectManager->createGameObject(j["mesh"], j["name"]);
		if(!obj) throw AnnNullGameObjectError();

		obj->setPosition(j["position"]);
		obj->setOrientation(j["orientation"]);
		obj->setScale(j["scale"]);

		if(j["hasPhysics"])
		{
			phyParam param = j["physics"];
			if(param.type == error) throw AnnInvalidPhysicalShapeError(obj->getName());
			obj->setupPhysics(param.mass, param.type, param.colideWithPlayer);
		}

		if(!j["scripts"].is_null())
		{
			for(auto& jsonScript : j["scripts"])
				obj->attachScript(jsonScript);
		}
	}

	AnnLightObject::LightTypes lightTypeFromString(const std::string& s)
	{
		if(s == "directional") return AnnLightObject::ANN_LIGHT_DIRECTIONAL;
		if(s == "spot") return AnnLightObject::ANN_LIGHT_SPOTLIGHT;
		if(s == "point") return AnnLightObject::ANN_LIGHT_POINT;

		return AnnLightObject::ANN_LIGHT_ERROR;
	}

	void from_json(const json_t& j, AnnLightObjectPtr& l)
	{
		auto GameObjectManager = AnnGetGameObjectManager();
		l					   = GameObjectManager->createLightObject(j["name"]);

		auto type = lightTypeFromString(j["type"]);
		if(type != AnnLightObject::ANN_LIGHT_ERROR)
			l->setType(type);

		l->setPower(j["power"]);
		if(j.find("position") != std::end(j))
			l->setPosition(j["position"]);
		if(j.find("direction") != std::end(j))
			l->setDirection(j["direction"]);
	}

	struct resLocParam
	{
		std::string group;
		std::string path;
		std::string type;
	};

	void declareResource(const resLocParam& res)
	{
		auto resourceManager = AnnGetResourceManager();
		if(res.type == "Zip")
			resourceManager->addZipLocation(res.path, res.group);
	}

	void from_json(const json_t& j, resLocParam& p)
	{
		if(!j["group"].is_null())
			p.group = j["group"];
		else
			p.group = AnnResourceManager::getDefaultResourceGroupName();

		p.path = j["path"];
		p.type = j["type"];
	}
}
using namespace Annwvyn;

AnnJsonLevel::AnnJsonLevel(std::string path, const bool preload) :
 constructLevel(),
 preloadResources(preload)
{
	jsonFile   = std::make_unique<AnnJson>();
	auto& json = jsonFile->j;

	//Read full content of the pointed file
	const std::string file{
		[&] {
			std::ifstream fileStream(path);
			if(fileStream)
				return std::string(std::istreambuf_iterator<char>(fileStream),
								   std::istreambuf_iterator<char>());
			return std::string{};
		}()
	};

	//If it wasn't possible to open the file or anything happened while reading chars
	if(file.empty())
		throw AnnInitializationError(ANN_ERR_INFILE, "Could not load content of JSON level file " + path);

	//Load JSON
	json = json_t::parse(file);

	processJson();
}

AnnJsonLevel::AnnJsonLevel(bool, std::string jsonCode, const bool preload) :
 constructLevel(),
 preloadResources(preload)
{
	jsonFile   = std::make_unique<AnnJson>();
	auto& json = jsonFile->j;
	json	   = json_t::parse(jsonCode);
	processJson();
}

AnnJsonLevel::~AnnJsonLevel()
{
}

void AnnJsonLevel::load()
{
	auto& json = jsonFile->j;

	for(auto& jsonGameObject : json["content"])
		levelContent.push_back(jsonGameObject);

	for(auto& jsonLight : json["lighting"])
		levelLighting.push_back(jsonLight);

	auto player = AnnGetPlayer();
	player->setPosition(json["player"]["startPosition"]);
	player->setOrientation(json["player"]["startOrientation"].get<AnnQuaternion>());
	AnnDebug() << "Player position reset";
	AnnDebug() << player->getPosition();
	AnnDebug() << player->getOrientation();
}

void AnnJsonLevel::runLogic()
{
}

void AnnJsonLevel::processJson()
{
	auto& json = jsonFile->j;
	AnnDebug() << "got json:\n"
			   << json;
	name = json["name"].get<std::string>();
	AnnDebug() << "name is " << name;

	if(!json["resources"].is_null())
		AnnDebug() << "Defined " << json["resources"].size() << " resources";
	for(const resLocParam resource : json["resources"])
	{
		declareResource(resource);
		auto resourceManager = AnnGetResourceManager();
		if(preloadResources && resource.group != resourceManager->getDefaultResourceGroupName())
		{
			resourceManager->loadGroup(resource.group);
		}
	}
}
