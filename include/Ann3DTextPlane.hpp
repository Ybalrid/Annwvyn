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
		///Text alignment flag
		enum TextAlign{ALIGN_LEFT = 'l', ALIGN_CENTER = 'c', ALIGN_RIGHT = 'r' };

		///Construct a 3D text plane. Need to provide a caption to auto render text
		/// \param w Width in metter
		/// \param h Height in metter
		/// \param caption Caption 
		/// \param size Character size in typographic point
		/// \param resolution Character "print" resolution in DPI. Thiw will influence the texture resolution
		/// \param font Your name of the font. To reuse a font configuration
		/// \param fontTTF Name of the TTF file known by the resource manager
		Ann3DTextPlane(float w, float h, std::string caption = "",int size = 128, float resolution = 96.0f, std::string font = "defaultFont", std::string fontTTF = "VeraMono.ttf");
		

		~Ann3DTextPlane();
		
		///Set or change the caption
		void setCaption(std::string newCaption);

		///The plane will auto-rerender at changes if set to true
		void setAutoUpdate(bool state);

		///Change the color
		void setTextColor(AnnColor color);

		///Change the background color (note: alpha is either one or nothing. Fragments are discarded, not blended)
		void setBackgroundColor(AnnColor color);

		///Call text re-render (if needed)
		void update();

		///Set the position of the plane
		void setPosition(AnnVect3 p);
		
		///Set the orient of the plane
		void setOrientation(AnnQuaternion q);

		///Set the text alignment mode
		void setTextAlign(TextAlign talign);

		///Get the position of the plane
		AnnVect3 getPosition();

		///Get the orientation
		AnnQuaternion getOrientaiton();

		void setMargin(float margin);

	private:
		///Render the text
		void renderText();
	
		///Fill the texture with transparent black
		void clearTexture();

		///Calculate the actuall vertex coordinates for the plane geometry
		void calculateVerticesForPlaneSize();
		
		///Check if autoupdate is on. If so, call update
		void autoUpdateCheck();
		
		///Generat a random string of leter of arbitrary lenght
		std::string generateRandomString(size_t len = 15);
		
		///Generate a random material name
		void generateMaterialName();

		Ogre::ManualObject* renderPlane;
		Ogre::SceneNode* node;
		Ogre::TexturePtr texture;
		std::string fontName, fontTTF;
		std::string caption;
		
		bool needUpdating;
		
		const size_t materialNameLen = 30;

		void createMaterial();
		std::string materialName;

		AnnVect3 vertices[4];
		const AnnVect2 textureCoords[4] = {{0,0},{0,1},{1,0},{1,1}};
		float width, height, resolutionFactor, xOffset, yOffset;

		Ogre::FontPtr font;
		AnnColor textColor, bgColor;
		TextAlign align;

		bool autoUpdate;

		int fontSize;


		const float dpi2dpm = 0.0254f;

		float dpi;

		unsigned int pixelMargin;
		float margin;

		float DoNotInitializeMe;
	};
}