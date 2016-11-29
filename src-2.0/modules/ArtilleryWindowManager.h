#ifndef ARTILLERYWINDOWMANAGER_H_
#define ARTILLERYWINDOWMANAGER_H_

#include "Includes.h"

/** This class handles the creation, destroying and managing of windows.
	Some analog input is processed here to support window dragging
	and focussing.
 */
class ArtilleryWindowManager : public WindowManager
{
	public:
		static ArtilleryWindowManager* Instance();
		void Tick();
		int RegisterArtilleryWindow(WindowApplication* window);
		void DestroyArtilleryWindow(WindowApplication* window);
		void ShowPreviousArtilleryWindow();
		void ShowNextArtilleryWindow();
		void ShowArtilleryWindow(WindowApplication* window);
		
	private:
		ArtilleryWindowManager();
		~ArtilleryWindowManager();
		void RenderMainWindow();
		void HandleMainWindow();
		
	private:
		static ArtilleryWindowManager* _artilleryInstance;
		list<WindowApplication*> _mainWindows;
		WindowApplication* _currentMain;
		WindowApplication* _previousMain;
		Image* _currentFade;
		Image* _savedScreenState;
		int _currentFadeAmount;
};
#endif
