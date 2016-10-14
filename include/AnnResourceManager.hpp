#ifndef ANN_RESOURCE_MANAGER
#define ANN_RESOURCE_MANAGER

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
		void addZipLocation(const std::string& path, const std::string& resourceGroupName = defaultResourceGroupName);
		///Give a directory resouce location to the Ogre Resource Group Manager
		/// \param path The path to the directory
		/// \param resourceGroupName name of the resource group
		void addFileLocation(const std::string& path, const std::string& resourceGroupName = defaultResourceGroupName);

		///Load resource.cfg like file
		void loadReseourceFile(const char path[]);

		///InitAllResources
		void initResources();

		///Load in memory the content of the specified group
		void loadGroup(const std::string& groupName);

		///Deprecated. Use addZipLocation instead
		DEPRECATED void loadZip(const char path[], const char resourceGroupName[] = defaultResourceGroupName) { return addZipLocation(path, resourceGroupName); }

		///Deprecated. Use addFileLocation instead
		DEPRECATED void loadDir(const char path[], const char resourceGroupName[] = defaultResourceGroupName) { return addFileLocation(path, resourceGroupName); }

		///Name of the default resource group
		static constexpr const char* const defaultResourceGroupName = "Annwvyn_UserDefined_Group";

		///name of the resrource group used to store CORE resources
		static constexpr const char* const reservedResourceGroupName = "ANNWVYN_CORE";

	public:
		///Log the fact that resource location creation as been rejected
		void refuseResource(const std::string& name, const std::string& group);

		///Add to the default resource group "FileSystem=media" and "Zip=media/CORE.zip"
		void addDefaultResourceLocation();
	};
}

#endif