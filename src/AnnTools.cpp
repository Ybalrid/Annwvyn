#include "stdafx.h"
#include "AnnTools.h"

Annwvyn::phyShapeType Annwvyn::getShapeTypeFromString(std::string str)
{
	if(str == "static")
		return staticShape;
	if(str == "convex")
		return convexShape;
	if(str == "box")
		return boxShape;
	if(str == "cylinder")
		return cylinderShape;
	if(str == "capsule")
		return capsuleShape;
	if(str == "sphere")
		return sphereShape;
	return phyShapeType(0);
}
