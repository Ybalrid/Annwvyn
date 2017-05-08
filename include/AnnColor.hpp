/**
 * \file AnnColor.hpp
 * \brief Represent an RGBA color. Each channel is stored as a float between 0 and 1
 * \author A. Brainville (Ybalrid)
 */

#ifndef ANNCOLOR
#define ANNCOLOR
#include "systemMacro.h"
#include <iostream>
#include <ostream>
#include <Ogre.h>

namespace Annwvyn
{
	///Class that represent a 4 channel color value
	class DLL AnnColor
	{
	public:
		///Create a color object. Each channel take a float between 0 and 1
		AnnColor(float red, float green, float blue, float alpha = 1.0f);
		///Create a color object from a Ogre::ColourValue
		AnnColor(const Ogre::ColourValue color);
		///Create a color object form a reference to another color object
		AnnColor(const AnnColor& color);

		AnnColor& operator=(const AnnColor& color);

		///Return a Ogre::ColourValue from the internal color value
		Ogre::ColourValue getOgreColor() const;

		///Return the red channel as an integer between 0 and 255
		unsigned char getRedI() const;
		///Return the blue channel as an integer between 0 and 255
		unsigned char getBlueI() const;
		///Return the green channel as an integer between 0 and 255
		unsigned char getGreenI() const;
		///Return the alpha channel as an integer between 0 and 255
		unsigned char getAlphaI() const;

		///Get the red value as float
		float getRed() const;
		///Get the green value as float
		float getGreen() const;
		///Get blue red value as float
		float getBlue() const;
		///Get alpha red value as float
		float getAlpha() const;

		///Set the red value
		void setRed(float red);
		///Set the green value
		void setGreen(float green);
		///Set the blue value
		void setBlue(float blue);
		///Set the alpha value
		void setAlpha(float alpha);

		///Prints color object description to output stream
		DLL friend std::ostream& operator<<(std::ostream& out, const  AnnColor& color)
		{
			out << "AnnColor("
				<< color.r << ", "
				<< color.g << ", "
				<< color.b << ", "
				<< color.a << ")";
			return out;
		}

	protected:
		float r, g, b, a;

	private:
		static constexpr const unsigned char MAX8BIT = 0xFF;
		///Return f*MAX8BIT
		static unsigned char f2i(float f);
		///Return true if f is inside interval [0;1]
		static bool isInFloatRange(float f);
	};
}
#endif //ANNCOLOR