#include "ASIncludes.h"

ASWindow::ASWindow()
{

}

void ASWindow::HandleController(int mouseX, int mouseY, int btn)
{
	if (Application != NULL)
		Application->HandleController(mouseX, mouseY, btn);
}

void ASWindow::Destroy()
{
	if (Application != NULL)
		Application->Destroy();
}

void ASWindow::Render()
{
	if (Application != NULL)
		Application->Render();
}

void ASWindow::RenderFullscreen()
{
	if (Application != NULL)
		Application->RenderFullscreen();
}
