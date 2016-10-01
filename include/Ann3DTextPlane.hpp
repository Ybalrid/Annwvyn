#pragma once

#include "systemMacro.h"

#include <Ogre.h>
#include <Overlay\OgreFontManager.h>
#include <Overlay\OgreFont.h>
#include <string>

#include "AnnEngine.hpp"

#include "AnnAbstractMovable.hpp"

namespace Annwvyn
{
	class DLL Ann3DTextPlane : public AnnAbstractMovable
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
		
		///Class destructor
		~Ann3DTextPlane();
		
		///Set or change the caption
		/// \param newCaption the new text to display
		void setCaption(std::string newCaption);

		///The plane will auto-rerender at changes if set to true
		/// \param state If true, the text will be updated each time you change something
		void setAutoUpdate(bool state);

		///Change the color
		/// \param color The color of the text
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
		AnnQuaternion getOrientation();

		///Set margin
		/// \param margin Margin in metters
		void setMargin(float margin);

		///Set background image
		/// \param imgName name of an image loaded in the resource manager
		void setBackgroundImage(std::string imgName);

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
		
		///Create the material
		void createMaterial();

		///The actual 3D text plane
		Ogre::ManualObject* renderPlane;

		///Node where the object is attached
		Ogre::SceneNode* node;

		///Textures 
		Ogre::TexturePtr texture, bgTexture;

		///Font configuration
		std::string fontName, fontTTF;

		///Text to display
		std::string caption;
		
		///If true, next update call will update
		bool needUpdating;
		
		///Lenght of the random material name
		const size_t materialNameLen = 30;

		///Name of the materail
		std::string materialName;

		///Vertex buffer
		AnnVect3 vertices[4];

		///Static texture coordinates
		const AnnVect2 textureCoords[4] = {{0,0},{0,1},{1,0},{1,1}};

		///Various floats for size computation
		float width, height, resolutionFactor, xOffset, yOffset;

		///The font from the Ogre::FontManager
		Ogre::FontPtr font;

		///Colors 
		AnnColor textColor, bgColor;

		///Text alignment
		TextAlign align;

		///if true, will update each time something is updated
		bool autoUpdate;

		///Size of the font, in points 
		int fontSize;

		///Static convertions factor between Dot Per Inch and Dot Per Metters
		const float dpi2dpm = 0.0254f;

		///DPI resolution of the rendered fonts
		float dpi;

		///Margin in pixels
		unsigned int pixelMargin;

		///Margin in metters
		float margin;
		
		///Will use an image as background
		bool useImageAsBackground;
	};
}