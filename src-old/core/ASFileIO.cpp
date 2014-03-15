#include <pspiofilemgr.h>
#include <stdio.h>
#include <pspkernel.h>
#include <dirent.h> 
#include "ASIncludes.h"

//----------------------------------------------------------------
// Class: ASFile

ASFile::ASFile(string filename, string path)
{
	Filename = filename;
	Path = path;
	if(Path[ASFile::Path.size()-1] == '/')
		FullPath = ASFile::Path + ASFile::Filename;
	else
		FullPath = ASFile::Path + "/" + ASFile::Filename;
}

ASFile::~ASFile()
{
	Path.~string(); 
	FullPath.~string(); 
	Filename.~string(); 
}

int ASFile::GetFileSize()
{
	int fd;
	if(!(fd = sceIoOpen(ASFile::FullPath.c_str(), PSP_O_RDONLY, 0777)))
   	return -1;
 	
 	int pos = sceIoLseek(fd, 0, SEEK_END);
 	sceIoClose(fd);
 	return pos;
}

//----------------------------------------------------------------
// Class: FileListItem

ASFileListItem::ASFileListItem(ASFile* file)
{
	FilePnt = file;
	Next = NULL;
	Previous = NULL;
}

ASFileListItem::~ASFileListItem()
{
	delete(FilePnt);
}
//----------------------------------------------------------------
// Class: FileList

ASFileList::ASFileList()
{
	_start = NULL;
	_end = NULL;
	_fileCount = 0;
}

ASFileList::~ASFileList()
{
	ASFileListItem* t = _start;
	while (t && t->Next)
	{
		if (t->Previous != NULL)
			delete(t->Previous);
		t = t->Next;
	}
	if (t != NULL)
		delete(t);
}

void ASFileList::Add(ASFile* file)
{
	ASFileListItem* item = new ASFileListItem(file);
	RegisterFileListItem(item);
}

void ASFileList::Remove(ASFile* file)
{
	ASFileListItem* t = _start;
	ASFileListItem* cur = NULL;
	ASFileListItem* prev= NULL;

	if (file == _start->FilePnt)
	{
		_start = t->Next;
		if (t->Next)
			t->Next->Previous = NULL;
		if (file == _end->FilePnt)
			_end = NULL;
		delete(t);
		return;
	}
	
	while (t && t->Next)
	{
		if (t->Next->FilePnt == file)
		{
			cur = t->Next;
			prev = t;
			break;
		}
		t = t->Next;
	}
	if (cur == _end)
			_end = prev;
	prev->Next = cur->Next;
	cur->Next->Previous = prev;
	delete(cur);
}

void ASFileList::ReadToFileList(string path)
{
	Read(path, true, false, NULL);
}

void ASFileList::ReadFilesToFileList(string path)
{
	Read(path, false, false, NULL);
}

void ASFileList::ReadFilesToFileList(string path, string extension)
{
	Read(path, false, true, extension);
}

void ASFileList::ReadDirsToFileList(string path)
{
	ReadDirs(path);
}

void ASFileList::Read(string path, bool dirs, bool extensionMask, string extension)
{
	int count = 0;
	DIR* dirp;
   struct dirent* en;
   string filename;
   
   dirp = opendir(path.c_str()); 
   while(1) 
   {
		en = readdir(dirp);
      if (!en) {
      	break;
      }
		if (en->d_name[0] == '.') 
			continue;
    	// Add to file list
    	if (dirs || !FIO_SO_ISDIR(en->d_stat.st_attr))
    	{
    		filename = en->d_name;
    		if (!extensionMask || filename.find(extension) != string::npos)
    		{
    			ASFile* f = new ASFile(en->d_name, path);
    			Add(f);
    			count++;
    		}
    	}
	}
	_fileCount += count;
}

void ASFileList::ReadDirs(string path)
{
	int count = 0;
	DIR* dirp;
   	struct dirent* en;
   
	dirp = opendir(path.c_str()); 
	while(1) 
	{
		en = readdir(dirp);
		if (!en) { break; } 
		
		if (en->d_name[0] == '.') 
			continue;
		// Add to file list
		if ((en->d_stat.st_attr & 0x10) == 0x10)
		{
				ASFile* f = new ASFile(en->d_name, path);
				Add(f);
				count++;
		}
	}
	_fileCount += count;
}


void ASFileList::Reset()
{
	_current = _start;
}

void ASFileList::Clear()
{
	ASFileListItem* t =  _start;
	ASFileListItem* rem = NULL;
	while (t)
	{
		delete (t->FilePnt);
		rem = t;
		t = t->Next;
	}
}

void ASFileList::RegisterFileListItem(ASFileListItem* item)
{
	if (!_start)
	{
		_start = item;
		_start->Next = 0;
		_start->Previous = 0;
		_end = item;
		_current = item;
	}
	else
	{
		// Add to end of list
		item->Next =  NULL;
		item->Previous = _end;
		_end->Next = item;
		_end = item;
	}
}

ASFile* ASFileList::GetNextFile()
{
	
	if (_current)
	{
		ASFile* ret = _current->FilePnt;
		if (_current->Next)
			_current = _current->Next;
		else if (_fileCount > 1)
			_current = NULL;
		return ret;
	}
	else
		return NULL; 
}

ASFile* ASFileList::GetPreviousFile()
{
	if (_current)
	{
		ASFile* ret = _current->FilePnt;
		if (_current->Previous)
			_current = _current->Previous;
		else if (_fileCount > 1)
			_current = NULL;
		return ret;
	}
	else
		return NULL;
}

ASFile* ASFileList::GetCurrentFile()
{
	if (_current)
		return _current->FilePnt;
	else
		return NULL;
}

ASFile* ASFileList::GetFile(int filenumber)
{
	if (filenumber == 0)
		return _start->FilePnt;
	
	int count = 0;
	ASFileListItem* f = _start;
	while(f)
	{
		if (count == filenumber)
			return f->FilePnt;
		count++;
		f = f->Next;
	}
	return NULL;
}

int ASFileList::FileCount()
{
	return _fileCount;
}
