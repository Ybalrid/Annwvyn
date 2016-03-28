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

		///Return a Ogre::ColourValue from the internal color value
		Ogre::ColourValue getOgreColor();
		
		///Return the red channel as an integer betwenn 0 and 255
		unsigned char getRedI();
		///Return the blue channel as an integer betwenn 0 and 255
		unsigned char getBlueI();
		///Return the green channel as an integer betwenn 0 and 255
		unsigned char getGreenI();
		///Return the alpha channel as an integer betwenn 0 and 255
		unsigned char getAlphaI();

		///Get the red value as float 
		float getRed();
		///Get the green value as float 
		float getGreen();
		///Get blue red value as float 
		float getBlue();
		///Get alpha red value as float 
		float getAlpha();

		///Set the red value
		void setRed(float red);
		///Set the green value
		void setGreen(float green);
		///Set the blue value
		void setBlue(float blue);
		///Set the alpha value
		void setAlpha(float alpha);

		///Prints color object description to output stream
		DLL friend std::ostream& operator<<(std::ostream& out,const  AnnColor& color)
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
		const unsigned char MAX8BIT;
		///Return f*MAX8BIT
		unsigned char f2i(float f);
		///Return true if f is inside interval [0;1]
		bool isInFloatRange(float f);
	};


}
#endif //ANNCOLOR
