class Desktop{
	public:
		static void Init();
		static void Destroy();
		static void Render();
		static void Handle();
		static void SwitchWallpaper();
		
	private:
		static void LoadWallpaper(AFile* file);
		static void CreateDesktop();
		
	private:
		static FileList* _files;
		static int _init;
};

