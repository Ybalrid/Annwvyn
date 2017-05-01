#pragma once

#include <OgreResourceManager.h>
#include <OgreSharedPtr.h>

#include "systemMacro.h"
namespace Annwvyn
{
	///Ogre resource that contains the source code of a loaded .chai file
	class DLL AnnChaiScriptResource : public Ogre::Resource
	{
		std::string sourceCode;
		bool loadedByChaiScript;

	protected:
		void loadImpl() override;
		void unloadImpl() override;
		size_t calculateSize() const override;

	public:
		AnnChaiScriptResource(Ogre::ResourceManager* creator,
			const Ogre::String& name,
			Ogre::ResourceHandle handle,
			const Ogre::String& group,
			bool isManual = false,
			Ogre::ManualResourceLoader* loader = nullptr);

		virtual ~AnnChaiScriptResource();

		const std::string& getString() const;
		void setString(const std::string& sourceCode);

		void callMeIfYouLoadMe();
		bool loadedInChaiscriptInterpretor() const;
	};

	///Ogre SharedPointer implementation for our resource

	//class DLL AnnChaiScriptResourcePtr : public Ogre::SharedPtr<AnnChaiScriptResource>
	//{
	//public:
	//	AnnChaiScriptResourcePtr() : Ogre::SharedPtr<Annwvyn::AnnChaiScriptResource>() {}
	//	explicit AnnChaiScriptResourcePtr(AnnChaiScriptResourcePtr* rep) : Ogre::SharedPtr<Annwvyn::AnnChaiScriptResource>(rep) {}
	//	AnnChaiScriptResourcePtr(const AnnChaiScriptResourcePtr& r) : Ogre::SharedPtr<Annwvyn::AnnChaiScriptResource>(r) {}
	//	AnnChaiScriptResourcePtr(const Ogre::ResourcePtr& r) : Ogre::SharedPtr<Annwvyn::AnnChaiScriptResource>(r)
	//	{
	//		if (r.isNull()) return;

	//		// lock & copy other mutex pointer
	//		OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME);
	//		OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME);

	//		pRep = static_cast<AnnChaiScriptResource*>(r.getPointer());
	//		pInfo = r.pInfo;

	//		if (pRep)
	//		{
	//			++(pInfo->useCount);
	//		}
	//	}
	//};

	///Ogre SharedPointer implementation for our resource
	//using AnnChaiScriptResourcePtr = Ogre::SharedPtr<AnnChaiScriptResource>;
	typedef Ogre::SharedPtr<AnnChaiScriptResource> AnnChaiScriptResourcePtr;

	class DLL AnnChaiScriptResourceSerializer : public Ogre::Serializer
	{
	public:
		AnnChaiScriptResourceSerializer();
		virtual ~AnnChaiScriptResourceSerializer();

		void exportChaiscript(const AnnChaiScriptResource* pScriptSource, const Ogre::String& filename);
		void importTextFile(Ogre::DataStreamPtr &stream, AnnChaiScriptResource* pDest);
	};

	class AnnChaisScriptResourceManager : public Ogre::ResourceManager, public Ogre::Singleton<AnnChaisScriptResourceManager>
	{
	protected:
		Ogre::Resource* createImpl(const Ogre::String &name, Ogre::ResourceHandle handle,
			const Ogre::String &group, bool isManual, Ogre::ManualResourceLoader *loader,
			const Ogre::NameValuePairList *createParams) override;
	public:
		AnnChaisScriptResourceManager();
		virtual ~AnnChaisScriptResourceManager();

		virtual AnnChaiScriptResourcePtr load(const Ogre::String& name, const Ogre::String& group);

		static AnnChaisScriptResourceManager& getSingleton();
		static AnnChaisScriptResourceManager* getSingletonPtr();
	};
}