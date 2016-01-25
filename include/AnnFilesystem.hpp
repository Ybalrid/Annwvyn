#ifndef ANN_FILESYSTEM
#define ANN_FILESYSTEM
#include "systemMacro.h"

#include <string>
#include <map>
using namespace std;
namespace Annwvyn
{
	class DLL AnnFilesystemManager
	{
	public:
		AnnFilesystemManager();
		void setSaveDirectoryName(string name);
		void getPathForFileName(string fileName);
	private:
		string saveDirectoryName;
		string pathToUserDir;
	};

	class DLL AnnSaveFileData
	{
	public:
		AnnSaveFileData();
	private:
		string fileName;
		map<string, string> storedData; 
	};
}

#endif