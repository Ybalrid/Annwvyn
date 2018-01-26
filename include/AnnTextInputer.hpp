#pragma once

#include <systemMacro.h>
#include <OIS.h>

namespace Annwvyn
{
	///This class permit to get text input from the keyboard
	class AnnDllExport AnnTextInputer : public OIS::KeyListener
	{
	public:
		///Object for text input
		AnnTextInputer();
		///Callback key press method
		bool keyPressed(const OIS::KeyEvent& arg) override;
		///Callback key released method
		bool keyReleased(const OIS::KeyEvent& arg) override;
		///Return the "input" string object
		std::string getInput() const;
		///Permit you to change the content of the input method
		void setInput(const std::string& content);
		///Clear the input string : remove all characters hanging there
		void clearInput();
		///Clear input THEN record typed text
		void startListening();
		///Stop recording typed text
		void stopListening();
		///Set the cursor offset by hand
		void setCursorOffset(int newPos);
		///Get the current position of the internal cursor
		int getCursorOffset() const;

	private:
		///String that holds typed text. Characters are push/popped at the back of this string
		std::string input;
		///If set false, this class does nothing.
		bool listen;
		///true if this text should be ascii only
		bool asciiOnly;
		///Offset from the end of the string where the operations has to be done
		int cursorOffset;
	};
}
