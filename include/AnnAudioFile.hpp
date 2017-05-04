#pragma once

#include "systemMacro.h"
#include <OgreSharedPtr.h>
#include <OgreResourceManager.h>
#include <vector>
#include <algorithm>
#include <sndfile.h>
#include "AnnTypes.h"

namespace Annwvyn
{
	class DLL AnnAudioFile : public Ogre::Resource
	{
		std::vector<byte> data;

		void readFromStream(Ogre::DataStreamPtr &stream);

	protected:
		void loadImpl() override;
		void unloadImpl() override;

		size_t calculateSize() const override;

	public:
		AnnAudioFile(Ogre::ResourceManager* creator,
			const Ogre::String& name,
			Ogre::ResourceHandle handle,
			const Ogre::String& group,
			bool isManual = false,
			Ogre::ManualResourceLoader* loader = nullptr);

		virtual ~AnnAudioFile();

		const byte* getData() const;
		size_t getSize() const override;

		//Virtual IO interface

		///Utility class that perform a static_cast<AnnAudioFile*> on the pointer you give it.
		static AnnAudioFile* cast(void* audioFileRawPtr);

		///Structure that will contain function pointers to all the functions defined below
		static SF_VIRTUAL_IO* sfVioStruct;

		///Get the length of the file
		static sf_count_t sfVioGetFileLen(void* audioFileRawPtr);

		///Seek (move reading cursor) inside virtual file
		static sf_count_t sfVioSeek(sf_count_t offset, int whence, void* audioFileRawPtr);

		///REad "count" bytes from cursor to ptr. Will return number of bytes actually read. Will not read past the end of data but do not check writing to the pointer
		static sf_count_t sfVioRead(void* ptr, sf_count_t count, void* audiFileRawPtr);

		///Do nothing. Dummy function just to fill up the interface
		static sf_count_t sfVioWriteDummy(const void *, sf_count_t, void*);

		///return current cursor position
		static sf_count_t sfVioTell(void* audioFileRawPtr);

		///Get the virtual I/O struct. Will initialize it at 1st call
		static SF_VIRTUAL_IO* getSndFileVioStruct();

		///For cleanup. Will deallocate the VioStruct and set the pointer back to nullptr
		static void clearSndFileVioStruct();

		///Current cursor position. the only "state" used while reading the file from libsndfile (except for the data itself, that is non mutable)
		size_t sf_offset;
	};

	using AnnAudioFilePtr = Ogre::SharedPtr<AnnAudioFile>;

	class DLL AnnAudioFileManager : public Ogre::ResourceManager, public Ogre::Singleton<AnnAudioFileManager>
	{
	protected:
		Ogre::Resource* createImpl(const Ogre::String &name, Ogre::ResourceHandle handle,
			const Ogre::String &group, bool isManual, Ogre::ManualResourceLoader *loader,
			const Ogre::NameValuePairList *createParams) override;
	public:
		AnnAudioFileManager();
		virtual ~AnnAudioFileManager();

		virtual AnnAudioFilePtr load(const Ogre::String& name, const Ogre::String& group);

		static AnnAudioFileManager& getSingleton();
		static AnnAudioFileManager* getSingletonPtr();
	};
}