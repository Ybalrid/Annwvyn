#ifndef ANNCOLOR
#define ANNCOLOR
#include "systemMacro.h"
#include <Ogre.h>

namespace Annwvyn
{
	class DLL AnnColor
	{
	public:
		AnnColor(float red, float green, float blue, float alpha = 1.0f);
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

	protected:
		float r, g, b, a;

	private:
		const unsigned char MAX8BIT;
		unsigned char f2i(float f);
		bool isInFloatRange(float f);
	};

}
#endif //ANNCOLOR
