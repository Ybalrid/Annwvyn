#include "stdafx.h"
#include "AnnResourceManager.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnResourceManager::AnnResourceManager() : AnnSubSystem("ResourceManager"),
ResourceGroupManager{ Ogre::ResourceGroupManager::getSingletonPtr() }
{
	addDefaultResourceLocation();
}

void AnnResourceManager::addZipLocation(const std::string& path, const std::string& resourceGroupName)
{
	if (resourceGroupName == reservedResourceGroupName) return refuseResource(path, resourceGroupName);
	AnnDebug("Will load resources from Zip archive :");
	AnnDebug() << path;
	ResourceGroupManager->addResourceLocation(path, "Zip", resourceGroupName);
}

void AnnResourceManager::addFileLocation(const std::string& path, const std::string& resourceGroupName)
{
	if (resourceGroupName == reservedResourceGroupName) return refuseResource(path, resourceGroupName);
	AnnDebug("Will load resources from File-system directory :");
	AnnDebug() << path;
	ResourceGroupManager->addResourceLocation(path, "FileSystem", resourceGroupName);
}

void AnnResourceManager::addDefaultResourceLocation() const
{
	AnnDebug("Adding Annwvyn CORE resource locations");
	ResourceGroupManager->addResourceLocation("media/CORE.zip", "Zip", reservedResourceGroupName);
	ResourceGroupManager->addResourceLocation("media", "FileSystem", reservedResourceGroupName, true);
	ResourceGroupManager->initialiseResourceGroup(reservedResourceGroupName);
}

void AnnResourceManager::loadReseourceFile(const std::string& path)
{
	/*from ogre wiki : load the given resource file*/
	Ogre::ConfigFile configFile;
	configFile.load(path);
	auto seci = configFile.getSectionIterator();
	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();

		if (secName == reservedResourceGroupName)
		{
			refuseResource("*Did not read form file*", secName);
			continue;
		}

		auto settings = seci.getNext();
		for (const auto& setting : *settings)
		{
			typeName = setting.first;
			archName = setting.second;
			ResourceGroupManager->addResourceLocation(archName, typeName, secName);
		}
	}
}

void AnnResourceManager::initResources() const
{
	//addDefaultResourceLocaton();
	ResourceGroupManager->initialiseAllResourceGroups();
	AnnDebug("Resources initialized");
}

void AnnResourceManager::loadGroup(const std::string & groupName) const
{
	ResourceGroupManager->loadResourceGroup(groupName);
}

void AnnResourceManager::refuseResource(const std::string& resourceName, const std::string& group)
{
	AnnDebug() << "Annwvyn cannot allow you to set location inside the " << group << " resource group";
	AnnDebug() << "Resource location " << resourceName << "has been rejected";
}