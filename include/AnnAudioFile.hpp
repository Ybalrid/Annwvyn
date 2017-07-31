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
		///Where the data is actually stored, as bytes.
		std::vector<byte> data;

		///Read bytes from a data stream and stick them inside the "data" re-sizable array
		void readFromStream(Ogre::DataStreamPtr &stream);

		///Utility class that perform a static_cast<AnnAudioFile*> on the pointer you give it.
		inline static AnnAudioFile* cast(void* audioFileRawPtr);

	protected:
		///Actually load the data
		void loadImpl() override;

		///Clear the data vector
		void unloadImpl() override;

		///Return the size of the data vector
		size_t calculateSize() const override;

	public:
		///Create an audio file. This is intended to be called by a resource manager, not by the user.
		AnnAudioFile(Ogre::ResourceManager* creator,
			const Ogre::String& name,
			Ogre::ResourceHandle handle,
			const Ogre::String& group,
			bool isManual = false,
			Ogre::ManualResourceLoader* loader = nullptr);

		virtual ~AnnAudioFile();

		///Return a raw const pointer to the data, in bytes
		const byte* getData() const;

		///Return the size
		size_t getSize() const override;

		//Virtual IO interface

		///Structure that will contain function pointers to all the functions defined below
		static SF_VIRTUAL_IO* sfVioStruct;

		///Get the length of the file. Give that function pointer to libsndfile.
		static sf_count_t sfVioGetFileLen(void* audioFileRawPtr);

		///Seek (move reading cursor) inside virtual file Give that function pointer to libsndfile.
		static sf_count_t sfVioSeek(sf_count_t offset, int whence, void* audioFileRawPtr);

		///REad "count" bytes from cursor to ptr. Will return number of bytes actually read. Will not read past the end of data but do not check writing to the pointer. Give that function pointer to libsndfile.
		static sf_count_t sfVioRead(void* ptr, sf_count_t count, void* audiFileRawPtr);

		///Do nothing. Dummy function just to fill up the interface. Give that function pointer to libsndfile.
		static sf_count_t sfVioWriteDummy(const void *, sf_count_t, void*);

		///return current cursor position. Give that function pointer to libsndfile.
		static sf_count_t sfVioTell(void* audioFileRawPtr);

		///Get the virtual I/O struct. Will initialize it at 1st call. Give that function pointer to libsndfile.
		static SF_VIRTUAL_IO* getSndFileVioStruct();

		///For cleanup. Will deallocate the VioStruct and set the pointer back to nullptr
		static void clearSndFileVioStruct();

		///Current cursor position. the only "state" used while reading the file from libsndfile (except for the data itself, that is non mutable)
		size_t sf_offset;
	};

	using AnnAudioFilePtr = Ogre::SharedPtr<AnnAudioFile>;

	class AnnAudioFileManager : public Ogre::ResourceManager, public Ogre::Singleton<AnnAudioFileManager>
	{
	protected:
		///Create the audio file resource itself
		Ogre::Resource* createImpl(const Ogre::String &name, Ogre::ResourceHandle handle,
			const Ogre::String &group, bool isManual, Ogre::ManualResourceLoader *loader,
			const Ogre::NameValuePairList *createParams) override;

	public:
		///Construct an AnnAudioFileManager. Will register itsel to the Ogre ResourceGroupManager.
		AnnAudioFileManager();

		///Will unregister itself to the Ogre ResourceGroupManager
		virtual ~AnnAudioFileManager();

		///Load a file via the AudioFileManager
		virtual AnnAudioFilePtr load(const Ogre::String& name, const Ogre::String& group);

		///Get singleton ref
		static AnnAudioFileManager& getSingleton();

		///Get singleton pointer
		static AnnAudioFileManager* getSingletonPtr();
	};
}