#include "artillery.h"

static Window* popupHnd;
FileList* themeList;
Image* popupHeader;
Image* popupStretch;
int toggle = 0;
int pinit = 0;
int visible = 0;

void PopupTheme::Init()
{
	themeList = new FileList();
	themeList->ReadDirsToFileList(Core::CfgTheme->ThemeDir);
	popupHeader = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("c_popupheader").c_str());
	popupStretch = Graphics::ResizeImage(popupHeader->imageWidth, themeList->FileCount() * 7, Graphics::LoadPNG(Core::CfgTheme->GetPathValue("c_popupstretch").c_str()));
	popupHnd = WindowManager::CreateWindow(110, (7*themeList->FileCount())+popupHeader->imageHeight, HandleThemePopup, RenderThemePopup, DestroyThemePopup, "Reader", 0);
	popupHnd->Borders = 0;
	WindowManager::WindowPosition(popupHnd, SCREEN_WIDTH-popupHnd->Width, SCREEN_HEIGHT-Taskbar::GetHeight()-popupHeader->imageHeight-popupStretch->imageHeight);
	Graphics::BlitImageToImage(0, 0, popupHeader->imageWidth, popupHeader->imageHeight, popupHeader, 0, 0, popupHnd->Screen);
	Graphics::BlitImageToImage(0, 0, popupStretch->imageWidth, popupStretch->imageHeight, popupStretch, 0, popupHeader->imageHeight, popupHnd->Screen);
	
	for (int i=0; i<themeList->FileCount(); i++)
		Font::PrintImage(themeList->GetFile(i)->Filename.c_str(), 1, i*7+popupHeader->imageHeight, popupHnd->Screen, 0x00000000, 3);	
	pinit = 1;
}

void PopupTheme::TogglePopup()
{
	toggle = 1;
}

bool PopupTheme::Visible()
{
	if (visible == 1)
		return true;
	else
		return false;	
}

void PopupTheme::Hide()
{
	if (pinit == 1)
	{
		popupHnd->Enabled = 0;
		visible = 0;
	}
}

void PopupTheme::Render()
{
	if (toggle == 1)
		SpawnThemePopup();
}

void PopupTheme::SpawnThemePopup()
{
	if (pinit == 0)
		Init();
	popupHnd->Enabled = 1;
	visible = 1;
	toggle = 0;
}

void PopupTheme::HandleThemePopup(int mouseX, int mouseY, int btn)
{
	if (Controller::LastInput == PSP_CTRL_CROSS  && !Controller::IsLastInputPressed)
	{
		if (mouseY > 0 && mouseY <  popupHeader->imageHeight)
			Desktop::SwitchWallpaper();
		if (mouseY > popupHeader->imageHeight)
		{
			int offset = popupHeader->imageHeight;
			int id = ((mouseY - offset) / 7);
			if (id < themeList->FileCount())
			{
				Core::Cfg->SetValue("theme", themeList->GetFile(id)->Filename);
				Core::Cfg->CloseConfig();
				ModuleManager::Reboot();
			}
		}
	}
}

void PopupTheme::RenderThemePopup()
{
	
}

void PopupTheme::DestroyThemePopup()
{
	
}
