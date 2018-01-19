// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "stdafx.h"
#include "AnnScriptFile.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;
using namespace Ogre;

//AnnChaiScriptFile resource ---------------------------------------
void AnnScriptFile::loadImpl()
{
	AnnScriptFileSerializer serializer;
	auto stream = ResourceGroupManager::getSingleton().openResource(mName, mGroup, true, this);
	serializer.importSourceCode(stream, this);
}

void AnnScriptFile::unloadImpl()
{
	sourceCode.clear();
}

size_t AnnScriptFile::calculateSize() const
{
	return sourceCode.length();
}

AnnScriptFile::AnnScriptFile(ResourceManager* creator, const String& name, ResourceHandle handle, const String& group, bool isManual, ManualResourceLoader* loader) :
	Resource(creator, name, handle, group, isManual, loader),
	loadedByChaiScript(false)
{
	createParamDictionary("AnnScriptFile");
}

AnnScriptFile::~AnnScriptFile()
{
	AnnScriptFile::unload();
}

const std::string& AnnScriptFile::getSourceCode() const
{
	return sourceCode;
}

void AnnScriptFile::setLoadedSourceCode(const std::string& newSourceCode)
{
	sourceCode = newSourceCode;
}

void AnnScriptFile::signalLoadedInChaiscript()
{
	loadedByChaiScript = true;
}

bool AnnScriptFile::loadedInChaiscriptInterpretor() const
{
	return loadedByChaiScript;
}

//AnnChaiScript serializer -------------------------------
void AnnScriptFileSerializer::exportChaiscript(const AnnScriptFile* pScriptSource, const String& filename)
{
	std::ofstream outFile;
	outFile.open(filename, std::ios::out);
	outFile << pScriptSource->getSourceCode();
}

void AnnScriptFileSerializer::importSourceCode(DataStreamPtr& stream, AnnScriptFile* pDest)
{
	pDest->setLoadedSourceCode(stream->getAsString());
}

Resource* AnnScriptFileResourceManager::createImpl(const String& name, ResourceHandle handle, const String& group, bool isManual, ManualResourceLoader* loader, const NameValuePairList* createParams)
{
	return OGRE_NEW AnnScriptFile(this, name, handle, group, isManual, loader);
}

//Res Manager
template<> AnnScriptFileResourceManager *Singleton<AnnScriptFileResourceManager>::msSingleton = nullptr;

AnnScriptFileResourceManager::AnnScriptFileResourceManager()
{
	mResourceType = "AnnScriptFile";
	mLoadOrder = 1; //Theses resources are super light, and are part of the gameplay code of the game
	ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
}

AnnScriptFileResourceManager::~AnnScriptFileResourceManager()
{
	ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);
}

AnnScriptFilePtr AnnScriptFileResourceManager::load(const String& name, const String& group)
{
	auto script = createOrRetrieve(name, group).first.staticCast<AnnScriptFile>();
	script->load();
	return script;
}

AnnScriptFileResourceManager& AnnScriptFileResourceManager::getSingleton()
{
	assert(msSingleton);
	return *msSingleton;
}

AnnScriptFileResourceManager* AnnScriptFileResourceManager::getSingletonPtr()
{
	return msSingleton;
}
