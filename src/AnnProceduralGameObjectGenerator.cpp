#include "AnnProceduralGameObjectGenerator.hpp"
#include "Procedural.h"
#include "Annwvyn.h"

using namespace Annwvyn;

AnnProceduralGameObjectGenerator::AnnProceduralGameObjectGenerator()
{
}

AnnGameObjectPtr AnnProceduralGameObjectGenerator::getBox(std::string name, AnnVect3 size, std::string identifier)
{
	std::string ext = "";

	if(auto position = name.find_last_of('.'); position != std::string::npos)
	{
		assert(name.substr(position + 1) == "mesh");
	}
	else
	{
		ext = ".mesh";
	}

	name += ext;

	auto boxGenerator = Procedural::BoxGenerator(size.x, size.y, size.z);
	auto mesh = boxGenerator.realizeMesh(name);

	static const auto manager = AnnGetGameObjectManager();
	manager->setImportParameter(true, true, false);
	auto object = manager->createGameObject(name, identifier);
	manager->setImportParameter(true, true, true);

	mesh.setNull();

	assert(object != nullptr);
	return object;
}
