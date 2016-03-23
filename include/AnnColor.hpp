#ifndef ANNCOLOR
#define ANNCOLOR
#include "systemMacro.h"
#include <iostream>
#include <ostream>
#include <Ogre.h>

namespace Annwvyn
{

	class DLL AnnColor
	{
	public:
		AnnColor(float red, float green, float blue, float alpha = 1.0f);
		AnnColor(const Ogre::ColourValue color);
		Ogre::ColourValue getOgreColor();
		
		unsigned char getRedI();
		unsigned char getBlueI();
		unsigned char getGreenI();
		unsigned char getAlphaI();

		float getRed();
		float getGreen();
		float getBlue();
		float getAlpha();

		void setRed(float red);
		void setGreen(float green);
		void setBlue(float blue);
		void setAlpha(float alpha);

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
		unsigned char f2i(float f);
		bool isInFloatRange(float f);
	};


}
#endif //ANNCOLOR
