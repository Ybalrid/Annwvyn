#pragma once
#include "systemMacro.h"
#include "OgreResourceGroupManager.h"
#include "AnnSubsystem.hpp"

namespace Annwvyn
{
	///Annwvyn interface class with Ogre's ResourceGroupManager. 
	class DLL AnnResourceManager : public AnnSubSystem
	{
	public:
		AnnResourceManager();
		///Give a zipped archive resource location to the Ogre Resource Group Manager
		/// \param path The path to a zip file.
		/// \param resourceGroupName name of the resource group where the content will be added
		void loadZip(const char path[], const char resourceGroupName[] = "ANNWVYN_DEFAULT");

		///Give a directory resouce location to the Ogre Resource Group Manager
		/// \param path The path to the directory
		/// \param resourceGroupName name of the resource group
		void loadDir(const char path[], const char resourceGroupName[] = "ANNWVYN_DEFAULT");

		///Add to the default resource group "FileSystem=media" and "Zip=media/CORE.zip"
		void addDefaultResourceLocaton();

		///Load resource.cfg like file
		void loadReseourceFile(const char path[]);

		///InitAllResources
		void initResources();


	};
}