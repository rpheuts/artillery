#include "artillery.h"
#include <stdio.h>

FileList* Reader::_files;
Window* Reader::_wndHnd;
std::string Reader::_text;
int Reader::_windowHeight;
int Reader::_windowWidth;
int Reader::_fontSize;

void Reader::Init()
{
	_windowHeight = 200;
	_windowWidth = 300;
	_files = new FileList();
	_files->ReadFilesToFileList("ms0:/MbShell/", "txt");
	_files->ReadFilesToFileList("ms0:/MbShell/", "TXT");
	_wndHnd = WindowManager::CreateWindow(_windowWidth, _windowHeight, HandleController, Render, HandleDestroy, RenderFullScreen, "Reader", 0);
	Reader::_fontSize = 3;
	
	if(_files->FileCount() == 0)
		return;
		
	FILE* pFile;
	if(!(pFile = fopen(_files->GetCurrentFile()->FullPath.c_str(), "r"))) {
			Console::Print("Reader: Error...");
        return;
 	}
	
	fseek(pFile, 0, SEEK_END);
	int size=ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	
 	char * buffer = (char*) malloc (size);
 	fread (buffer,1,size,pFile);
 	
 	fclose(pFile);
	_text = buffer;
 	
 	RenderText();
}

void Reader::RenderText()
{
	int carretX = 1;
	int carretY = 5;
	int count = 0;
	int fontWidth = Font::GetFontWidth(Reader::_fontSize);
	int textLength = _text.length()-2;
	
	Graphics::ClearImage(0xFFEEEEEE, _wndHnd->Screen);
	
	while (count < textLength)
	{
		if (_text[count] == '\n' || (carretX + fontWidth) > _windowWidth)
		{
			if ((carretY + 6) > _windowHeight)
				break;
			carretY += 7;
			carretX = 1;
			if (_text[count] == '\n')
				count++;
			continue;
		}
		
		char ch = _text[count];
		if (ch == ' ')
			carretX += fontWidth;
		else
			carretX += Font::PrintImage(ch, carretX, carretY, _wndHnd->Screen, 0xFF000000, 3) + 1;
		count++;
	}
}

void Reader::HandleDestroy()
{
	//delete(_files);
	_text = "";
}

void Reader::HandleController(int mouseX, int mouseY, int btn)
{

}

void Reader::Render()
{

}

void Reader::RenderFullScreen()
{

}


