#include "stdafx.h"
#include "engineBootstrap.hpp"

namespace Annwvyn
{
	TEST_CASE("FileSystem subsystem sanityCheck")
	{
		auto GameEngine = bootstrapTestEngine("TestFileSystem");

		auto fileSystemManager = AnnGetFileSystemManager();
		REQUIRE(fileSystemManager);
		REQUIRE(fileSystemManager->getFileReader());
		REQUIRE(fileSystemManager->getFileReader());
	}

	TEST_CASE("FileSystem attempting to read non-existing file")
	{
		auto GameEngine = bootstrapTestEngine("TestFileSystem");
		auto doNotExistFileSystemDataObject = AnnGetFileSystemManager()->getFileReader()->read("doNotExist");
		REQUIRE_FALSE(doNotExistFileSystemDataObject);
	}

	TEST_CASE("FileSystem write data to save file and read it back")
	{
		auto GameEngine = bootstrapTestEngine("TestFileSystem");
		auto fsManager = AnnGetFileSystemManager();
		//Write
		{
			auto fileData = fsManager->crateSaveFileDataObject("TestSave");
			REQUIRE(fileData);
			fileData->setValue("IAmTestProgram", 1);
			fsManager->getFileWriter()->write(fileData);
			fsManager->releaseSaveFileDataObject(fileData);
		}

		//Read
		{
			auto fileData = fsManager->getFileReader()->read("TestSave");
			REQUIRE(fileData->getValue("IAmTestProgram") == "1");
			fsManager->releaseSaveFileDataObject(fileData);
		}

		//Declare a file data interpretor for this save file (by nesting a class...)
		class TestFileSystemDataInterpretror : public AnnSaveDataInterpretor
		{
		public:
			TestFileSystemDataInterpretror(std::shared_ptr<AnnSaveFileData> data) : AnnSaveDataInterpretor(data),
				IAmTestProgram(false) {}

			bool amITestProgram() const
			{
				return IAmTestProgram;
			}

			void extract() override
			{
				IAmTestProgram = keyStringToInt("IAmTestProgram");
			}

		private:
			bool IAmTestProgram;
		};

		//Using a data interpretor
		{
			auto fileData = AnnGetFileSystemManager()->getFileReader()->read("TestSave");
			auto dataInterpretor = TestFileSystemDataInterpretror(fileData);
			dataInterpretor.extract();
			REQUIRE(dataInterpretor.amITestProgram());
			AnnGetFileSystemManager()->releaseSaveFileDataObject(fileData);
		}
	}
}