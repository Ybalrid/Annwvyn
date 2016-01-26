#ifndef ANN_FILESYSTEM
#define ANN_FILESYSTEM
#include "systemMacro.h"

#include <string>
#include <map>
#include <algorithm>
#include <vector>

using namespace std;

namespace Annwvyn
{
	class DLL AnnFilesystemManager
	{
	public:
		AnnFilesystemManager();
		void setSaveDirectoryName(string name);
		string getPathForFileName(string fileName);
	private:
		string saveDirectoryName;
		string pathToUserDir;
		std::vector<char> charToEscape;
	};

	class DLL AnnSaveFileData
	{
	public:
		AnnSaveFileData();
	private:
		string fileName;
		std::map<string, string> storedData; 
	};
}

#endif