#include "stdafx.h"
#include "AnnFilesystem.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;
using namespace std;

AnnFileWriter::AnnFileWriter()
{

}

void AnnFileWriter::write(AnnSaveFileData* data)
{
	auto fsmanager(AnnEngine::Instance()->getFileSystemManager());
	fsmanager->createSaveDirectory();
	string path(fsmanager->getPathForFileName(data->fileName));
	ofstream saveFile;
	saveFile.open(path);
	if(!saveFile.is_open())return;
	for(auto storedData : data->storedTextData)
	{
		saveFile << storedData.first 
			<< "=" 
			<< storedData.second 
			<< endl;
	}
}

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

	fileWriter = new AnnFileWriter;

}

AnnFilesystemManager::~AnnFilesystemManager()
{
	delete fileWriter;
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

string AnnFilesystemManager::getSaveDirectoryFullPath()
{
	return pathToUserDir + "/" + saveDirectoryName;
}

void AnnFilesystemManager::createDirectory(string path)
{
#ifdef WIN32
	CreateDirectory(wstring(path.begin(), path.end()).c_str(), NULL);
#endif
}

void AnnFilesystemManager::createSaveDirectory()
{
	createDirectory(getSaveDirectoryFullPath());
}

AnnSaveFileData::AnnSaveFileData(string name) :
	fileName(name)
{
}

