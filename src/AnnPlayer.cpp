#include "AnnPlayer.hpp"

using namespace Annwvyn;

bodyParams::bodyParams()
{
    //these parameters looks good for testing. Costumise them before initializing the physics!
    eyeHeight = 1.59f;
    walkSpeed = 3.0f;
    turnSpeed = 0.003f;
    mass = 80.0f;
    Position = Ogre::Vector3(0,0,10);
    HeadOrientation = Ogre::Quaternion(1,0,0,0);
    Shape = NULL;
    Body = NULL;
    runFactor = 5;
    jumpForce = btVector3(0,500,0);
}

AnnPlayer::AnnPlayer()
{
    playerBody = new bodyParams;
    locked = false;
    run = false;

    walking[forward] = false;
    walking[backward] = false;
    walking[left] = false;
    walking[right] = false;
}

AnnPlayer::~AnnPlayer()
{
    delete playerBody;
}

bool AnnPlayer::isLocked()
{
    return locked;
}

void AnnPlayer::setPosition(Ogre::Vector3 Position)
{
    playerBody->Position = Position;
}

void AnnPlayer::setOrientation(Ogre::Euler Orientation)
{
    playerBody->Orientation = Orientation;
}

void AnnPlayer::setHeadOrientation(Ogre::Quaternion Orientation)
{
    playerBody->HeadOrientation = Orientation;
}

void AnnPlayer::setEyesHeight(float eyeHeight)
{
    if(!isLocked())
        playerBody->eyeHeight = eyeHeight;
}

void AnnPlayer::setWalkSpeed(float walk)
{
    if(!isLocked())
        playerBody->eyeHeight = walk;
}

void AnnPlayer::setTurnSpeed(float ts)
{
    if(!isLocked())
        playerBody->eyeHeight = ts;
}

void AnnPlayer::setMass(float mass)
{
    if(!isLocked())
        playerBody->eyeHeight = mass;
}

void AnnPlayer::setShape(btCollisionShape* Shape)
{
    if(!isLocked())
        playerBody->Shape = Shape;
}

void AnnPlayer::setBody(btRigidBody* Body)
{
    if(!isLocked())
        playerBody->Body = Body;
}

void AnnPlayer::lockParameters()
{
    locked = true;
}

void AnnPlayer::unlockParameters()
{
    locked = true;
}

bodyParams* AnnPlayer::getLowLevelBodyParams()
{
    return playerBody;
}

float AnnPlayer::getWalkSpeed()
{
    return playerBody->walkSpeed;
}

float AnnPlayer::getEyesHeight()
{
    return playerBody->eyeHeight;
}

float AnnPlayer::getTurnSpeed()
{
    return playerBody->turnSpeed;
}

float AnnPlayer::getMass()
{
    return playerBody->mass;
}

btRigidBody* AnnPlayer::getBody()
{
    return playerBody->Body;
}

btCollisionShape* AnnPlayer::getShape()
{
    return playerBody->Shape;
}

Ogre::Vector3 AnnPlayer::getPosition()
{
    return playerBody->Position;
}

Ogre::Euler AnnPlayer::getOrientation()
{
    return playerBody->Orientation;
}

void AnnPlayer::applyRelativeBodyYaw(Ogre::Radian angle)
{
    playerBody->Orientation.yaw(angle);
}

void AnnPlayer::applyMouseRelativeRotation(int relValue)
{
	applyRelativeBodyYaw(Ogre::Radian(-float(relValue)*getTurnSpeed()));
}


void AnnPlayer::setLinearSpeed(Ogre::Vector3 v)
{
    if(!playerBody->Body) return;
    playerBody->Body->setLinearVelocity(btVector3(v.x, v.y, v.z));
}

void AnnPlayer::killLinearSpeed()
{
    if(!playerBody->Body) return;
    playerBody->Body->setLinearVelocity(btVector3(0, 0, 0));
}

void AnnPlayer::addLinearSpeed(Ogre::Vector3 v)
{
    if(!playerBody->Body) return;
    playerBody->Body->setLinearVelocity(
            playerBody->Body->getLinearVelocity() 
            + btVector3(v.x, v.y, v.z));

}

Ogre::Vector3 AnnPlayer::getTranslation()
{

    Ogre::Vector3 translation (Ogre::Vector3::ZERO);
    if(walking[forward])
        translation.z -= getWalkSpeed();
    if(walking[backward])
        translation.z += getWalkSpeed();
    if(walking[left])
        translation.x -= getWalkSpeed();
    if(walking[right])
        translation.x += getWalkSpeed();


    return translation;
}

void AnnPlayer::jump()
{
    if(!getBody()) return;
    if(contactWithGround)
        getBody()->applyCentralImpulse(playerBody->jumpForce);
}


void AnnPlayer::engineUpdate()
{
    bool standing = true;
    if(getBody())
    {
        Ogre::Vector3 translate(getTranslation());
        btVector3 currentVelocity = getBody()->getLinearVelocity();

        //Prevent the rigid body to be put asleep by the physics engine
        getBody()->activate();

        //if the player can stand (= not dead or something like that)

        //if no  user input, be just pull toward the ground by gravity (not physicly realist, but usefull)
        if(translate.isZeroLength())
        {
            getBody()->setLinearVelocity(btVector3(
                        0,
                        currentVelocity.y(),
                        0
                        ));
        }
        else
        {
            Ogre::Vector3 velocity(getOrientation()*translate);
            if(run) velocity *= playerBody->runFactor;
            getBody()->setLinearVelocity(btVector3(
                        velocity.x,
                        currentVelocity.y(),
                        velocity.z
                        ));
        }

        if(standing) //if physic
        {
            btTransform Transform = getBody()->getCenterOfMassTransform();
            Transform.setRotation(btQuaternion(0,0,0,1));
            getBody()->setCenterOfMassTransform(Transform);
        }

        setPosition(
                Ogre::Vector3( 
                    getBody()->getCenterOfMassPosition().x(),
                    getBody()->getCenterOfMassPosition().y() + getEyesHeight()/2,
                    getBody()->getCenterOfMassPosition().z()
                    ));

    }
}
