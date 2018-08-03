#include "AnnProceduralGameObjectGenerator.hpp"

#include "Procedural.h"

#include "OgreMeshManager.h"
#include "OgreMeshManager2.h"

using namespace Annwvyn;

std::string AnnProceduralGameObjectGenerator::makeDotMeshName(const std::string& string) const
{
	static std::string const ext = ".mesh";

	if(const auto position = string.find_last_of('.'); position != std::string::npos)
	{
		assert(string.substr(position + 1) == "mesh");
		return string;
	}

	return string + ext;
}

void AnnProceduralGameObjectGenerator::clearMeshCache()
{
	cachedMeshes.clear();
}

bool AnnProceduralGameObjectGenerator::meshExists(const std::string& meshName) const
{
	if(!Ogre::v1::MeshManager::getSingleton().getByName(meshName).isNull())
		return true;

	if(!Ogre::MeshManager::getSingleton().getByName(meshName).isNull())
		return true;

	return false;
}

AnnProceduralGameObjectGenerator::AnnProceduralGameObjectGenerator(AnnGameObjectManager* ptr)
{
	GameObjectManager = ptr;
}

AnnGameObjectPtr AnnProceduralGameObjectGenerator::getBox(const std::string& name, const std::string& identifier, AnnVect3 size) const
{
	return getGeneratedObject<Procedural::BoxGenerator>(name, identifier, size.x, size.y, size.z);
}

AnnGameObjectPtr AnnProceduralGameObjectGenerator::getSphere(const std::string& name, const std::string& identifier, float radius, float rings, float segment) const
{
	return getGeneratedObject<Procedural::SphereGenerator>(name, identifier, radius, rings, segment);
}
