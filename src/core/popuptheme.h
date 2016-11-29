#ifndef PTHEME_H_
#define PTHEME_H_	

class PopupTheme
{
	public:
		static void Init();
		static void Render();
		static void Hide();
		static bool Visible();
		static void SpawnThemePopup();
		static void HandleThemePopup(int mouseX, int mouseY, int btn);
		static void RenderThemePopup();
		static void DestroyThemePopup();
		static void TogglePopup();
};
#endif
