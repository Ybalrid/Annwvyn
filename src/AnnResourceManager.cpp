// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "stdafx.h"
#include "AnnResourceManager.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnResourceManager::AnnResourceManager() : AnnSubSystem("ResourceManager"),
ResourceGroupManager{ Ogre::ResourceGroupManager::getSingletonPtr() }
{
	ResourceGroupManager->createResourceGroup(getDefaultResourceGroupName());
	addDefaultResourceLocation();
}

void AnnResourceManager::addZipLocation(const std::string& path, const std::string& resourceGroupName) const
{
	if (resourceGroupName == getReservedResourceGroupName()) return refuseResource(path, resourceGroupName);
	AnnDebug("Will load resources from Zip archive :");
	AnnDebug() << path;
	ResourceGroupManager->addResourceLocation(path, "Zip", resourceGroupName);
}

void AnnResourceManager::addFileLocation(const std::string& path, const std::string& resourceGroupName) const
{
	if (resourceGroupName == getReservedResourceGroupName()) return refuseResource(path, resourceGroupName);
	AnnDebug("Will load resources from File-system directory :");
	AnnDebug() << path;
	ResourceGroupManager->addResourceLocation(path, "FileSystem", resourceGroupName);
}

void AnnResourceManager::addDefaultResourceLocation() const
{
	AnnDebug("Adding Annwvyn CORE resource locations");
	ResourceGroupManager->addResourceLocation("media/CORE.zip", "Zip", getReservedResourceGroupName());
	ResourceGroupManager->addResourceLocation("media", "FileSystem", getReservedResourceGroupName(), true);
	ResourceGroupManager->initialiseResourceGroup(getReservedResourceGroupName(), true);
}

void AnnResourceManager::loadReseourceFile(const std::string& path) const
{
	/*from ogre wiki : load the given resource file*/
	Ogre::ConfigFile configFile;
	configFile.load(path);
	auto seci = configFile.getSectionIterator();
	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();

		if (secName == getReservedResourceGroupName())
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
	ResourceGroupManager->initialiseAllResourceGroups(true);
	AnnDebug("Resources initialized");
}

void AnnResourceManager::loadGroup(const std::string & groupName) const
{
	ResourceGroupManager->loadResourceGroup(groupName);
}

const char* AnnResourceManager::getDefaultResourceGroupName()
{
	return "b";
}

const char* AnnResourceManager::getReservedResourceGroupName()
{
	return "a";
}

void AnnResourceManager::refuseResource(const std::string& resourceName, const std::string& group)
{
	AnnDebug() << "Annwvyn cannot allow you to set location inside the " << group << " resource group";
	AnnDebug() << "Resource location " << resourceName << "has been rejected";
}
