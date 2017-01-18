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
		AnnStringUility() :
			//Allocate the string hasher
			stringHasher(std::make_unique<std::hash<std::string>>()),
			//Seed the 64bit Mersenne Twister generator with the random device
			mt(r())
		{
		}

		size_t hash(const std::string& string) const
		{
			return (*stringHasher)(string);
		}

		std::string getRandomString(size_t length = 15U)
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
	private:
		std::unique_ptr<std::hash<std::string>> stringHasher;
		std::random_device r;
		std::mt19937_64 mt;
	};
}