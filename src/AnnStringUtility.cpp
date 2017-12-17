// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "stdafx.h"
#include "AnnStringUtility.hpp"
#include "AnnLogger.hpp"
using namespace Annwvyn;

AnnStringUility::AnnStringUility() :
	//Allocate the string hasher
	stringHasher(std::make_unique<std::hash<std::string>>()),
	//Seed the 64bit Mersenne Twister generator with the random device
	mt(r())
{
	AnnDebug() << "AnnStringUtility created";
}

AnnStringUility::~AnnStringUility()
{
	AnnDebug() << "AnnStringUtility destructed";
}

size_t AnnStringUility::hash(const std::string& string) const
{
	return (*stringHasher)(string);
}

std::string AnnStringUility::getRandomString(size_t length)
{
	static const std::string charset{ "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_" };
	static const auto maxNbChar{ charset.size() };
	static std::uniform_int_distribution<size_t> distribution(0, maxNbChar - 1);

	std::string output;
	//No need to push_back X times if we already know the length;
	output.resize(length);
	for (size_t i{ 0 }; i < length; i++)
		output[i] = charset[distribution(mt)];

	return output;
}