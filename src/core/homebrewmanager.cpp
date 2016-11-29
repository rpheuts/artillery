#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <pspdebug.h>
#include <pspkernel.h>
#include <pspiofilemgr.h>
#include <pspctrl.h>

#include "artillery.h"

#define ICON_WIDTH 80
#define ICON_HEIGHT 50

// Every PBP has a header containing the offsetts to different parts of the PBP
typedef struct { char signature[4]; int version; int offset[8]; } HEADER;

// we need:
// - first part (PSF / SFO) containing the name etc
// - second part (ICON) containing a nice icon

FileList* HomeBrewManager::_dirs;
HomebrewItem* HomeBrewManager::_listStart;
HomebrewItem* HomeBrewManager::EbootList;
PanelContainer* HomeBrewManager::Container;
int HomeBrewManager::IsInit;
int HomeBrewManager::HomeBrewCount;

void HomeBrewManager::ReadDirs()
{
	_dirs = new FileList();
	_dirs->ReadDirsToFileList(Core::Cfg->GetValue("homebrewpath").c_str());
}

bool HomeBrewManager::SaveIcon(int fd, string dir, int size)
{
	int wd;
	string tmp = dir + "/Icon0.png";
	
	if(!(wd = sceIoOpen(tmp.c_str(), PSP_O_WRONLY|PSP_O_CREAT, 0777)))
        return false;
 	
 	void* data = malloc(size);
 	sceIoRead(fd, data, size);
 	sceIoWrite(wd, data, size);
 	sceIoClose(wd);
 	return true;
}

AFile* HomeBrewManager::FindMatchingDir(string dir, FileList* list)
{
	int count = 0;
	while(count < list->FileCount())
	{
		AFile* file = list->GetFile(count);
		if (file != NULL && file->Filename.find(dir) != string::npos && file->Filename.find("%") != string::npos)
			return file;
		else if (file != NULL && file->Filename.find("~1%") != string::npos)
		{
			unsigned int pos = file->Filename.find("~1%");
			if (dir.find(file->Filename.substr(0, pos)) != string::npos)
				return file;
		}
		count++;
	}
	return NULL;
}

Image* HomeBrewManager::LoadPNGFromEboot(string path)
{
	int size,fd;
	Image* ret = NULL;
	Console::Print(path);
	if ((fd = sceIoOpen(path.c_str(), PSP_O_RDONLY, 0777)) > 0) 
	{
		HEADER header;
		sceIoRead(fd, &header, sizeof(HEADER));
						
		if (header.signature[0] != 0x00) { printf("ERROR: Input file is not a PBP file.\n"); return NULL;} else
		if (header.signature[1] != 0x50) { printf("ERROR: Input file is not a PBP file.\n"); return NULL;} else
		if(header.signature[2] != 0x42) { printf("ERROR: Input file is not a PBP file.\n"); return NULL;} else
		if (header.signature[3] != 0x50) { printf("ERROR: Input file is not a PBP file.\n"); return NULL;}
		size = header.offset[2] - header.offset[1];
		sceIoLseek(fd, header.offset[1], PSP_SEEK_SET);
		
		ret = Graphics::ResizeImage(ICON_WIDTH, ICON_HEIGHT, Graphics::LoadPNG(fd));
		
		sceIoClose(fd);
	}
	else
		ret = Graphics::CreateImage(ICON_WIDTH, ICON_HEIGHT);
	return ret;
}

Eboot* HomeBrewManager::LoadEboot(FileList* list, AFile* dir)
{
	int fd;
	unsigned int pos = dir->Filename.find("   ");
	string filename;
	if(pos != string::npos)
		filename = dir->Filename.substr(0, pos);
	else
		filename = dir->Filename;
		
	AFile* pbp = FindMatchingDir(filename, list);
	if (pbp == NULL)
		return NULL;
	
	Eboot* eboot = new Eboot();
	eboot->ElfPath = dir->FullPath + "/EBOOT.PBP";
	string buf = pbp->FullPath + "/EBOOT.PBP";

    if ((fd = sceIoOpen(buf.c_str(), PSP_O_RDONLY, 0777)) > 0) 
    {
			//  opened file, so get size now
			int size = sceIoLseek(fd, 0, PSP_SEEK_END);
			sceIoLseek(fd, 0, PSP_SEEK_SET);
						
			// ------------------------------------------------------
			// Read Header
			HEADER header;
			sceIoRead(fd, &header, sizeof(HEADER));
						
			if (header.signature[0] != 0x00) { printf("ERROR: Input file is not a PBP file.\n"); return NULL;} else
  			if (header.signature[1] != 0x50) { printf("ERROR: Input file is not a PBP file.\n"); return NULL;} else
  			if(header.signature[2] != 0x42) { printf("ERROR: Input file is not a PBP file.\n"); return NULL;} else
  			if (header.signature[3] != 0x50) { printf("ERROR: Input file is not a PBP file.\n"); return NULL;}
			size = header.offset[1] - header.offset[0];
						
			// ------------------------------------------------------
			// Read SFO
			char* sfo = (char*) malloc(size);
			if (sfo != 0) 
			{
				// Read Section0
	   		sceIoRead(fd, sfo, size);
	   					 
	   		// Extract the name
	   		eboot->Name = (char*) malloc(sizeof(char[128]));
	   		int i = 128, count = 0;
	   		char c = '1';
	   		while (i <size && c != 0x00)
	   		{
	   			c = sfo[i];
	   			eboot->Name[i++ - 128] = c;
	   			count++;
	   		}
			} 
			else 
			{
	    		printf("Error allocing\n");
	    		sceIoClose(fd);
	    		return NULL;
	    	}
	    	free(sfo);
	    				
	    	// ------------------------------------------------------
			// Read Icon
			size = header.offset[2] - header.offset[1];
			if (size > 0)
				eboot->Icon = Graphics::ResizeImage(ICON_WIDTH, ICON_HEIGHT, Graphics::LoadPNG(fd));
	   	else
	   	{
	   		// use something else
	   		 eboot->Icon = Graphics::CreateImage(ICON_WIDTH, ICON_HEIGHT);
	   		 Graphics::ClearImage(0xFFFF0000, eboot->Icon);
	   	}
	   	
	   	sceIoClose(fd);		
	}
	else
	{
		return NULL;
	}
	HomeBrewCount++;
	return eboot;
}

void HomeBrewManager::AddToList(Eboot* eboot)
{
	if (eboot == NULL)
		return;
	
	HomebrewItem* item = new HomebrewItem();
	item->EbootContainer = eboot;
	if (_listStart == NULL)
	{
		_listStart = item;
		_listStart->Next = 0;
	}
	else
	{
		// Add to the beginning of the list, most easy :]
		item->Next =  _listStart;
		_listStart = item;
	}
}

void HomeBrewManager::Init()
{
	if (!IsInit)
	{
		
		if (Core::Cfg->CheckValue("autodetecthomebrew"))
		{
			ReadDirs();
			if (_dirs->FileCount() == 0)
			{
				LCD::PrintMessage("No homebrew found");
				return;
			}
			
			int count = 0;
			HomeBrewCount = 0;
			AFile* f = _dirs->GetFile(count);
			while (f && count < _dirs->FileCount())
			{
				if (f->Filename.find("%") == string::npos && f->Filename.find("Artillery") == string::npos)
					AddToList(LoadEboot(_dirs, f));
				f = _dirs->GetFile(++count);
			}
			EbootList = _listStart;
		}
		else
		{
			list<string> hblist;
			ConfigHomebrew* cfgh = new ConfigHomebrew(Core::CfgTheme->IconConfig);
			cfgh->FillIconList(&hblist);
			HomeBrewCount = 0;
			while (hblist.size() > 0)
			{
				string path = cfgh->GetPathValue(hblist.front());
				if (path.substr(0,1) != "_" && (Core::Cfg->CheckValue("panelshowdesktopicons")) || !cfgh->CheckDesktopValue(hblist.front()))
				{
					Eboot* eboot = new Eboot();
					eboot->ElfPath = path;
					if (cfgh->GetIconValue(hblist.front()) == "_eboot")
						eboot->Icon = LoadPNGFromEboot(cfgh->GetPathPerValue(hblist.front()));
					else
						eboot->Icon = Graphics::LoadPNG(cfgh->GetIconValue(hblist.front()).c_str());
					eboot->Name = hblist.front();
					AddToList(eboot);
					HomeBrewCount++;
				}
				hblist.pop_front();	
			}
			EbootList = _listStart;
		}
		//Setup panel
		Container = PanelManager::AddContainer(Right, Core::Cfg->GetIntValue("panelwidth"), Graphics::LoadPNG(Core::CfgTheme->GetPathValue("c_sidebar").c_str()));
		HomebrewPanel* panel = new HomebrewPanel();
		Container->AddPanel(panel);
		panel->ScrollYPosition = 0;
		IsInit = 1;
	}
	else if (!Container->Visible)
		Container->Show();
}

void HomeBrewManager::Destroy()
{
	HomebrewItem* item = HomeBrewManager::EbootList;
	while (item != NULL)
	{
		Graphics::FreeImage(item->EbootContainer->Icon);
		delete(item->EbootContainer);
		item = item->Next;
	}
	delete(HomeBrewManager::EbootList);
	delete(Container);
	delete(_dirs);
	IsInit = 0;
}

int HomebrewPanel::Render()
{
	if (_screenUpdate == 1)
	{
		int y = 5-ScrollYPosition;
		HomebrewItem* item = HomeBrewManager::EbootList;
		
		Graphics::ClearImage(0x000000, Panel::Display);
		while (item != NULL)
		{
			if (y + item->EbootContainer->Icon->imageHeight > 0)
			{
				Graphics::BlitImageToImage(0, 0, item->EbootContainer->Icon->imageWidth, item->EbootContainer->Icon->imageHeight, item->EbootContainer->Icon, 3, y, Panel::Display);
				item->Y = y;
				y+= item->EbootContainer->Icon->imageHeight + 5;
				if (item->EbootContainer->Name.length() > 19)
					Font::PrintImage(item->EbootContainer->Name.substr(0,16).c_str(), 3, y, Panel::Display, 0xFF000000, 3);
				else
					Font::PrintImage(item->EbootContainer->Name.c_str(), 3, y, Panel::Display, 0xFF000000, 3);
			}
			else
				y+= item->EbootContainer->Icon->imageHeight + 5;
			y+= 20;
			item = item->Next;
		}
		_screenUpdate = 0;
		return 1;
	}
	return 0;
}

void HomebrewPanel::Handle(int mouseX, int mouseY, int btn)
{
	HomebrewItem* item = HomeBrewManager::EbootList;
	while (item != NULL)
	{
		if (btn == PSP_CTRL_CROSS && mouseY > item->Y && mouseY < item->Y + item->EbootContainer->Icon->imageHeight + 15)
			ModuleManager::LoadStartELF(item->EbootContainer->ElfPath.c_str());
		item = item->Next;
	}
	if (btn == PSP_CTRL_RTRIGGER && !Controller::IsLastInputPressed)
	{
		if(HomeBrewManager::Container->Visible)
			HomeBrewManager::Container->Hide();
		else
			HomeBrewManager::Container->Show();
		Controller::Handled = 1;
	}
	if (btn == PSP_CTRL_DOWN)
	{
		if ((HomeBrewManager::HomeBrewCount * ICON_HEIGHT) + (HomeBrewManager::HomeBrewCount * 10) > ScrollYPosition)
			ScrollYPosition += 5;
		_screenUpdate = 1;
	}
	if (btn == PSP_CTRL_UP)
	{
		if (ScrollYPosition >= 5)
			ScrollYPosition -= 5;
		_screenUpdate = 1;
	}
}
