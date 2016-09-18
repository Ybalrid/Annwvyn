#pragma once

#include "systemMacro.h"

#include <Ogre.h>
#include <Overlay\OgreFontManager.h>
#include <Overlay\OgreFont.h>

#include <string>

#include "AnnEngine.hpp"

#include "AnnTypes.h"

namespace Annwvyn
{
	class DLL Ann3DTextPlane
	{
	public:
		Ann3DTextPlane(float w, float h, float resolution, std::string caption = "", std::string font = "defaultFont");
		void setCaption(std::string newCaption);
		void setFontName(std::string font);
	private:
		void renderText();
		void clearTexture();



		void calculateVerticesForPlaneSize();

		Ogre::ManualObject* renderPlane;
		Ogre::SceneNode* node;
		Ogre::TexturePtr texture;
		std::string caption, fontName;
		
		bool needUpdating;
		
		void generateMaterialName();

		const size_t materialNameLen = 30;
		std::string generateRandomString(size_t len = 15);


		void createMaterial();
		std::string materialName;

		AnnVect3 vertices[4];
		const AnnVect2 textureCoords[4] = {{0,0},{0,1},{1,0},{1,1}};
		float width, height, resolutionFactor, xOffset, yOffset;

		Ogre::FontPtr font;
	};
}