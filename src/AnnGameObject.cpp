#include "AnnGameObject.hpp"
#include "AnnTools.h"


using namespace Annwvyn;

AnnGameObject::AnnGameObject()
{
    m_node = NULL;
    m_entity = NULL;
    bulletReady = false;
    m_DynamicsWorld = NULL;
    m_Body = NULL;
    m_Shape = NULL;
    m_AudioEngine = NULL;
    alGenSources(1,&m_Source);
    m_anim = NULL;
    animIsLooping = false;
    animIsPlaying = false;
    animIsSetted = false;
}

AnnGameObject::~AnnGameObject()
{
    alSourceStop(m_Source);
    alDeleteSources(1,&m_Source);
    alDeleteBuffers(1,&m_Buffer);
}


void AnnGameObject::setAudioEngine(AnnAudioEngine* AudioEngine)
{
    m_AudioEngine = AudioEngine;
}

void AnnGameObject::playSound(std::string path, bool loop, float volume)
{
    m_Buffer = m_AudioEngine->loadSndFile(path);


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

void AnnGameObject::setPos(Ogre::Vector3 pos)
{
    setPos(pos.x,pos.y,pos.z);
}

void AnnGameObject::setOrientation(float w, float x, float y, float z)
{
    //Ogre3D
    if(m_node != NULL)
        m_node->setOrientation(w,x,y,z);
    //bullet
    if(m_Body != NULL)
    {
        btTransform t = m_Body->getCenterOfMassTransform();
        t.setRotation(btQuaternion(x,y,z,w));
        m_Body->setCenterOfMassTransform(t);
    }
    //OpenAL
}

void AnnGameObject::setOrientation(Ogre::Quaternion orient)
{
    setOrientation(orient.w,orient.x,orient.y,orient.z);
}

void AnnGameObject::setScale(Ogre::Vector3 scale)
{
    setScale(scale.x,scale.y,scale.z);
}


void AnnGameObject::setScale(float x, float y, float z)
{
    m_node->setScale(Ogre::Vector3(x,y,z));
}

Ogre::Vector3 AnnGameObject::pos()
{
    if(m_node != NULL)
        return m_node->getPosition();
    return Ogre::Vector3(0,0,0);
}

Ogre::Quaternion AnnGameObject::Orientation()
{
    if(m_node != NULL)
        return m_node->getOrientation();
    return Ogre::Quaternion(1,0,0,0);
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


void AnnGameObject::setUpBullet(float mass, phyShapeType type)
{
    //check if everything is OK
    if(m_DynamicsWorld == NULL)
        return;
    if(m_node == NULL)
        return;
    if(m_entity == NULL)
        return;

    //init shap converter
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
        default:
            return;
    }

    if(m_Shape == NULL)
        return;

    Ogre::Vector3 scale =  node()->getScale();
    m_Shape->setLocalScaling(btVector3(scale.x,scale.y,scale.z));

    btVector3 inertia;
    if(mass != 0)
        m_Shape->calculateLocalInertia(mass, inertia);
    else
        inertia = btVector3(0,0,0);

    BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(m_node);

    //create rigidBody from shape
    m_Body = new btRigidBody(mass,state,m_Shape,inertia);

    if(m_Body != NULL)
    {
        /*		m_Body->translate(btVector3(this->node()->getPosition().x,
                this->node()->getPosition().y,
                this->node()->getPosition().z));*/

        m_DynamicsWorld->addRigidBody(m_Body);
    }	
    else
        return;

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
    return Tools::Geometry::distance(this,otherObject);
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
    if(Object == this) return; //Explain me how I can colide with myself.

    struct collisionTest* tester = new collisionTest;

    tester->collisionState = false;
    tester->Object = Object;

    tester->Receiver = this;

    collisionMask.push_back(tester);
}

void AnnGameObject::stopGettingCollisionWith(AnnGameObject* Object)
{
    for(size_t i = 0; i < collisionMask.size(); i++)
        if(collisionMask[i]->Object == Object)
        {
            delete(collisionMask[i]);
            collisionMask.erase(collisionMask.begin()+i);
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

void AnnGameObject::addTime(float offset)
{
    if(!animIsSetted || !animIsPlaying)
        return;
    m_anim->addTime(offset);
}

void AnnGameObject::applyImpulse(Ogre::Vector3 force)
{
    m_Body->applyCentralImpulse(btVector3(force.x,force.y,force.z));
}

void AnnGameObject::applyForce(Ogre::Vector3 force)
{
   m_Body->applyCentralForce(btVector3(force.x,force.y,force.z));
}

