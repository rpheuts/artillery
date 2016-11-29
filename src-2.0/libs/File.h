#ifndef FILE_H
#define FILE_H

#include <string>
using namespace std;

class File
{
	public:
		File(string name, string path);
		File(string fullname);
		~File();
		bool Exists();
		int Create();
		FILE* Open(string parameters);
		int Close();
		int Delete();
		
	protected:
		void InitNames();
		long GetSize();
		
	public:
		string Name;
		string Path;
		string FullName;
		string Extension;
		long Size;
		
	protected:
		FILE* _fileHnd;
};

#endif
