#include "stdafx.h"
#include "AnnFilesystem.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;
using namespace std;

AnnFilesystemManager::AnnFilesystemManager() 
{
	AnnDebug() << "Filesystem manager created";
	//get from the OS the user's personal directory
#ifdef WIN32
	pathToUserDir = getenv("USERPROFILE");
#endif

	AnnDebug() << "Path got from operating system : " << pathToUserDir;

	charToEscape.push_back(' ');
	charToEscape.push_back('&');
	charToEscape.push_back('<');
	charToEscape.push_back('>');
	charToEscape.push_back(':');
	charToEscape.push_back('|');
	charToEscape.push_back('?');
	charToEscape.push_back('*');
	charToEscape.push_back('"');
	charToEscape.push_back(':');
	charToEscape.push_back('%');

}

void AnnFilesystemManager::setSaveDirectoryName(string dirname)
{
	for(auto achar : charToEscape)
		replace(dirname.begin(), dirname.end(), achar, '_');
	saveDirectoryName = dirname;
	AnnDebug() << "Path example : " << getPathForFileName("example");
}

string AnnFilesystemManager::getPathForFileName(string filename)
{
	if(!pathToUserDir.empty())
	return pathToUserDir + "/" + saveDirectoryName + "/" + filename;
	return "";
}

AnnSaveFileData::AnnSaveFileData(string name) :
	fileName(name)
{
}

