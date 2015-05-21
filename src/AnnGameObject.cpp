#include "stdafx.h"
#include "AnnGameObject.hpp"
#include "AnnTools.h"
#include "AnnEngine.hpp"

using namespace Annwvyn;

AnnGameObject::AnnGameObject()
{
    m_node = NULL;
    m_entity = NULL;
    bulletReady = false;
    m_DynamicsWorld = NULL;
    m_Body = NULL;
    m_Shape = NULL;
    alGenSources(1,&m_Source);
    m_anim = NULL;
    animIsLooping = false;
    animIsPlaying = false;
    animIsSetted = false;
	visible = true;
}

AnnGameObject::~AnnGameObject()
{
	//Clean OpenAL desaloc
    alSourceStop(m_Source);
    alDeleteSources(1,&m_Source);
    alDeleteBuffers(1,&m_Buffer);
}

void AnnGameObject::playSound(std::string path, bool loop, float volume)
{
	//Load a sound file to the buffer (uncompress the file to the RAM)
    m_Buffer = AnnEngine::Instance()->getAudioEngine()->loadSndFile(path);

	//create a source to the buffer
    alSourcei(m_Source, AL_BUFFER, m_Buffer);

    if(loop)
        alSourcei(m_Source, AL_LOOPING, AL_TRUE);
    
	alSourcef(m_Source, AL_GAIN, volume);

    alSource3f(m_Source, AL_POSITION,
            m_node->getPosition().x,
            m_node->getPosition().y,
            m_node->getPosition().z);


    alSourcePlay(m_Source);
}

void AnnGameObject::updateOpenAlPos()
{
    alSource3f(m_Source, AL_POSITION,
            pos().x,
            pos().y,
            pos().z);
}

void AnnGameObject::setPos(float x, float y, float z)
{
    if(m_node == NULL)
        return;
    /*
     *Position of object have to be the same in each part of the engine
     *(graphics, physics, audio)
     */
    //change BulletPosition
    if(bulletReady)
    {
        m_Body->translate(btVector3(x - m_node->getPosition().x,
                    y - m_node->getPosition().y,
                    z - m_node->getPosition().z));
    }
    //change OgrePosition
    m_node->setPosition(x,y,z);


    //change OpenAL Source Position
    alSource3f(m_Source, AL_POSITION,
            m_node->getPosition().x,
            m_node->getPosition().y,
            m_node->getPosition().z);
}

void AnnGameObject::translate(float x, float y, float z)		  
{
    //Ogre
    m_node->translate(x,y,z);
    //Bullet
    m_Body->translate(btVector3(x,y,z));
    //OpenAL
    alSource3f(m_Source, AL_POSITION,
            m_node->getPosition().x,
            m_node->getPosition().y,
            m_node->getPosition().z);

}

void AnnGameObject::setPos(AnnVect3 pos)
{
    setPos(pos.x,pos.y,pos.z);
}

void AnnGameObject::setOrientation(float w, float x, float y, float z)
{
	setOrientation(AnnQuaternion(w,x,y,z));
	
}

void AnnGameObject::setOrientation(AnnQuaternion orient)
{
	//setOrientation(orient.w,orient.x,orient.y,orient.z);
	//beware : Ogre Quaternion convetion is WXYZ. Bullet use XYZW
    //Ogre3D
    if(m_node != NULL)
        m_node->setOrientation(orient);
    //bullet
    if(m_Body != NULL)
    {
        btTransform t = m_Body->getCenterOfMassTransform();
        t.setRotation(orient.getBtQuaternion());
        m_Body->setCenterOfMassTransform(t);
    }
}

void AnnGameObject::setScale(AnnVect3 scale)
{
	m_node->setScale(scale);
}


void AnnGameObject::setScale(float x, float y, float z)
{
    m_node->setScale(AnnVect3(x,y,z));
}

AnnVect3 AnnGameObject::pos()
{
    if(m_node != NULL)
        return m_node->getPosition();
    return AnnVect3(0,0,0);
}

AnnQuaternion AnnGameObject::Orientation()
{
    if(m_node != NULL)
        return m_node->getOrientation();
    return AnnQuaternion(1,0,0,0);
}

void AnnGameObject::setNode(Ogre::SceneNode* node)
{
    m_node = node;
}

void AnnGameObject::setEntity(Ogre::Entity* entity)
{
    m_entity = entity;
}

void AnnGameObject::setBulletDynamicsWorld(btDiscreteDynamicsWorld* dynamicsWorld)
{
    m_DynamicsWorld = dynamicsWorld;
}

void AnnGameObject::setUpBullet(float mass, phyShapeType type, bool colideWithPlayer)
{
    //check if everything is OK
    if(m_DynamicsWorld == NULL)
        return;
    if(m_node == NULL)
        return;
    if(m_entity == NULL)
        return;

    //init shape converter
    BtOgre::StaticMeshToShapeConverter converter(m_entity);

    //create the correct shape
    switch(type)
    {
        case boxShape:
            m_Shape = converter.createBox();
            break;

        case cylinderShape:
            m_Shape = converter.createCylinder();
            break;

        case capsuleShape:
            m_Shape = converter.createCapsule();
            break;

        case convexShape:
            m_Shape = converter.createConvex();
            break;

        case staticShape:
            m_Shape = converter.createTrimesh();
            break;
		case sphereShape:
			m_Shape = converter.createSphere();
			break;
        default:
			//non valid;
            return;
    }

    if(m_Shape == NULL)
        return;

    AnnVect3 scale =  node()->getScale();
	m_Shape->setLocalScaling(scale.getBtVector());

    btVector3 inertia;

    if(mass != 0)
        m_Shape->calculateLocalInertia(mass, inertia);
    else
        inertia = btVector3(0,0,0); //No influence. But mass zero objects are static

	

    //create rigidBody from shape
	if(!m_Body)
	{
		BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(m_node);
		m_Body = new btRigidBody(mass, state, m_Shape, inertia);
	}

    if(m_Body != NULL)
		if(colideWithPlayer)
        {
			m_DynamicsWorld->addRigidBody(m_Body, BIT(1), BIT(0) | BIT(1));
		}
		else
		{
			m_DynamicsWorld->addRigidBody(m_Body, BIT(1), BIT(1));
		}
    else
        return; //Unable to create the physical representation

    bulletReady = true;
}

Ogre::SceneNode* AnnGameObject::node()
{
    return m_node;
}

Ogre::Entity* AnnGameObject::Entity()
{
    return m_entity;
}

btRigidBody* AnnGameObject::RigidBody()
{
    return m_Body;
}

float AnnGameObject::getDistance(AnnGameObject *otherObject)
{
    return Tools::Geometry::distance(this, otherObject);
}

btRigidBody* AnnGameObject::getBody()
{
    return m_Body;
}

std::vector<struct collisionTest*> AnnGameObject::getCollisionMask()
{
    return collisionMask;
}

bool AnnGameObject::collideWith(AnnGameObject* Object)
{
    for(size_t i = 0; i < collisionMask.size(); i++)
        if(collisionMask[i]->Object == Object)
            return collisionMask[i]->collisionState;
    return false;
}

void AnnGameObject::updateCollisionStateWith(AnnGameObject* Object, bool state)
{
    for(size_t i = 0; i < collisionMask.size(); i++)
        if(collisionMask[i]->Object == Object)
            collisionMask[i]->collisionState = state;
}

void AnnGameObject::cleanCollisionMask()
{
    for(size_t i = 0; i < collisionMask.size(); i++)
    {
        delete collisionMask[i];
        collisionMask.erase(collisionMask.begin()+i);
    }
}

void AnnGameObject::resetCollisionMask()
{
    for (size_t i = 0; i < collisionMask.size(); i++)
        collisionMask[i]->collisionState = false;
}

void AnnGameObject::testCollisionWith(AnnGameObject* Object)
{
    if(Object == this) return; //Explain me how I can colide with myself o.O

    struct collisionTest* tester = new collisionTest;

    tester->collisionState = false;
    tester->Object = Object;

    tester->Receiver = this;

    collisionMask.push_back(tester);
}

void AnnGameObject::stopGettingCollisionWith(AnnGameObject* Object)
{
	if(!Object) return;
    for(size_t i = 0; i < collisionMask.size(); i++)
        if(collisionMask[i]->Object == Object)
        {
            delete(collisionMask[i]);
			collisionMask[i] = NULL;
            //collisionMask.erase(collisionMask.begin()+i);
        }
}

void AnnGameObject::setAnimation(const char name[])
{
    if(animIsSetted)
    {
        m_anim->setEnabled(false);
        m_anim->setLoop(false);
        animIsSetted = false;
        animIsLooping = false;
        animIsPlaying = false;
        m_anim = NULL;
    }

    m_anim = m_entity->getAnimationState(name);
    if (m_anim != NULL)
        animIsSetted = true;
}

void AnnGameObject::playAnimation(bool play)
{
    if(animIsSetted)
    {
        m_anim->setEnabled(play);
        animIsPlaying = play;
    }
}

void AnnGameObject::loopAnimation(bool loop)
{
    if(animIsSetted)
    {
        m_anim->setLoop(loop);
        animIsLooping = loop;
    }
}

void AnnGameObject::addTime(double offset)
{
    if(!animIsSetted || !animIsPlaying)
        return;

    m_anim->addTime(float(offset));
}

void AnnGameObject::applyImpulse(AnnVect3 force)
{
	m_Body->applyCentralImpulse(force.getBtVector());
}

void AnnGameObject::applyForce(AnnVect3 force)
{
   m_Body->applyCentralForce(force.getBtVector());
}

void AnnGameObject::setLinearSpeed(AnnVect3 v)
{
	if(bulletReady)
		m_Body->setLinearVelocity(v.getBtVector());
}


void AnnGameObject::setVisible()
{
	visible = true;
	node()->setVisible(true);
}

void AnnGameObject::setInvisible()
{
	visible = false;
	node()->setVisible(false);
}

bool AnnGameObject::isVisible()
{
	return visible;
}
