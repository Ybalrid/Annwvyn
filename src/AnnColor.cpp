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

AnnColor::AnnColor(Ogre::ColourValue color) :
	r(color.r),
	g(color.g),
	b(color.b),
	a(color.a),
	MAX8BIT(255)
{
}

AnnColor::AnnColor(const AnnColor & color) :
	r(color.r),
	g(color.g),
	b(color.b),
	a(color.a),
	MAX8BIT(255)
{
}

Ogre::ColourValue AnnColor::getOgreColor() const
{
	return Ogre::ColourValue(r, g, b, a);
}

float AnnColor::getRed() const
{
	return r;
}

float AnnColor::getGreen() const
{
	return g;
}

float AnnColor::getBlue() const
{
	return b;
}

float AnnColor::getAlpha() const
{
	return a;
}

unsigned char AnnColor::f2i(float f) const
{
	return static_cast<unsigned char>(f * MAX8BIT);
}

unsigned char AnnColor::getRedI() const
{
	return f2i(r);
}

unsigned char AnnColor::getGreenI() const
{
	return f2i(g);
}

unsigned char AnnColor::getBlueI() const
{
	return f2i(b);
}

unsigned char AnnColor::getAlphaI() const
{
	return f2i(a);
}

bool AnnColor::isInFloatRange(float f)
{
	return (f >= 0.0f && f <= 1.0f);
}

void AnnColor::setRed(float red)
{
	if (isInFloatRange(red))
		r = red;
}

void AnnColor::setGreen(float green)
{
	if (isInFloatRange(green))
		g = green;
}

void AnnColor::setBlue(float blue)
{
	if (isInFloatRange(blue))
		b = blue;
}

void AnnColor::setAlpha(float alpha)
{
	if (isInFloatRange(alpha))
		a = alpha;
}