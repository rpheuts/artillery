#ifndef READER_H_
#define READER_H_

#include "artillery.h"

class Reader{
	public:
		static void Init();
		
	private:
		static void HandleDestroy();
		static void HandleController(int mouseX, int mouseY, int btn);
		static void RenderFullScreen();
		static void RenderText();
		static void Render();
	
	private:
		static FileList* _files;
		static Window* _wndHnd; 
		static std::string _text;
		static int _fontSize;
		static int _windowWidth;
		static int _windowHeight;
};
#endif
