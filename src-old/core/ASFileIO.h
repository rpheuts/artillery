#ifndef FILEIO_H_
#define FILEIO_H_

#define MAXDIRNUM 1024
#define MAXPATH   256

class ASFile{
	public:
		ASFile(string filename, string path);
		~ASFile();
		
	public:
		std::string Filename;
		std::string Path;
		std::string FullPath;
		int GetFileSize();
};

class ASFileListItem{
	public:
		ASFileListItem(ASFile* file);
		~ASFileListItem();
		
	public:
		ASFile* FilePnt;
		ASFileListItem* Next;
		ASFileListItem* Previous;
};

class ASFileList{
	public:
		ASFileList();
		~ASFileList();
		void Add(ASFile* file);
		void Remove(ASFile* file);
		void ReadFilesToFileList(string path);
		void ReadFilesToFileList(string path,string extension);
		void ReadDirsToFileList(string path);
		void ReadToFileList(string path);
		void Reset();
		void Clear();
		ASFile* GetNextFile();
		ASFile* GetPreviousFile();
		ASFile* GetCurrentFile();
		ASFile* GetFile(int filenumber); 
		int FileCount();
		
	private:
		void RegisterFileListItem(ASFileListItem* item);
		void Read(string path, bool dirs, bool extensionMask, string extension);
		void ReadDirs(string path);
		
	private:
		ASFileListItem* _start;
		ASFileListItem* _end;
		ASFileListItem* _current;
		int _fileCount;
};

#endif 
