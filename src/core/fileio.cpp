#include <pspiofilemgr.h>
#include <stdio.h>
#include <pspkernel.h>
#include <dirent.h> 
#include "artillery.h"

//----------------------------------------------------------------
// Class: AFile

AFile::AFile(const char * filename, const char * path)
{
	AFile::Filename = filename;
	AFile::Path = path;
	if(AFile::Path[AFile::Path.size()-1] == '/')
		AFile::FullPath = AFile::Path + AFile::Filename;
	else
		AFile::FullPath = AFile::Path + "/" + AFile::Filename;
}

AFile::~AFile()
{
	Path.~string(); 
	FullPath.~string(); 
	Filename.~string(); 
}

int AFile::GetFileSize()
{
	int fd;
	if(!(fd = sceIoOpen(AFile::FullPath.c_str(), PSP_O_RDONLY, 0777)))
   	return -1;
 	
 	int pos = sceIoLseek(fd, 0, SEEK_END);
 	sceIoClose(fd);
 	return pos;
}

//----------------------------------------------------------------
// Class: FileListItem

FileListItem::FileListItem(AFile* file)
{
	FileListItem::FilePnt = file;
	FileListItem::Next = NULL;
	FileListItem::Previous = NULL;
}

FileListItem::~FileListItem()
{
	delete(FileListItem::FilePnt);
}
//----------------------------------------------------------------
// Class: FileList

FileList::FileList()
{
	FileList::_start = NULL;
	FileList::_end = NULL;
	FileList::_fileCount = 0;
}

FileList::~FileList()
{
	FileListItem* t = FileList::_start;
	while (t && t->Next)
	{
		if (t->Previous != NULL)
			delete(t->Previous);
		t = t->Next;
	}
	if (t != NULL)
		delete(t);
}

void FileList::Add(AFile* file)
{
	FileListItem* item = new FileListItem(file);
	RegisterFileListItem(item);
}

void FileList::Remove(AFile* file)
{
	FileListItem* t = FileList::_start;
	FileListItem* cur = NULL;
	FileListItem* prev= NULL;

	if (file == FileList::_start->FilePnt)
	{
		FileList::_start = t->Next;
		if (t->Next)
			t->Next->Previous = NULL;
		if (file == FileList::_end->FilePnt)
			FileList::_end = NULL;
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
	if (cur == FileList::_end)
			FileList::_end = prev;
	prev->Next = cur->Next;
	cur->Next->Previous = prev;
	delete(cur);
}

void FileList::ReadToFileList(string path)
{
	Read(path, true, false, NULL);
}

void FileList::ReadFilesToFileList(string path)
{
	Read(path, false, false, NULL);
}

void FileList::ReadFilesToFileList(string path, string extension)
{
	Read(path, false, true, extension);
}

void FileList::ReadDirsToFileList(string path)
{
	ReadDirs(path);
}

void FileList::Read(string path, bool dirs, bool extensionMask, string extension)
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
    			AFile* f = new AFile(en->d_name, path.c_str());
    			Add(f);
    			count++;
    		}
    	}
	}
	FileList::_fileCount += count;
}

void FileList::ReadDirs(string path)
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
    				AFile* f = new AFile(en->d_name, path.c_str());
    				Add(f);
    				count++;
    		}
	}
	FileList::_fileCount += count;
}


void FileList::Reset()
{
	FileList::_current = FileList::_start;
}

void FileList::Clear()
{
	FileListItem* t =  FileList::_start;
	FileListItem* rem = NULL;
	while (t)
	{
		delete (t->FilePnt);
		rem = t;
		t = t->Next;
		//delete(rem);
	}
}

void FileList::RegisterFileListItem(FileListItem* item)
{
	if (!FileList::_start)
	{
		FileList::_start = item;
		FileList::_start->Next = 0;
		FileList::_start->Previous = 0;
		FileList::_end = item;
		FileList::_current = item;
	}
	else
	{
		// Add to end of list
		item->Next =  NULL;
		item->Previous = FileList::_end;
		FileList::_end->Next = item;
		FileList::_end = item;
	}
}

AFile* FileList::GetNextFile()
{
	
	if (FileList::_current)
	{
		AFile* ret = FileList::_current->FilePnt;
		if (FileList::_current->Next)
			FileList::_current = FileList::_current->Next;
		else if (FileList::_fileCount > 1)
			FileList::_current = NULL;
		return ret;
	}
	else
		return NULL; 
}

AFile* FileList::GetPreviousFile()
{
	if (FileList::_current)
	{
		AFile* ret = FileList::_current->FilePnt;
		if (FileList::_current->Previous)
			FileList::_current = FileList::_current->Previous;
		else if (FileList::_fileCount > 1)
			FileList::_current = NULL;
		return ret;
	}
	else
		return NULL;
}

AFile* FileList::GetCurrentFile()
{
	if (FileList::_current)
		return FileList::_current->FilePnt;
	else
		return NULL;
}

AFile* FileList::GetFile(int filenumber)
{
	if (filenumber == 0)
		return FileList::_start->FilePnt;
	
	int count = 0;
	FileListItem* f = FileList::_start;
	while(f)
	{
		if (count == filenumber)
			return f->FilePnt;
		count++;
		f = f->Next;
	}
	return NULL;
}

int FileList::FileCount()
{
	return FileList::_fileCount;
}
