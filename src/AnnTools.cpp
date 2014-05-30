#include "AnnTools.h"

using namespace Ogre;



float Annwvyn::Tools::Geometry::distance(Annwvyn::AnnGameObject* a, Annwvyn::AnnGameObject* b)
{
	Vector3 vector(Vector3(0,0,0));

	vector += (b->pos().x - a->pos().x);
	vector += (b->pos().y - a->pos().y);
	vector += (b->pos().z - a->pos().z);

	float dist;

	dist = vector.x * vector.x;
	dist += vector.y * vector.y;
	dist += vector.z * vector.z;

	return sqrt(dist);
}

float Annwvyn::Tools::Geometry::distance(Ogre::Vector3 a, Ogre::Vector3 b)
{
	Vector3 vector = b-a;
	float dist;

	dist = vector.x * vector.x;
	dist += vector.y * vector.y;
	dist += vector.z * vector.z;

	return sqrt(dist);
}
