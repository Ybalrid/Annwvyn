#include "stdafx.h"
#include "AnnResourceManager.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnResourceManager::AnnResourceManager() : AnnSubSystem("ResourceManager")
{

}

void AnnResourceManager::loadZip(const char path[], const char resourceGroupName[])
{
	AnnDebug("Loading resources from Zip archive :");
	AnnDebug() << path;
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path, "Zip", resourceGroupName);
}

void AnnResourceManager::loadDir(const char path[], const char resourceGroupName[])
{
	AnnDebug("Loading resources from Filesystem directory :");
	AnnDebug() << path;
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path, "FileSystem", resourceGroupName);
}

void AnnResourceManager::addDefaultResourceLocaton()
{
	AnnDebug("Adding Annwvyn CORE resources");
	loadDir("media");
	loadZip("media/CORE.zip");
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
	addDefaultResourceLocaton();
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	AnnDebug("Resources initialized");
}
