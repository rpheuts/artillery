#ifndef MP3PLAYER_H_
#define MP3PLAYER_H_

#include "artillery.h"

class MP3Player{
	public:
		static void Init();
		static void Destroy();
		static void Play();
		static void Render();
		static void Pause();
		static void Stop();
		static void Resume();
		static void Next();
		static void Previous();
		
	private:
		static void setupPlayer();
		static void HandleController(int analogX, int analogY, int btn);
		static void HandleDestroy();
		static void RenderPlayer();
		static void HandleControllerPLL(int analogX, int analogY, int btn);
		static void HandleDestroyPLL();
		static void RenderPLL();
	
	private:
		static FileList* _files;
};
#endif
