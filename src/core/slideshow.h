#include "artillery.h"

class SlideShow{
	public:
		static void Init();
		static void Render();
	
	private:
		static void GetImageFiles(string path);
		static void HandleController(int mouseX, int mouseY, int btn);
		static void HandleDestroy();
		static void RenderFullScreen();
		static void LoadNextPicture(AFile* file);
		static void GetImageFiles();
		
	private:
		static Image* FullscreenBuffer;
		static FileList* _files;
		static Config* _cfg;
		static LCDScreen* _lcd;
};
