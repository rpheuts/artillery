#ifndef HBMANAGER_H_
#define HBMANAGER_H_

#include "artillery.h"

class Eboot{
	public:
		string ElfPath;
		string Name;
		Image* Icon;
};

class HomebrewItem{
	public:
		Eboot* EbootContainer;
		int X;
		int Y;
		int Selected;
		HomebrewItem* Next;
};

class HomeBrewManager{
	public:
		static void Init();
		static void Destroy();
		
	public:
		static HomebrewItem* EbootList;
		static PanelContainer* Container;
		static int IsInit;
		static int HomeBrewCount;
	
	private:
		static void ReadDirs();
		static AFile* FindMatchingDir(string dir, FileList* list);
		static bool SaveIcon(int fd, string dir, int size);
		static Eboot* LoadEboot(FileList* list, AFile* dir);
		static void AddToList(Eboot* eboot);
		static Image* LoadPNGFromEboot(string path);
		
	private:
		static FileList* _dirs;
		static HomebrewItem* _listStart;
};

class HomebrewPanel: public Panel{
	public:
		void Handle(int mouseX, int mouseY, int btn);
		int Render();
	
	public:
		int ScrollYPosition;
};
#endif
