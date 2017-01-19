#pragma once

#include "systemMacro.h"
#include <string>
#include <memory>
#include <functional>

#include <random>

namespace Annwvyn
{
	class DLL AnnStringUility
	{
	public:
		///Construct the string utility
		AnnStringUility();
		///Hash a string with the built-in hash algorithm of C++11
		size_t hash(const std::string& string) const;
		///Get a string of random characters of specified length. 15 char by default
		std::string getRandomString(size_t length = 15U);

	private:
		///Hasher
		std::unique_ptr<std::hash<std::string>> stringHasher;
		///Random device, to seed the mt engine
		std::random_device r;
		///mt engine
		std::mt19937_64 mt;
	};
}