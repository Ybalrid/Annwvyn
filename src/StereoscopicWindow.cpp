#include "StereoscopicWindow.hpp"

using namespace CEGUI;

StereoscopicWindow::StereoscopicWindow():
    centerOffset(0),
    knownOffset(false),
    left(NULL),
    right(NULL),
    posX(0), posY(0),
    sizeX(0), sizeY(0)
{
    //nothing realy interesting to do here in fact... just construct an empty thing with a lot of 0 and NULL
}

void StereoscopicWindow::createWindow(const char* type, const char* name)
{
    CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
    
    left = wmgr.createWindow(type,name);
    right = wmgr.createWindow(type, name);
}

void StereoscopicWindow::setText(const char* text)
{
    if(left)
        left->setText(text);
    if(right)
        right->setText(text);
}

void StereoscopicWindow::setSize(float x, float y)
{
    if(left)
        left->setSize(CEGUI::USize(CEGUI::UDim(x,0), CEGUI::UDim(y,0)));
    if(right)
        right->setSize(CEGUI::USize(CEGUI::UDim(x,0), CEGUI::UDim(y,0)));

    sizeX = x; sizeY = y;

}


float StereoscopicWindow::calculatePosX(float posX, float sizeX, float offset, bool mirror)
{
    if(mirror)
        return  (posX/2) - (sizeX/2) - (offset/2)+0.5f;
    return (posX/2) - (sizeX/2) + (offset/2);
}

void StereoscopicWindow::setPosition(float x, float y)
{
    if(!knownOffset) return;
    if(left)
        left->setPosition(CEGUI::UVector2(CEGUI::UDim(calculatePosX(x,sizeX,centerOffset),0),CEGUI::UDim(y,0)));
    if(right)
        right->setPosition(CEGUI::UVector2(CEGUI::UDim(calculatePosX(x,sizeX,centerOffset,true),0),CEGUI::UDim(y,0)));
    posX = x; posY = y;
}

void StereoscopicWindow::addChildToSheet(CEGUI::Window* sheet)
{
    if(left)
        sheet->addChild(left); 
    if(right)
        sheet->addChild(right);
}

void StereoscopicWindow::setCenterOffset(float offset)
{
    centerOffset = offset;
    knownOffset = true;
}
