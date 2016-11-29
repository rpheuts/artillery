#include <malloc.h>
#include <string.h>
#include <pspctrl.h>

#include "artillery.h"

static Window* ListStart;
static Window* ListEnd;
static int createdLastX;
static int createdLastY;
static int IsBeingDragged;
static int IsActive;
static Image* shadow_h;
static Image* shadow_v_r;
static Image* shadow_v_l;
static Image* shadow_c_r;
static Image* shadow_c_l;
static Image* wnd_btn_close;
static Image* wnd_img_left;
static Image* wnd_img_right;
static Image* wnd_img_center;
static Image* wnd_img_scale_left;
static Image* wnd_img_scale_right;
int WindowManager::FullScreenEnabled;
int titlebartextoffsetx = 0;
int titlebartextoffsety = 0;

void WindowManager::Init()
{
	createdLastX = 30;
	createdLastY = 30;
	shadow_h = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("c_wndborderhor").c_str());
	shadow_v_r = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("c_wndborderverright").c_str());
	shadow_v_l = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("c_wndborderverleft").c_str());
	shadow_c_r = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("c_wndbordercorright").c_str());
	shadow_c_l = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("c_wndbordercorleft").c_str());
	wnd_btn_close = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("c_wndtopbarbtn").c_str());
	wnd_img_left = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("c_wndtopbarleft").c_str());
	wnd_img_right = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("c_wndtopbarright").c_str());
	wnd_img_center = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("c_wndtopbarcenter").c_str());
	wnd_img_scale_left = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("c_wndtopbarscaleleft").c_str());
	wnd_img_scale_right = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("c_wndtopbarscaleright").c_str());
	Core::CfgTheme->GetIntValue("titlebartextoffsetx", titlebartextoffsetx);
	Core::CfgTheme->GetIntValue("titlebartextoffsety", titlebartextoffsety);
}

void WindowManager::Destroy()
{
	Graphics::FreeImage(shadow_h);
	Graphics::FreeImage(shadow_v_r);
	Graphics::FreeImage(shadow_v_l);
	Graphics::FreeImage(shadow_c_r);
	Graphics::FreeImage(shadow_c_l);
	Graphics::FreeImage(wnd_btn_close);
	Graphics::FreeImage(wnd_img_left);
	Graphics::FreeImage(wnd_img_right);
	Graphics::FreeImage(wnd_img_center);
	Graphics::FreeImage(wnd_img_scale_left);
	Graphics::FreeImage(wnd_img_scale_right);
	
	Window* t = ListEnd;
	while (t)
	{
		DestroyWindow(t);
		t = t->Previous;
	}
}

void WindowManager::DestroyWindow(Window* window)
{
	window->Destroy();
	if (window->Screen)
		Graphics::FreeImage(window->Screen);
	Graphics::FreeImage(window->Topbar);
	
	Window* t = ListStart;
	Window* cur = NULL;
	Window* prev= NULL;

	if (window == ListStart)
	{
		ListStart = t->Next;
		if (t->Next)
			t->Next->Previous = NULL;
		if (window == ListEnd)
			ListEnd = NULL;
		free(t);
		return;
	}
	
	while (t && t->Next)
	{
		if (t->Next == window)
		{
			cur = t->Next;
			prev = t;
			break;
		}
		t = t->Next;
	}
	if (cur == ListEnd)
			ListEnd = prev;
	prev->Next = cur->Next;
	cur->Next->Previous = prev;
	free(cur);
	free(window);
}

void WindowManager::RegisterWindow(Window* window)
{
	if (!ListStart)
	{
		ListStart = window;
		ListStart->Next = 0;
		ListStart->Previous = 0;
		ListEnd = ListStart;
	}
	else
	{
		// Window gets focus, so add to the beginning of the window list
		window->Next =  ListStart;
		window->Next->Previous = window;
		window->Previous = NULL;
		ListStart = window;
	}
}

Image* WindowManager::InitTopbar(int width, char * text)
{
	int index = 0;
	Image* topbar =  Graphics::CreateImage(width, wnd_img_left->imageHeight);
	
	// Blit left part first
	Graphics::BlitImageToImage(0, 0, wnd_img_left->imageWidth, wnd_img_left->imageHeight, wnd_img_left, 0, 0, topbar);
	index += wnd_img_left->imageWidth;
	
	// Blit scaling part for text
	Image* tmp = Graphics::ResizeImage((strlen(text)*6)+titlebartextoffsetx, wnd_img_scale_left->imageHeight, wnd_img_scale_left);
	Graphics::BlitImageToImage(0, 0, tmp->imageWidth, tmp->imageHeight, tmp, index, 0, topbar);
	index += tmp->imageWidth;
	Graphics::FreeImage(tmp);
	
	// Blit center part
	Graphics::BlitImageToImage(0, 0, wnd_img_center->imageWidth, wnd_img_center->imageHeight, wnd_img_center, index, 0, topbar);
	index += wnd_img_center->imageWidth;
	
	// Blit scaling right part
	tmp = Graphics::ResizeImage(width - index - wnd_img_right->imageWidth, wnd_img_scale_right->imageHeight, wnd_img_scale_right);
	Graphics::BlitImageToImage(0, 0, tmp->imageWidth, tmp->imageHeight, tmp, index, 0, topbar);
	index += tmp->imageWidth;
	Graphics::FreeImage(tmp);
	
	// Blit last part
	Graphics::BlitImageToImage(0, 0, wnd_img_right->imageWidth, wnd_img_right->imageHeight, wnd_img_right, index, 0, topbar);
	
	// Blit close button
	Graphics::BlitAlphaImageToImage(0, 0, wnd_btn_close->imageWidth, wnd_btn_close->imageHeight, wnd_btn_close, width - wnd_btn_close->imageWidth - 3, 0, topbar);
	
	// Print text
	Font::PrintImage(text, titlebartextoffsetx, titlebartextoffsety, topbar, 0xFFFFFFFF, 3);
	return topbar;
}

Window* WindowManager::CreateWindow(int width, int height, void (*handlectrlr)(int mouseX, int mouseY, int btn), void (*render)(void), void (*handleDstry)(void), void (*handleFullscrn)(void), char * name, int fullscreen)
{
	Window* w = CreateWindow(width, height, handlectrlr, render, handleDstry, name, 0);
	w->Fullscreen = fullscreen;
	w->RenderFullscreen = handleFullscrn;
	return w;
}

Window* WindowManager::CreateWindow(int width, int height, void (*handlectrlr)(int mouseX, int mouseY, int btn), void (*render)(void), void (*handleDstry)(void), char * name, int transparent, int borders)
{
	Window* w = CreateWindow(width, height, handlectrlr, render, handleDstry, name, transparent);
	w->Borders = borders;
	return w;
}

Window* WindowManager::CreateWindow(int width, int height, void (*handlectrlr)(int mouseX, int mouseY, int btn), void (*render)(void), void (*handleDstry)(void), char * name, int transparent)
{
	Window* w = new Window();
	w->Screen = Graphics::CreateImage(width, height);
	
	
	w->Topbar = InitTopbar(width, name);
	w->Width = width;
	w->Height = height + w->Topbar->imageHeight;
	w->X = createdLastX;
	w->Y = createdLastY;
	w->ContentX = w->X;
	w->ContentY = w->Y + w->Topbar->imageHeight;
	w->Handle = handlectrlr;
	w->Destroy = handleDstry;
	w->Transparent = transparent;
	w->Fullscreen = 0;
	w->Borders = 1;
	w->Enabled = 1;
	w->Render = render;

	RegisterWindow(w);
	createdLastX += 20;
	createdLastY += 10;
	return w;
}

void WindowManager::Focus(Window* window)
{
	Window* t = ListStart;
	
	if (window != t)
	{
		Window* cur = NULL;
		Window* prev= NULL;
		
		while (t && t->Next)
		{
			if (t->Next == window)
			{
				cur = t->Next;
				prev = t;
				break;
			}
			t = t->Next;
		}
		// if we are at the end of the list and there is a previous node set that node to be the end 
		if (cur == ListEnd && prev)
			ListEnd = prev;
		// set the next node of our previous node to our next
		prev->Next = cur->Next;
		// set the previous of our next node to our previous if there was any
		if (cur->Next)
			cur->Next->Previous = prev;
		// set our next node the to the original start of the list
		cur->Next = ListStart;
		// we are the first now, so we don't have a previous
		cur->Previous = NULL;
		// swap the previous node of our next node to our previous so we get rendered ;)
		cur->Next->Previous = cur;
		// set us to the static startnode
		ListStart = cur;
		// pfew.. done
	}
}

void WindowManager::WindowPosition(Window* window, int x, int y)
{
	if (!window->Borders)
		y -= window->Topbar->imageHeight;
	window->X = x;
	window->Y = y;
	window->ContentX = x;
	window->ContentY = y + window->Topbar->imageHeight;
}

void WindowManager::HandleFocus()
{
	if (Controller::LastInput == PSP_CTRL_CROSS && ListStart && !IsBeingDragged)
	{
		Window* t = ListStart;
		while (t)
		{
			if (t->Enabled)
			{
				if (Interface::IsWithinWindow(Controller::AnalogX, Controller::AnalogY, t))
				{
					if (ListStart != t)
					{
						Focus(t);
						IsActive = 1;
						Controller::Handled = 1;
					}
					break;
				}
				else if (Controller::AnalogX < t->X || Controller::AnalogY < t->Y || Controller::AnalogX > (t->X + t->Width) || Controller::AnalogY > (t->Y + t->Height))
					IsActive = 0;
			}
			t = t->Next;
		}
	}
}

void WindowManager::HandleDrag()
{
	if (Controller::LastInput == PSP_CTRL_CROSS && ListStart && Controller::AnalogX > ListStart->X + ListStart->Width - (wnd_btn_close->imageWidth + 3) && Controller::AnalogY > ListStart->Y && Controller::AnalogX < (ListStart->X + ListStart->Width) && Controller::AnalogY < (ListStart->Y + ListStart->Topbar->imageHeight))
	{
				DestroyWindow(ListStart);
				Controller::Handled = 1;
	}
	if (Controller::LastInput == PSP_CTRL_CROSS && ListStart)
	{
		if (Interface::IsWithinImage(Controller::AnalogX, Controller::AnalogY, ListStart->X, ListStart->Y, ListStart->Topbar))
		{
			WindowPosition(ListStart, ListStart->X + (Controller::AnalogX - Controller::PreviousAnalogX), ListStart->Y + (Controller::AnalogY - Controller::PreviousAnalogY));
			IsBeingDragged = 1;
			Controller::Handled = 1;
			IsActive = 1;
		}
		else if (Controller::IsLastInputPressed && Controller::LastInput == PSP_CTRL_CROSS && ListStart && IsBeingDragged)
		{
			WindowPosition(ListStart, ListStart->X + (Controller::AnalogX - Controller::PreviousAnalogX), ListStart->Y + (Controller::AnalogY - Controller::PreviousAnalogY));
			Controller::Handled = 1;
			IsActive = 1;
		}
	}
	else if (IsBeingDragged)
		IsBeingDragged = 0;
}

void WindowManager::HandleController()
{
	if (ListStart)
	{
		if ((Interface::IsWithinWindowContent(Controller::AnalogX, Controller::AnalogY, ListStart) || IsActive) && ListStart->Enabled == 1)
		{
			if (!ListStart->Fullscreen)
				ListStart->Handle(Controller::AnalogX - ListStart->ContentX, Controller::AnalogY - ListStart->ContentY, Controller::LastInput);
			else
				ListStart->Handle(Controller::AnalogX, Controller::AnalogY, Controller::LastInput);
			Controller::Handled = 1;
			IsActive = 1;
		}
		else
			IsActive = 0;
	}
}

void WindowManager::Handle()
{
	if (!Controller::Handled)
	{
		HandleFocus();
		if (!Controller::Handled)
			HandleDrag();
		if (!Controller::Handled)
			HandleController();
	}
}

void WindowManager::Render()
{
	FullScreenEnabled = 0;
	if (ListEnd)
	{
		int width, height, offsetx, offsety, posx, posy;
		Window* t = ListEnd;
		while (t)
		{
			if (t->Enabled == 1)
			{
				t->Render();
				if (t->Fullscreen)
				{
					t->RenderFullscreen();
					t = t->Previous;
					FullScreenEnabled = 1;
					continue;
				}
				if (!(t->ContentY >= 272) && !(t->ContentX >= 480))
				{
				width = 0; height = 0; offsetx = 0; offsety = 0, posx = t->ContentX, posy = t->ContentY;
				// render window content
				// calculate size to render
				if (t->Screen->imageWidth + t->ContentX > 480)
					width = t->Screen->imageWidth - ((t->Screen->imageWidth + t->ContentX) - 480);
				if (t->X < 0)
				{
					width = t->Screen->imageWidth + t->ContentX;
					offsetx = -t->X;
					posx = 0;
				}
				if (t->Screen->imageHeight + t->ContentY > 272)
					height = t->Screen->imageHeight - ((t->Screen->imageHeight + t->ContentY) - 272);
				if (t->ContentY < 0)
				{
					height = t->Screen->imageHeight + t->ContentY;
					offsety = -t->ContentY;
					posy = 0;
				}
				// render the window content
				if (t->Transparent)
					Graphics::BlitAlphaImageToScreen(0, 0, t->Screen->imageWidth, t->Screen->imageHeight, t->Screen, t->ContentX, t->ContentY);
				else {
				if (width && height)
					Graphics::BlitImageToScreen(offsetx, offsety, width, height, t->Screen, posx, posy);
				if (width && !height)
					Graphics::BlitImageToScreen(offsetx, offsety, width, t->Screen->imageHeight, t->Screen, posx, posy);
				if (!width && height)
					Graphics::BlitImageToScreen(offsetx, offsety, t->Screen->imageWidth, height, t->Screen, posx, posy);
				if (!width && !height)
					Graphics::BlitImageToScreen(offsetx, offsety, t->Screen->imageWidth, t->Screen->imageHeight, t->Screen, posx, posy); }
				
				if (t->Borders)
				{
					// render topbar
					Graphics::BlitAlphaImageToScreen(0, 0, t->Topbar->imageWidth, t->Topbar->imageHeight, t->Topbar, t->X, t->Y);
					// render shadows
					Graphics::BlitAlphaImageToScreen(0,0 , shadow_v_r->imageWidth, t->Height, shadow_v_r, t->ContentX + t->Width, t->Y);
					Graphics::BlitAlphaImageToScreen(0,0 , shadow_v_l->imageWidth, t->Height, shadow_v_l, t->ContentX - shadow_v_l->imageWidth, t->Y);
					Graphics::BlitAlphaImageToScreen(0,0 , t->Width, shadow_h->imageHeight, shadow_h, t->ContentX, t->Y + t->Height);
					Graphics::BlitAlphaImageToScreen(0,0 , shadow_c_r->imageWidth, shadow_c_r->imageHeight, shadow_c_r, t->ContentX + t->Width, t->Y + t->Height);
					Graphics::BlitAlphaImageToScreen(0,0 , shadow_c_l->imageWidth, shadow_c_l->imageHeight, shadow_c_l, t->ContentX - shadow_c_l->imageWidth, t->Y + t->Height);
					}
				}
			}
			t = t->Previous;
		}
	}
}
