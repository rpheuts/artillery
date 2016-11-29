#ifndef FILEIO_H_
#define FILEIO_H_

#define MAXDIRNUM 1024
#define MAXPATH   256

class AFile{
	public:
		AFile(const char * filename, const char * path);
		~AFile();
		
	public:
		std::string Filename;
		std::string Path;
		std::string FullPath;
		int GetFileSize();
};

class FileListItem{
	public:
		FileListItem(AFile* file);
		~FileListItem();
		
	public:
		AFile* FilePnt;
		FileListItem* Next;
		FileListItem* Previous;
};

class FileList{
	public:
		FileList();
		~FileList();
		void Add(AFile* file);
		void Remove(AFile* file);
		void ReadFilesToFileList(string path);
		void ReadFilesToFileList(string path,string extension);
		void ReadDirsToFileList(string path);
		void ReadToFileList(string path);
		void Reset();
		void Clear();
		AFile* GetNextFile();
		AFile* GetPreviousFile();
		AFile* GetCurrentFile();
		AFile* GetFile(int filenumber); 
		int FileCount();
		
	private:
		void RegisterFileListItem(FileListItem* item);
		void Read(string path, bool dirs, bool extensionMask, string extension);
		void ReadDirs(string path);
		
	private:
		FileListItem* _start;
		FileListItem* _end;
		FileListItem* _current;
		int _fileCount;
};

#endif 
