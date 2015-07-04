#include "stdafx.h"
#include "AnnTools.h"

using namespace Ogre;

float Annwvyn::Tools::Geometry::distance(Annwvyn::AnnGameObject* a, Annwvyn::AnnGameObject* b)
{
    return Annwvyn::Tools::Geometry::distance(a->pos(), b->pos());
}

float Annwvyn::Tools::Geometry::distance(Ogre::Vector3 a, Ogre::Vector3 b)
{
	//get a vector representing translation between the two objects
	Vector3 vector = b-a; 
	
	//store math result :
	float dist;
	
	//do dist = x² + y² + z²
	dist = vector.x * vector.x;
	dist += vector.y * vector.y;
	dist += vector.z * vector.z;
	
    return sqrt(dist); 
}
