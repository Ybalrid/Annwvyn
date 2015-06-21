#ifndef	ANNCONSOLE
#define ANNCONSOLE

#include "systemMacro.h"
#include "AnnTypes.h"
#include <string>
#include <iostream>
#include <Ogre.h>
#include <Overlay/OgreFont.h>
#include <Overlay/OgreFontManager.h>

#define CONSOLE_BUFFER 19
#define BASE 256
#define MARGIN 4

namespace Annwvyn
{
	///In engine - On screen floating console
	class DLL AnnConsole
	{
	public:
		///Construct the console. This should only be called by AnnEngine itself when the camera and ogre are operational
		AnnConsole();

		///Add text to the console buffer. The console buffer will keep CONSOLE_BUFFER lines of messages in memory only
		/// \pram string text to append to the console
		void append(std::string string);

		///Set arbitrary the visibility state of the console
		void setVisible(bool visibility);

		///Toogle the console. 
		void toogle();

		///True if text has been updated on the console and the console is visible.
		bool needUpdate();

		///Update the console by filling it with background texture then bliting text on it. 
		///Can take some computing time depending on the size/resolution of the textures and buffer
		void update();

	private:
		void WriteToTexture(const Ogre::String& str, Ogre::TexturePtr destTexture, Ogre::Image::Box destRectangle, Ogre::Font* font, const Ogre::ColourValue &color, char justify = 'l',  bool wordwrap = true);
		bool modified;
		//////////////////////////////

		AnnVect3 points[4];
		AnnVect2 textCoord[4];

		std::string buffer[CONSOLE_BUFFER];
		bool visibility;
		Ogre::ManualObject* displaySurface;
		Ogre::TexturePtr displayTexture;
		Ogre::SceneNode* consoleNode;

		Ogre::TexturePtr texture;
		Ogre::FontPtr font;
		Ogre::TexturePtr background; 

		AnnVect3 offset;
	};
}

#endif
