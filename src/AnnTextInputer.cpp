// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "AnnTextInputer.hpp"
#include "AnnGetter.hpp"
#include <string>

using namespace Annwvyn;

AnnTextInputer::AnnTextInputer() :
 listen(false),
 asciiOnly { true },
 cursorOffset { 0 }
{
}

bool AnnTextInputer::keyPressed(const OIS::KeyEvent& arg)
{
	if(!listen) return true;

	//Handle backspace
	if(arg.key == OIS::KC_BACK && !input.empty())
	{
		if(cursorOffset > input.size()) cursorOffset = int(input.size());
		input.erase(end(input) - std::min(int(input.size()), 1 + cursorOffset));
	}

	//Text
	else if((arg.text < 0x7F && arg.text > 0x1F) || !asciiOnly)
	{
		//Put typed char into the application
		input.insert(std::max(0, int(input.size()) - int(cursorOffset)), 1, char(arg.text));
	}

	//Return key
	else if(arg.text == '\r')
	{
		input.push_back('\r');
	}

	//Arrow Keys
	else if(arg.key == OIS::KC_UP || arg.key == OIS::KC_DOWN || arg.key == OIS::KC_LEFT || arg.key == OIS::KC_RIGHT)
	{
		AnnGetOnScreenConsole()->notifyNavigationKey(KeyCode::code(arg.key));
	}
	return true;
}

bool AnnTextInputer::keyReleased(const OIS::KeyEvent& arg)
{
	return true;
}

std::string AnnTextInputer::getInput() const
{
	return input;
}

void AnnTextInputer::clearInput()
{
	input.clear();
	cursorOffset = 0;
}

void AnnTextInputer::startListening()
{
	clearInput();
	listen = true;
}

void AnnTextInputer::stopListening()
{
	listen = false;
}

void AnnTextInputer::setInput(const std::string& content)
{
	input		 = content;
	cursorOffset = 0;
}

void AnnTextInputer::setCursorOffset(int newPos)
{
	if(newPos >= 0)
		cursorOffset = newPos;
}

int AnnTextInputer::getCursorOffset() const
{
	return cursorOffset;
}
