#include "stdafx.h"
#include "AnnChaiScriptResource.hpp"
#include "AnnLogger.hpp"

//AnnChaiScriptFile resource ---------------------------------------
void Annwvyn::AnnChaiScriptResource::loadImpl()
{
	AnnDebug() << "AnnChaiScriptResource::loadImpr()";
	AnnChaiScriptResourceSerializer serializer;
	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(mName, mGroup, true, this);
	serializer.importTextFile(stream, this);
}

void Annwvyn::AnnChaiScriptResource::unloadImpl()
{
	sourceCode.clear();
}

size_t Annwvyn::AnnChaiScriptResource::calculateSize() const
{
	return sourceCode.length();
}

Annwvyn::AnnChaiScriptResource::AnnChaiScriptResource(Ogre::ResourceManager* creator, const Ogre::String& name, Ogre::ResourceHandle handle, const Ogre::String& group, bool isManual, Ogre::ManualResourceLoader* loader) :
	Ogre::Resource(creator, name, handle, group, isManual, loader),
	loadedByChaiScript(false)
{
	createParamDictionary("AnnChaiScriptFile");
}

Annwvyn::AnnChaiScriptResource::~AnnChaiScriptResource()
{
	unload();
}

const std::string& Annwvyn::AnnChaiScriptResource::getString() const
{
	return sourceCode;
}

void Annwvyn::AnnChaiScriptResource::setString(const std::string& newSourceCode)
{
	sourceCode = newSourceCode;
}

void Annwvyn::AnnChaiScriptResource::callMeIfYouLoadMe()
{
	loadedByChaiScript = true;
}

bool Annwvyn::AnnChaiScriptResource::loadedInChaiscriptInterpretor() const
{
	return loadedByChaiScript;
}

//AnnChaiScript serializer -------------------------------
Annwvyn::AnnChaiScriptResourceSerializer::AnnChaiScriptResourceSerializer()
{
}

Annwvyn::AnnChaiScriptResourceSerializer::~AnnChaiScriptResourceSerializer()
{
}

void Annwvyn::AnnChaiScriptResourceSerializer::exportChaiscript(const AnnChaiScriptResource* pScriptSource, const Ogre::String& filename)
{
	std::ofstream outFile;
	outFile.open(filename, std::ios::out);
	outFile << pScriptSource->getString();
}

void Annwvyn::AnnChaiScriptResourceSerializer::importTextFile(Ogre::DataStreamPtr& stream, AnnChaiScriptResource* pDest)
{
	pDest->setString(stream->getAsString());
}

Ogre::Resource* Annwvyn::AnnChaisScriptResourceManager::createImpl(const Ogre::String& name, Ogre::ResourceHandle handle, const Ogre::String& group, bool isManual, Ogre::ManualResourceLoader* loader, const Ogre::NameValuePairList* createParams)
{
	return new AnnChaiScriptResource(this, name, handle, group, isManual, loader);
}

//Res Manager
template<> Annwvyn::AnnChaisScriptResourceManager *Ogre::Singleton<Annwvyn::AnnChaisScriptResourceManager>::msSingleton = nullptr;

Annwvyn::AnnChaisScriptResourceManager::AnnChaisScriptResourceManager()
{
	mResourceType = "AnnChaiScriptFile";
	mLoadOrder = 1;
	Ogre::ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
}

Annwvyn::AnnChaisScriptResourceManager::~AnnChaisScriptResourceManager()
{
	Ogre::ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);
}

Annwvyn::AnnChaiScriptResourcePtr Annwvyn::AnnChaisScriptResourceManager::load(const Ogre::String& name, const Ogre::String& group)
{
	AnnChaiScriptResourcePtr script = createOrRetrieve(name, group).first.staticCast<AnnChaiScriptResource>();
	script->load();
	return script;
}

Annwvyn::AnnChaisScriptResourceManager& Annwvyn::AnnChaisScriptResourceManager::getSingleton()
{
	assert(msSingleton);
	return *msSingleton;
}

Annwvyn::AnnChaisScriptResourceManager* Annwvyn::AnnChaisScriptResourceManager::getSingletonPtr()
{
	return msSingleton;
}