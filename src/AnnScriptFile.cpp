#include "stdafx.h"
#include "AnnScriptFile.hpp"
#include "AnnLogger.hpp"

//AnnChaiScriptFile resource ---------------------------------------
void Annwvyn::AnnScriptFile::loadImpl()
{
	AnnScriptFileSerializer serializer;
	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(mName, mGroup, true, this);
	serializer.importSourceCode(stream, this);
}

void Annwvyn::AnnScriptFile::unloadImpl()
{
	sourceCode.clear();
}

size_t Annwvyn::AnnScriptFile::calculateSize() const
{
	return sourceCode.length();
}

Annwvyn::AnnScriptFile::AnnScriptFile(Ogre::ResourceManager* creator, const Ogre::String& name, Ogre::ResourceHandle handle, const Ogre::String& group, bool isManual, Ogre::ManualResourceLoader* loader) :
	Ogre::Resource(creator, name, handle, group, isManual, loader),
	loadedByChaiScript(false)
{
	createParamDictionary("AnnChaiScriptFile");
}

Annwvyn::AnnScriptFile::~AnnScriptFile()
{
	AnnScriptFile::unload();
}

const std::string& Annwvyn::AnnScriptFile::getSourceCode() const
{
	return sourceCode;
}

void Annwvyn::AnnScriptFile::setLoadedSourceCode(const std::string& newSourceCode)
{
	sourceCode = newSourceCode;
}

void Annwvyn::AnnScriptFile::signalLoadedInChaiscript()
{
	loadedByChaiScript = true;
}

bool Annwvyn::AnnScriptFile::loadedInChaiscriptInterpretor() const
{
	return loadedByChaiScript;
}

//AnnChaiScript serializer -------------------------------
void Annwvyn::AnnScriptFileSerializer::exportChaiscript(const AnnScriptFile* pScriptSource, const Ogre::String& filename)
{
	std::ofstream outFile;
	outFile.open(filename, std::ios::out);
	outFile << pScriptSource->getSourceCode();
}

void Annwvyn::AnnScriptFileSerializer::importSourceCode(Ogre::DataStreamPtr& stream, AnnScriptFile* pDest)
{
	pDest->setLoadedSourceCode(stream->getAsString());
}

Ogre::Resource* Annwvyn::AnnScriptFileResourceManager::createImpl(const Ogre::String& name, Ogre::ResourceHandle handle, const Ogre::String& group, bool isManual, Ogre::ManualResourceLoader* loader, const Ogre::NameValuePairList* createParams)
{
	return new AnnScriptFile(this, name, handle, group, isManual, loader);
}

//Res Manager
template<> Annwvyn::AnnScriptFileResourceManager *Ogre::Singleton<Annwvyn::AnnScriptFileResourceManager>::msSingleton = nullptr;

Annwvyn::AnnScriptFileResourceManager::AnnScriptFileResourceManager()
{
	mResourceType = "AnnChaiScriptFile";
	mLoadOrder = 1;
	Ogre::ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
}

Annwvyn::AnnScriptFileResourceManager::~AnnScriptFileResourceManager()
{
	Ogre::ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);
}

Annwvyn::AnnScriptFilePtr Annwvyn::AnnScriptFileResourceManager::load(const Ogre::String& name, const Ogre::String& group)
{
	AnnScriptFilePtr script = createOrRetrieve(name, group).first.staticCast<AnnScriptFile>();
	script->load();
	return script;
}

Annwvyn::AnnScriptFileResourceManager& Annwvyn::AnnScriptFileResourceManager::getSingleton()
{
	assert(msSingleton);
	return *msSingleton;
}

Annwvyn::AnnScriptFileResourceManager* Annwvyn::AnnScriptFileResourceManager::getSingletonPtr()
{
	return msSingleton;
}