#include "filesystem.h"

FileSystem::FileSystem()
{
	this->root = new Folder("root", this->root);
	this->currentFolder = this->root;
	this->freeBlock = 0;
}

FileSystem::~FileSystem()
{
	deleteFolder(this->root);
}

void FileSystem::addFolder(std::string name)
{
	this->currentFolder->addFolder(name, this->currentFolder);
}

void FileSystem::addFile(std::string name, std::string data)
{
	this->currentFolder->addFile(name, this->freeBlock++, data);
}

/*Remove a folder from the FS and return it(unmount)*/
FileSystem::Folder* FileSystem::unmountFolder(std::string fileName)
{
	Folder* result = nullptr;
	// Search for right folder
	Folder* child = nullptr;
	int location = -1;
	for (int i = 0; i < this->currentFolder->children.size() && location == -1; i++) {
		child = dynamic_cast<Folder*>(this->currentFolder->children[i]);
		if (fileName == child->name) {
			location = i;
		}
	}

	//uemount from FS
	if (location != -1)
	{
		result = dynamic_cast<Folder*>(this->currentFolder->children[location]);
		this->currentFolder->children.erase(this->currentFolder->children.begin() + location);
	}

	//Returns a nullptr if not found
	return result;
}

/*Delete a folder and its children(inclusive the files in it)*/
void FileSystem::deleteFolder(Folder * folder)
{
	if (folder->children.size() != 0)
	{
		Folder* child = nullptr;
		for (int i = 0; i < folder->children.size(); i++) {
			child = dynamic_cast<Folder*>(folder->children[i]);
			if (child != nullptr) // is a folder
			{
				deleteFolder(child);
			}
			delete folder->children[i];
			folder->children.erase(folder->children.begin() + i); // Vet inte om denna avallokerar minne
		}
	}
	if (folder != this->root)
	{
		// Search for the position of the folder in is parent list of children
		Folder* child = nullptr;
		int location = -1;
		for (int i = 0; i < this->currentFolder->children.size() && location == -1; i++) {
			child = dynamic_cast<Folder*>(this->currentFolder->children[i]);
			if (child == folder) {
				location = i;
			}
		}
		// Delete the folder
		this->currentFolder = dynamic_cast<Folder*>(this->currentFolder->parent);
		delete this->currentFolder->children[location];
		this->currentFolder->children.erase(this->currentFolder->children.begin() + location); // Vet inte om denna avallokerar minne

	}
}

/*Remove a file from the current folder and return that file*/
FileSystem::File* FileSystem::removeFile(std::string name)
{
	File* file = nullptr;
	// Search for right file
	int location = -1;
	for (int i = 0; i < this->currentFolder->children.size() && location == -1; i++) {
		file = dynamic_cast<File*>(this->currentFolder->children[i]);
		if (name == file->name) {
			location = i;
		}
	}
	if (location == -1)
	{
		file = nullptr;
	}
	else
	{
		// Remove file from its parents list of children
		this->currentFolder->children.erase(this->currentFolder->children.begin() + location);
	}

	return file;
}

/*Delete a file in the current folder*/
void FileSystem::deleteFile(File * file)
{
	std::string name = file->name;
	// Search for right file
	int location = -1;
	for (int i = 0; i < this->currentFolder->children.size() && location == -1; i++) {
		file = dynamic_cast<File*>(this->currentFolder->children[i]);
		if (name == file->name) {
			location = i;
		}
	}
	this->currentFolder->children.erase(this->currentFolder->children.begin() + location); // Vet inte om denna avallokerar minne
	delete file;
}

int FileSystem::changeFolder(std::string path)
{
	int result = 1;
	if (path == "..") {
		this->currentFolder = dynamic_cast<Folder*>(this->currentFolder->parent);
	}
	else if (path == ".") {

	}
	else if (path == "") {

	}
	return result;
}



/* Please insert your code */