#include <pspkernel.h>
#include <pspiofilemgr.h>
#include <pspctrl.h>
#include <string.h>
#include <malloc.h>
#include <psphprm.h>
#include <stdio.h>
#include <psppower.h>

#include "mp3player.h"
#include "windowmanager.h"
#include "graphics.h"
#include "font.h"

#define MAXDIRNUM 1024
#define MAXPATH   256

int test2 = 0;
static int CurrentFile;
static int FileCount;
static int Playback;
static int Paused;
static int Enabled;
static int IsInit;
static int DriverInit;
static int GraphicalEnabled;
static int pllScroll;
static int plloffsetx;
static int plloffsety;
FileList* MP3Player::_files;
LCDScreen* lcd;

// graphical representation (main window)
static Window* wndHnd;
static Image* img_back_panel;
static Image* img_btn_play;
static Image* img_btn_stop;
static Image* img_btn_pause;
static Image* img_btn_next;
static Image* img_btn_previous;
static Image* img_pll_topbar;
static Image* img_pll_body;

// graphical representation (playlist window)
static Window* pllWndHnd;

void MP3Player::setupPlayer()
{
	CurrentFile = -1;
	FileCount = 0;
	Playback = 0;
	Paused = 0;
	Enabled = 1;
	pllScroll = 0;
	
	Config* cfg = new Config("ms0:/MBShell/config/main.cfg");
	
	_files = new FileList();
	_files->ReadFilesToFileList(cfg->GetValue("mp3path"), "mp3");
	_files->ReadFilesToFileList(cfg->GetValue("mp3path"), "MP3");

	// load and render backpanel
	img_back_panel = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("m_backpanel").c_str());
	Image* tmp = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("m_backpaneltitle").c_str());
	Graphics::BlitAlphaImageToImage(0, 0, tmp->imageWidth, tmp->imageHeight, tmp, 4, 0, img_back_panel);
	Graphics::FreeImage(tmp);

	//load buttons
	img_btn_play = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("m_btnplay").c_str());
	img_btn_stop = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("m_btnstop").c_str());
	img_btn_pause = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("m_btnpause").c_str());
	img_btn_next = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("m_btnforward").c_str());
	img_btn_previous = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("m_btnreverse").c_str());
	
	//load playlist images
	img_pll_topbar = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("m_plltopbar").c_str());
	img_pll_body = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("m_pllbody").c_str());
	
	Core::CfgTheme->GetIntValue("mp3playlistoffsetx", plloffsetx);
	Core::CfgTheme->GetIntValue("mp3playlistoffsety", plloffsety);

	IsInit = 1;
	free (cfg);
	
	lcd = new LCDScreen();
	lcd->SetDefaultText("MP3 Player");
	LCD::AddScreen(lcd);
	
}

void MP3Player::HandleController(int analogX, int analogY, int btn)
{
	if (btn == PSP_CTRL_CROSS)
	{
		if (analogY > (img_back_panel->imageHeight - 30) && !Controller::IsLastInputPressed)
		{
			// software button pressed
			if (analogX < 36)
			{
				Previous();
				lcd->PrintMessage("Previous song");
			}
			if (analogX > 34 && analogX < 67 && Playback)
			{
				lcd->PrintMessage("Stop");
				Stop();
			}
			if (analogX > 65 && analogX < 98 && !Paused && Playback)
			{
				lcd->PrintMessage("Pause");
				Pause();
			}
			if (analogX > 96 && analogX < 129 && !Paused && !Playback)
			{
				Play();
				lcd->PrintMessage("Play");
			}
			if (analogX > 96 && analogX < 129 && Paused)
			{
				lcd->PrintMessage("Play");
				Resume();
			}
			if (analogX > 127 && analogX < 160)
			{
				lcd->PrintMessage("Next song");
				Next();
			}
			if (analogX > 170)
			{
				pllWndHnd->Enabled = !pllWndHnd->Enabled;
			}
		}
	}
	if ((btn == PSP_CTRL_TRIANGLE || Controller::LastInputRemote == PSP_HPRM_PLAYPAUSE) && !Playback)
	{
		Play();
		lcd->PrintMessage("Play");
	}
	if ((btn == PSP_CTRL_TRIANGLE || Controller::LastInputRemote == PSP_HPRM_PLAYPAUSE) && Paused)
	{
		lcd->PrintMessage("Play");
		Resume();
	}
	if ((btn == PSP_CTRL_CIRCLE || Controller::LastInputRemote == PSP_HPRM_PLAYPAUSE) && !Paused && Playback)
	{
		lcd->PrintMessage("Pause");
		Pause();
	}
	if ((btn == PSP_CTRL_RTRIGGER || Controller::LastInputRemote == PSP_HPRM_FORWARD) && !Controller::IsLastInputPressed)
	{
		lcd->PrintMessage("Next song");
		Next();
	}
	if ((btn == PSP_CTRL_LTRIGGER || Controller::LastInputRemote == PSP_HPRM_BACK) && !Controller::IsLastInputPressed)
	{
		Previous();
		lcd->PrintMessage("Previous song");
	}
	if (PlayBackMP3::EndOfFile())
		Next();
}

void MP3Player::HandleControllerPLL(int analogX, int analogY, int btn)
{
	if (btn == PSP_CTRL_CROSS && !Controller::IsLastInputPressed)
	{
		int offset = img_pll_topbar->imageHeight + 1;
		int id = ((analogY - offset) / 7) + pllScroll;
		if (id > -1 && id < _files->FileCount())
		{
			_files->Reset();
			for (int i=0; i<id; i++)
				 _files->GetNextFile();
			Play();
		}
	}
	if (btn == PSP_CTRL_DOWN && !Controller::IsLastInputPressed)
	{
		if (pllScroll < _files->FileCount())
			pllScroll += 1;	
	}
	
	if (btn == PSP_CTRL_UP && !Controller::IsLastInputPressed)
	{
		if (pllScroll > 0)
			pllScroll -= 1;	
	}
}

void MP3Player::RenderPLL()
{
	if (Enabled)
	{
		if (GraphicalEnabled)
		{
			Graphics::BlitImageToImage(0, 0, img_pll_topbar->imageWidth, img_pll_topbar->imageHeight, img_pll_topbar, 0, 0, pllWndHnd->Screen);
			Graphics::BlitImageToImage(0, 0, img_pll_body->imageWidth, img_pll_body->imageHeight, img_pll_body, 0, img_pll_topbar->imageHeight, pllWndHnd->Screen);
			int offset = img_pll_topbar->imageHeight + 1;
			for (int i=0; i<_files->FileCount();i++)
			{
				if (i >= pllScroll && i < pllScroll + 14)
				{
					if (_files->GetCurrentFile() == _files->GetFile(i))
						Font::PrintImage(_files->GetFile(i)->Filename.c_str(), 3, ((i - pllScroll)*7)+offset, pllWndHnd->Screen, 0x99BB0B5E, 3);
					else
						Font::PrintImage(_files->GetFile(i)->Filename.c_str(), 5, ((i - pllScroll)*7)+offset, pllWndHnd->Screen, 0xBBDDBD80, 3);
			
				}
			}
		}
	}
	if (IsInit == 0)
		WindowManager::DestroyWindow(pllWndHnd);
}

void MP3Player::HandleDestroyPLL()
{
	Graphics::FreeImage(img_pll_topbar);
	Graphics::FreeImage(img_pll_body);
}

void MP3Player::Render()
{
	if (Enabled)
		if (GraphicalEnabled)
			RenderPlayer();
}

void MP3Player::HandleDestroy()
{
	if (Playback)
		Stop();
	Enabled = 0;
	GraphicalEnabled = 0;

	Graphics::FreeImage(img_back_panel);
	Graphics::FreeImage(img_btn_play);
	Graphics::FreeImage(img_btn_stop);
	Graphics::FreeImage(img_btn_pause);
	Graphics::FreeImage(img_btn_next);
	Graphics::FreeImage(img_btn_previous);
	IsInit = 0;

	// Temp fix, just hide playlist for now
	pllWndHnd->Enabled = 0;
	Graphics::FreeImage(img_pll_topbar);
	Graphics::FreeImage(img_pll_body);
	
	LCD::RemoveScreen(lcd);
	free(lcd);
	IsInit = 0;
}

void MP3Player::Init()
{	
	if (!Enabled && !GraphicalEnabled)
	{
		if (!IsInit)
			setupPlayer();
		// create window
		wndHnd = WindowManager::CreateWindow(img_back_panel->imageWidth, img_back_panel->imageHeight, HandleController, Render, HandleDestroy, "MP3 Player", 0);
		if (!pllWndHnd)
		{
			pllWndHnd = WindowManager::CreateWindow(img_pll_topbar->imageWidth, img_pll_topbar->imageHeight + img_pll_body->imageHeight, HandleControllerPLL, RenderPLL, HandleDestroyPLL, "Playlist", 0);
			pllWndHnd->Screen = Graphics::CreateImage(img_pll_topbar->imageWidth, img_pll_topbar->imageHeight + img_pll_body->imageHeight);
			pllWndHnd->Borders = 0;
		}
		pllWndHnd->Enabled = 1;
		WindowManager::WindowPosition(pllWndHnd, wndHnd->X-2, wndHnd->Y + wndHnd->Height + 3);
		GraphicalEnabled = 1;
		Enabled = 1;
	}
}

void MP3Player::Destroy()
{
	if(IsInit == 1)
		HandleDestroy();
	HandleDestroyPLL();
}

void MP3Player::RenderPlayer()
{
	Graphics::BlitImageToImage(0, 0, img_back_panel->imageWidth, img_back_panel->imageHeight, img_back_panel, 0, 0, wndHnd->Screen);
	Graphics::BlitAlphaImageToImage(0, 0, img_btn_previous->imageWidth, img_btn_previous->imageHeight, img_btn_previous, 4, wndHnd->Screen->imageHeight - (img_btn_previous->imageHeight + 4), wndHnd->Screen); 
	Graphics::BlitAlphaImageToImage(0, 0, img_btn_stop->imageWidth, img_btn_stop->imageHeight, img_btn_stop, 35, wndHnd->Screen->imageHeight - (img_btn_stop->imageHeight + 4), wndHnd->Screen); 
	Graphics::BlitAlphaImageToImage(0, 0, img_btn_pause->imageWidth, img_btn_pause->imageHeight, img_btn_pause, 66, wndHnd->Screen->imageHeight - (img_btn_pause->imageHeight + 4), wndHnd->Screen); 
	Graphics::BlitAlphaImageToImage(0, 0, img_btn_play->imageWidth, img_btn_play->imageHeight, img_btn_play, 97, wndHnd->Screen->imageHeight - (img_btn_play->imageHeight + 4), wndHnd->Screen); 
	Graphics::BlitAlphaImageToImage(0, 0, img_btn_next->imageWidth, img_btn_next->imageHeight, img_btn_next, 128, wndHnd->Screen->imageHeight - (img_btn_next->imageHeight + 4), wndHnd->Screen);
	
	if(_files->FileCount() > 0)
	{
		if (_files->GetCurrentFile())
			if (_files->GetCurrentFile()->Filename.length() < 16)
				Font::PrintImage(_files->GetCurrentFile()->Filename.c_str(), 65, 10, wndHnd->Screen, 0xBBDDBD80, 1);
			else
				Font::PrintImage(_files->GetCurrentFile()->Filename.substr(0,15).c_str(), 65, 10, wndHnd->Screen, 0xBBDDBD80, 1);
		if (Playback)
		{
			char buf[3];
			sprintf(buf, "%i", PlayBackMP3::GetPercentDone());
			Font::PrintImage("Percent:", 10, 10, wndHnd->Screen, 0x99BB0B5E, 3);
			Font::PrintImage(buf, 24, 17, wndHnd->Screen, 0x99BB0B5E, 1);
		}
	}
	if (pllWndHnd->Enabled == 1)
		WindowManager::WindowPosition(pllWndHnd, wndHnd->X-plloffsetx, wndHnd->Y + wndHnd->Height + plloffsety);
}

void MP3Player::Next()
{
	Stop();
	_files->GetNextFile();
	Play();
}

void MP3Player::Previous()
{
	Stop();
	_files->GetPreviousFile();
	Play();
}

void MP3Player::Play()
{
	if(_files->FileCount() > 0)
	{
		AFile* file = _files->GetCurrentFile();
		if (!file)
		{
			_files->Reset();
			file = _files->GetCurrentFile();
			if (!file)
				return;
		}
		scePowerSetClockFrequency(333, 333, 166);
		if (DriverInit == 0)
			PlayBackMP3::Init(0);
		DriverInit = 1;
		PlayBackMP3::Load(file->FullPath.c_str());
		PlayBackMP3::Play();
		Playback = 1;
		lcd->SetDefaultText(file->Filename);
	}
	else
		lcd->PrintMessage("No Files");
}

void MP3Player::Pause()
{
		PlayBackMP3::Pause();
		Paused = 1;
}

void MP3Player::Resume()
{
		PlayBackMP3::Play();
		Paused = 0;
}

void MP3Player::Stop()
{
	scePowerSetClockFrequency(222, 222, 111);
	PlayBackMP3::Stop();
	lcd->SetDefaultText("MP3 Player");
	DriverInit = 0;
	Playback = 0;
	Paused = 0;
}
