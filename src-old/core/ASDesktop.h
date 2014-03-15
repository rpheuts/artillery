class ASDesktop{
	public:
		ASDesktop();
		~ASDesktop();
		void Render();
		void Handle();
		void SwitchWallpaper();
		
	private:
		void LoadWallpaper(ASFile* file);
		void CreateDesktop();
		
	private:
		ASImage* _wallpaper;
		ASImage* _desktopData;
		ASFileList* _files;
};

