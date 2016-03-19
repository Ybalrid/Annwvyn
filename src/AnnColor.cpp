#include "stdafx.h"
#include "AnnColor.hpp"

using namespace Annwvyn;

AnnColor::AnnColor(float red, float green, float blue, float alpha) : 
	r(red),
	g(green),
	b(blue),
	a(alpha),
	MAX8BIT(255)
{
}

Ogre::ColourValue AnnColor::getOgreColor()
{
	return Ogre::ColourValue(r,g,b,a);
}

float AnnColor::getRed()
{
	return r;
}

float AnnColor::getGreen()
{
	return g;
}

float AnnColor::getBlue()
{
	return b;
}

float AnnColor::getAlpha()
{
	return a;
}

unsigned char AnnColor::f2i(float f)
{
	return static_cast<unsigned char>(f * MAX8BIT); 

}

unsigned char AnnColor::getRedI()
{
	return f2i(r);
}

unsigned char AnnColor::getGreenI()
{
	return f2i(g);
}

unsigned char AnnColor::getBlueI()
{
	return f2i(b);
}

unsigned char AnnColor::getAlphaI()
{
	return f2i(a);
}

bool AnnColor::isInFloatRange(float f)
{
	return (f >= 0.0f && f <= 1.0f);
}

void AnnColor::setRed(float red)
{
	if(isInFloatRange(red))
		r = red;
}

void AnnColor::setGreen(float green)
{
	if(isInFloatRange(green))
		g = green;
}

void AnnColor::setBlue(float blue)
{
	if(isInFloatRange(blue))
		b = blue;
}

void AnnColor::setAlpha(float alpha)
{
	if(isInFloatRange(alpha))
		a = alpha;
}

