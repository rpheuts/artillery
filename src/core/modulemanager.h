class ModuleManager{
	public:
		static int LoadStartModule(const char *path, const char* arg);
		static void LoadStartUMD();
		static void LoadStartELF(const char * elf);
		static void LoadStartMPH();
		static void Reboot();
};
