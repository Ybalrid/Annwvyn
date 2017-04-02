/*
 * =====================================================================================
 *
 *       Filename:  BtOgreGP.h
 *
 *    Description:  The part of BtOgre that handles information transfer from Ogre to
 *                  Bullet (like mesh data for making trimeshes).
 *
 *        Version:  1.0
 *        Created:  27/12/2008 03:29:56 AM
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#ifndef _BtOgrePG_H_
#define _BtOgrePG_H_

#include "systemMacro.h"

#include "btBulletDynamicsCommon.h"
#include "BtOgreExtras.h"
#include "Ogre.h"

namespace BtOgre {

typedef std::map<unsigned char, Vector3Array*> BoneIndex;
typedef std::pair<unsigned short, Vector3Array*> BoneKeyIndex;

class DLL VertexIndexToShape
{
public:
	VertexIndexToShape(const Ogre::Matrix4 &transform = Ogre::Matrix4::IDENTITY);
	~VertexIndexToShape();

	Ogre::Real getRadius();
	Ogre::Vector3 getSize();


	btSphereShape* createSphere();
	btBoxShape* createBox();
	btBvhTriangleMeshShape* createTrimesh();
	btCylinderShape* createCylinder();
	btConvexHullShape* createConvex();
	btCapsuleShape* createCapsule();

	const Ogre::Vector3* getVertices();
	unsigned int getVertexCount();
	const unsigned int* getIndices();
	unsigned int getIndexCount();

protected:

	void addStaticVertexData(const Ogre::v1::VertexData *vertex_data);

	void addAnimatedVertexData(const Ogre::v1::VertexData *vertex_data,
		const Ogre::v1::VertexData *blended_data,
		const Ogre::v1::Mesh::IndexMap *indexMap);

	void addIndexData(Ogre::v1::IndexData *data, const unsigned int offset = 0);


protected:
	Ogre::Vector3*	    mVertexBuffer;
	unsigned int*       mIndexBuffer;
	unsigned int        mVertexCount;
	unsigned int        mIndexCount;

	Ogre::Vector3		mBounds;
	Ogre::Real		    mBoundRadius;

	BoneIndex           *mBoneIndex;

	Ogre::Matrix4		mTransform;

	Ogre::Vector3		mScale;
};

//For static (non-animated) meshes.
class DLL StaticMeshToShapeConverter : public VertexIndexToShape
{
public:

	StaticMeshToShapeConverter(Ogre::Renderable *rend, const Ogre::Matrix4 &transform = Ogre::Matrix4::IDENTITY);
	StaticMeshToShapeConverter(Ogre::v1::Entity *entity, const Ogre::Matrix4 &transform = Ogre::Matrix4::IDENTITY);
	StaticMeshToShapeConverter(Ogre::v1::Mesh *mesh, const Ogre::Matrix4 &transform = Ogre::Matrix4::IDENTITY);
	StaticMeshToShapeConverter();

	~StaticMeshToShapeConverter();

	void addEntity(Ogre::v1::Entity *entity,const Ogre::Matrix4 &transform = Ogre::Matrix4::IDENTITY);

	void addMesh(const Ogre::v1::Mesh *mesh, const Ogre::Matrix4 &transform = Ogre::Matrix4::IDENTITY);


protected:

	Ogre::v1::Entity*		mEntity;
	Ogre::SceneNode*	mNode;
};

//For animated meshes.
class DLL AnimatedMeshToShapeConverter : public VertexIndexToShape
{
public:

	AnimatedMeshToShapeConverter(Ogre::v1::Entity *entity, const Ogre::Matrix4 &transform = Ogre::Matrix4::IDENTITY);
	AnimatedMeshToShapeConverter();
	~AnimatedMeshToShapeConverter();

	void addEntity(Ogre::v1::Entity *entity,const Ogre::Matrix4 &transform = Ogre::Matrix4::IDENTITY);
	void addMesh(const Ogre::v1::MeshPtr &mesh, const Ogre::Matrix4 &transform);

	btBoxShape* createAlignedBox(unsigned char bone,
		const Ogre::Vector3 &bonePosition,
		const Ogre::Quaternion &boneOrientation);

	btBoxShape* createOrientedBox(unsigned char bone,
		const Ogre::Vector3 &bonePosition,
		const Ogre::Quaternion &boneOrientation);

protected:

	bool getBoneVertices(unsigned char bone,
		unsigned int &vertex_count,
		Ogre::Vector3* &vertices,
		const Ogre::Vector3 &bonePosition);

	bool getOrientedBox(unsigned char bone,
		const Ogre::Vector3 &bonePosition,
		const Ogre::Quaternion &boneOrientation,
		Ogre::Vector3 &extents,
		Ogre::Vector3 *axis,
		Ogre::Vector3 &center);


	Ogre::v1::Entity*		mEntity;
	Ogre::SceneNode*	mNode;

	Ogre::Vector3       *mTransformedVerticesTemp;
	size_t               mTransformedVerticesTempSize;
};

}

#endif
