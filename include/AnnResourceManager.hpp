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
		void addZipLocation(const std::string& path, const std::string& resourceGroupName = defaultResourceGroupName) const;

		///Give a directory resource location to the Ogre Resource Group Manager
		/// \param path The path to the directory
		/// \param resourceGroupName name of the resource group
		void addFileLocation(const std::string& path, const std::string& resourceGroupName = defaultResourceGroupName) const;

		///Load resource.cfg like file
		void loadReseourceFile(const std::string& path) const;

		///InitAllResources
		void initResources() const;

		///Load in memory the content of the specified group
		void loadGroup(const std::string& groupName) const;

		///Name of the default resource group
		static constexpr const char* const defaultResourceGroupName = "Annwvyn_UserDefined_Group";

		///name of the resource group used to store CORE resources
		static constexpr const char* const reservedResourceGroupName = "ANNWVYN_CORE";

	private:
		///Log the fact that resource location creation as been rejected
		static void refuseResource(const std::string& name, const std::string& group);

		///Add to the default resource group "FileSystem=media" and "Zip=media/CORE.zip"
		void addDefaultResourceLocation() const;

		///Pointer to the resource group manager. We cache the address to prevent calling a static method all the time
		Ogre::ResourceGroupManager* ResourceGroupManager;
	};
}

#endif