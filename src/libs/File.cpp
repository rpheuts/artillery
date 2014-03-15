#include "File.h"
#include <unistd.h>
#include <stdio.h>
#include <algorithm>


File::File(string name, string path) : Name(name), Path(path)
{
	InitNames();
}

File::File(string fullname)
{
	unsigned int nameStart = fullname.rfind("/");
	if (nameStart != string::npos)
	{
		Name = fullname.substr(nameStart + 1, fullname.length() - nameStart);
		Path = fullname.substr(0, fullname.length() - Name.length());
		InitNames();
	}
}

File::~File()
{
	
}

void File::InitNames()
{
	_fileHnd = 0;
	if (Path[Path.length() - 1] != '/')
		Path += '/';
	FullName = Path + Name;
	
	unsigned int extensionStart = Name.rfind(".");
	if (extensionStart != string::npos)
		Extension = Name.substr(extensionStart + 1, 3);
	
	transform(Extension.begin(), Extension.end(), Extension.begin(), (int(*)(int)) toupper);
	
	// Set the file size
	Size = GetSize();
}

bool File::Exists()
{
	if(FullName.c_str() != 0 && (_fileHnd = fopen(FullName.c_str(), "r")))
	{
		fclose(_fileHnd);
		return true;
	}
	return false;
}

int File::Create()
{
	if (!Exists())
	{
		if (!(_fileHnd = fopen(FullName.c_str(), "w")))
			return -1;
		fclose(_fileHnd);
		_fileHnd = 0;
	}
	return 0;
}

FILE* File::Open(string parameters)
{
	if (_fileHnd != 0)
		return _fileHnd;
	else if (Exists())
	{
		if (!(_fileHnd = fopen(FullName.c_str(), parameters.c_str())))
			return 0;
		return _fileHnd;
	}
	return 0;
}

int File::Close()
{
	if (_fileHnd != 0)
	{
		int ret = fclose(_fileHnd);
		_fileHnd = 0;
		if (!ret);
			return 0;
	}
	else
		return 0;
	
	return -1;
}

int File::Delete()
{
	if(remove(FullName.c_str()) != 0 )
		return -1;
	return 0;
}

long File::GetSize()
{
	FILE* fd;
	if ((fd = Open("r")) != 0)
	{
		fseek(fd, 0, SEEK_END);
		long size = ftell(fd);
		Close();
		return size;
	}
	return -1;
}
