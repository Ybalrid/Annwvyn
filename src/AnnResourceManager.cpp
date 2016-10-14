#include "stdafx.h"
#include "AnnResourceManager.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnResourceManager::AnnResourceManager() : AnnSubSystem("ResourceManager")
{
	addDefaultResourceLocation();
}

void AnnResourceManager::addZipLocation(const std::string& path, const std::string& resourceGroupName)
{
	if (resourceGroupName == reservedResourceGroupName) return refuseResource(path, resourceGroupName);
	AnnDebug("Will load resources from Zip archive :");
	AnnDebug() << path;
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path, "Zip", resourceGroupName);
}

void AnnResourceManager::addFileLocation(const std::string& path, const std::string& resourceGroupName)
{
	if (resourceGroupName == reservedResourceGroupName) return refuseResource(path, resourceGroupName);
	AnnDebug("Will load resources from File-system directory :");
	AnnDebug() << path;
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path, "FileSystem", resourceGroupName);
}

void AnnResourceManager::addDefaultResourceLocation()
{
	AnnDebug("Adding Annwvyn CORE resource locations");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/CORE.zip", "Zip", reservedResourceGroupName);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media", "FileSystem", reservedResourceGroupName, true);
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(reservedResourceGroupName);
}

void AnnResourceManager::loadReseourceFile(const char path[])
{
	/*from ogre wiki : load the given resource file*/
	Ogre::ConfigFile configFile;
	configFile.load(path);
	Ogre::ConfigFile::SectionIterator seci = configFile.getSectionIterator();
	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();

		if (secName == reservedResourceGroupName)
		{
			refuseResource("*Did not read file*", secName);
			continue;
		}

		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
		}
	}
}

void AnnResourceManager::initResources()
{
	//addDefaultResourceLocaton();
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	AnnDebug("Resources initialized");
}

void Annwvyn::AnnResourceManager::loadGroup(const std::string & groupName)
{
	Ogre::ResourceGroupManager::getSingleton().loadResourceGroup(groupName);
}

void AnnResourceManager::refuseResource(const std::string& resourceName, const std::string& group)
{
	AnnDebug() << "Annwvyn cannot allow you to set location inside the " << group << " resource group";
	AnnDebug() << "Resource location " << resourceName << "has been rejected";
}