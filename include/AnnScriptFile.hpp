#pragma once

#include <OgreResourceManager.h>
#include <OgreSharedPtr.h>

#include "systemMacro.h"
namespace Annwvyn
{
	///Ogre resource that contains the source code of a loaded .chai file
	class DLL AnnScriptFile : public Ogre::Resource
	{
		///Actual code of the file
		std::string sourceCode;

		///Code evaluated on the chaiscript global scope
		bool loadedByChaiScript;

	protected:
		///Load the file
		void loadImpl() override;

		///Unload the file
		void unloadImpl() override;

		///Get the size of the file
		size_t calculateSize() const override;

	public:
		///Resource constructor. Called by the ResourceManager not by you
		AnnScriptFile(Ogre::ResourceManager* creator,
			const Ogre::String& name,
			Ogre::ResourceHandle handle,
			const Ogre::String& group,
			bool isManual = false,
			Ogre::ManualResourceLoader* loader = nullptr);

		///Destroy the resource object (will call `unload()`)
		virtual ~AnnScriptFile();

		///Get the loaded code
		const std::string& getSourceCode() const;

		///Set the source code. Intended to be used by the ResourceManager too
		void setLoadedSourceCode(const std::string& sourceCode);

		///Set the loaded to true
		void signalLoadedInChaiscript();

		///Return if we are loaded
		bool loadedInChaiscriptInterpretor() const;
	};

	///Easy define of the shared pointer
	using AnnScriptFilePtr = Ogre::SharedPtr<AnnScriptFile>;

	///Chaiscript File Serializer
	class DLL AnnScriptFileSerializer : public Ogre::Serializer
	{
	public:
		///Default construct a ChaiscriptResourceSerializer
		AnnScriptFileSerializer() = default;

		///Export code to file (dump)
		void exportChaiscript(const AnnScriptFile* pScriptSource, const Ogre::String& filename);

		///Import the code from a file datastream into a AnnScriptFile file
		void importSourceCode(Ogre::DataStreamPtr &stream, AnnScriptFile* pDest);
	};

	///Chaiscript File ResourceManager
	class AnnScriptFileResourceManager : public Ogre::ResourceManager, public Ogre::Singleton<AnnScriptFileResourceManager>
	{
	protected:
		///Create an AnnScriptFile
		Ogre::Resource* createImpl(const Ogre::String &name, Ogre::ResourceHandle handle,
			const Ogre::String &group, bool isManual, Ogre::ManualResourceLoader *loader,
			const Ogre::NameValuePairList *createParams) override;
	public:
		AnnScriptFileResourceManager();
		virtual ~AnnScriptFileResourceManager();

		virtual AnnScriptFilePtr load(const Ogre::String& name, const Ogre::String& group);

		static AnnScriptFileResourceManager& getSingleton();
		static AnnScriptFileResourceManager* getSingletonPtr();
	};
}