#include "filesystem.h"

FileSystem::FileSystem()
{
	this->root = new Folder("root", this->root);
	this->root->parent = this->root;
	this->currentFolder = this->root;

	for (int i = 0; i < this->mMemblockDevice.size(); i++)
		this->emptyBlocks.push_back(i);
}

FileSystem::~FileSystem()
{
	deleteFolder(this->root);
}

void FileSystem::createFolder(const std::string &dirpath)
{
	Folder* folder = this->currentFolder;
	int i = posOfLastNameInPath(dirpath, -1);
	std::string path = dirpath.substr(0, i);
	std::string name = dirpath.substr(i, dirpath.size() - i);
	this->goToFolder(path);
	this->currentFolder->addFolder(name, this->currentFolder);
	this->currentFolder = folder;
}

void FileSystem::removeFile(std::string filepath)
{
	// Separate the path to the folder and the name of the file
	int i = this->posOfLastNameInPath(filepath, 0);
	std::string path = filepath.substr(0, i);
	std::string name = filepath.substr(i, filepath.size() - i);

	Folder* folder = this->currentFolder;
	this->goToFolder(path);
	File* file = dynamic_cast<File*>(this->currentFolder->children[findFile(name)]);
	this->deleteFile(file);
	this->currentFolder = folder;
}

void FileSystem::removeFolder(std::string path) {
	if (this->currentFolder != this->root)
	{
		this->goToFolder(path);
		Folder* parent = dynamic_cast<Folder*>(this->currentFolder->parent);
		this->deleteFolder(this->currentFolder);
		this->currentFolder = parent;
	}
}

void FileSystem::addFile(std::string name, std::string data)
{
	File* newFile = dynamic_cast<File*>(this->currentFolder->addFile(name));

	int nrOfNeededBlocks = 1;
	std::string tempData = data;
	while (tempData.size() > this->mMemblockDevice[0].size())
	{
		nrOfNeededBlocks++;
		tempData = tempData.substr(0,
			(tempData.size() > this->mMemblockDevice[0].size()? tempData.size() - this->mMemblockDevice[0].size(): tempData.size()));
	}

	for (int i = 0; i < nrOfNeededBlocks; i++)
	{
		if (this->emptyBlocks.size() > 0)
		{
			newFile->blocks.push_back(this->emptyBlocks.back());
			this->emptyBlocks.pop_back();
		}
	}
	int index = 0;
	std::string remainingData = data;
	std::string inputData;
	for(int i = 0; i < newFile->blocks.size(); i++)
	{
		index = newFile->blocks[i];
		inputData = remainingData.substr(0,
			(remainingData.size() > this->mMemblockDevice[0].size() ? this->mMemblockDevice[0].size() : remainingData.size()));

		remainingData = remainingData.substr(0,
			(remainingData.size() > this->mMemblockDevice[0].size()? remainingData.size() - this->mMemblockDevice[0].size(): remainingData.size()));

		this->mMemblockDevice.writeBlock(index, inputData);
	}
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
			} else { // is a file
				this->deleteFile(dynamic_cast<File*>(folder->children[i]), folder);
			}
		}
		for (int i = 0; i < folder->children.size(); i++)
			folder->children.erase(folder->children.begin() + i); // Vet inte om denna avallokerar minne
	}
	if (folder != this->root)
	{
		// Search for the position of the folder in is parent list of children
		int location = this->findFolder(folder->name);

		// Delete the folder
		this->currentFolder = dynamic_cast<Folder*>(this->currentFolder->parent);
		delete this->currentFolder->children[location];
		this->currentFolder->children.erase(this->currentFolder->children.begin() + location); // Vet inte om denna avallokerar minne

	}
}

/*Remove a file from the current folder and return that file*/
FileSystem::File* FileSystem::detachFile(std::string name)
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
void FileSystem::deleteFile(File * file, Folder *parent)
{
	if (file != nullptr)
	{
		if (parent == nullptr) parent = this->currentFolder;
		int location = this->findFile(file->name);
		parent->children.erase(parent->children.begin() + location);
		this->freeFile(file);
		delete file;
	}
}

/* Change folder and return the last name in the path */
std::string FileSystem::goToFolder(std::string path)
{

	bool err = false;
	std::string temp = "";
	if (path != "")
	{
		if (path.at(0) == '/')
		{
			this->currentFolder = this->root;
		}
		while (path.size() > 1 && !err)
		{
			this->parsePath(temp, path);
			if (temp == "..")
			{
				this->currentFolder = dynamic_cast<Folder*>(this->currentFolder->parent);
			}
			else if (temp != ".")
			{
				int location = this->findFolder(temp);
				if (location != -1)
				{
					this->currentFolder = dynamic_cast<Folder*>(this->currentFolder->children[location]);
				}
				else
				{
					err = true;
				}
			}
		}
	}
	else
	{
		temp = "/";
	}
	if (err) temp = "/"; // if temp == "/" the path is invalid
	return temp;
}

/*Create a new file in the FS*/
void FileSystem::createFile(const std::string &filepath, const std::string &data)
{
	Folder* folder = this->currentFolder;
	int i = posOfLastNameInPath(filepath, -1);
	std::string path = filepath.substr(0, i);
	std::string name = filepath.substr(i, filepath.size() - i);
	this->goToFolder(path);
	this->addFile(name, data);
	this->currentFolder = folder;
}

int FileSystem::findFolder(std::string name) const
{
	int location = -1;
	Folder* folder = nullptr;
	for (int i = 0; i < this->currentFolder->children.size() && location == -1; i++) {
		folder = dynamic_cast<Folder*>(this->currentFolder->children[i]);
		if (name == folder->name) {
			location = i;
		}
	}
	return location;
}

int FileSystem::findFile(std::string name) const
{
	int location = -1;
	File* file = nullptr;
	for (int i = 0; i < this->currentFolder->children.size() && location == -1; i++) {
		file = dynamic_cast<File*>(this->currentFolder->children[i]);
		if (name == file->name) {
			location = i;
		}
	}
	return location;
}

void FileSystem::parsePath(std::string &temp, std::string &path)
{
	int i = -1;
	int index = 0;
	if (path.at(0) == '/')
	{
		index = 1;
		while (i == -1)
		{
			if (path.size() == index || path.at(index) == '/')
			{
				i = index;
			}
			index++;
		}
		index = 1;
	}
	else if (path.at(0) == '.' && path.at(1) == '/') // start with './'
	{
		index = 0;
		i = 1;
	}
	else // start with '..'
	{
		index = 0;
		i = 2;
	}
	temp = path.substr(index, i - index);
	path = path.substr(i, path.size() - i);
}

/* Free the blocks that the file uses */
void FileSystem::freeFile(File *file)
{
	for (int i = 0; i < file->blocks.size(); i++)
	{
		this->emptyBlocks.push_back(file->blocks[i]);
	}
	file->blocks.clear();
}

int FileSystem::posOfLastNameInPath(std::string path, int type)
{
	int i = path.size() - 1;
	bool done = false;
	while (!done && i != -1)
	{
		if (path.at(i) == '/')
		{
			done = true;
			i += 2;
		}
		i--;
	}
	if (type == 0)
	{
		if (this->findFile(path.substr(i, path.size() - i)) == -1)
			i = -1;
	}
	else if (type == 1)
	{
		if (this->findFolder(path.substr(i, path.size() - i)) == -1)
			i = -1;
	}

	return i;
}

std::string FileSystem::displayChildren(std::string path) 
{
	std::string result = "";
	if (path == "./")
	{
		for (int i = 0; i < this->currentFolder->children.size(); i++)
		{
			result += " " + this->currentFolder->children[i]->name;
			if (dynamic_cast<File*>(this->currentFolder->children[i]) != nullptr)
			{
				int size = 0;
				for (int j = 0; j < dynamic_cast<File*>(this->currentFolder->children[i])->blocks.size(); j++)
				{
					size += mMemblockDevice[dynamic_cast<File*>(this->currentFolder->children[i])->blocks[j]].size();
				}
				result += "(" + std::to_string(size) + "B)";
			}
		}
	}
	else
	{
		Folder* mem = this->currentFolder;
		this->goToFolder(path);
		result = "Type      Name      Permissions      Size\n";
		std::string type = "DIR";
		File* file = nullptr;
		for (int i = 0; i < this->currentFolder->children.size(); i++)
		{
			file = dynamic_cast<File*>(this->currentFolder->children[i]);
			int size = 0;
			type = "DIR";
			if (file != nullptr)
			{
				type = "FILE";
				for (int j = 0; j < file->blocks.size(); j++)
				{
					size += mMemblockDevice[file->blocks[j]].size();
				}
			}
			result += type + "      " + this->currentFolder->children[i]->name + "      " +
				"?" + "      " + std::to_string(size) + "\n";
		}
		this->currentFolder = mem;
	}
	
	return result;
}

std::string FileSystem::getblockString(std::string path) 
{
	std::string result = "";
	Folder* mem = this->currentFolder;
	// Separate the path to the parent and the nameof the file of the filepath
	int i = posOfLastNameInPath(path, 0);
	std::string pathOfParent = path.substr(0, i);
	std::string name = path.substr(i, path.size() - i);
	this->goToFolder(pathOfParent);
	int location = this->findFile(name);
	for (int i = 0; i < dynamic_cast<File*>(this->currentFolder->children[location])->blocks.size(); i++) 
	{
		result += mMemblockDevice.readBlock(dynamic_cast<File*>(this->currentFolder->children[location])->blocks[i]).toString();
	}
	this->currentFolder = mem;
	return result;
}

std::string FileSystem::getCurrentFilePath() 
{
	Folder* mem = this->currentFolder;
	std::string result = "", temp = "";
	if (this->currentFolder == this->root)
	{
		result = "/";
	}else{
		while (this->currentFolder != this->root) {
			temp = "/" + this->currentFolder->name;
			result = temp + result;
			this->currentFolder = dynamic_cast<Folder*>(this->currentFolder->parent);
		}
	}
	this->currentFolder = mem;
	return result;
}

int FileSystem::move(std::string source, std::string dest) //source includes a file name, dest is a directory (and the name of the new file)
{
	Folder* mem = this->currentFolder; //Remember starting directory
	// Separate the path and the name of the source
	int i = posOfLastNameInPath(source, 0);
	if (i == -1)// Invalid path of source
	{
		this->currentFolder = mem; //Return to starting directory
		return 0;
	}
	std::string name = source.substr(i, source.size() - i);
	File* theFile = this->detachFile(name); //Find the file to be moved

	// Separate the path and the name of the destination
	i = posOfLastNameInPath(dest, -1);
	std::string path = dest.substr(0, i);
	name = dest.substr(i, dest.size() - i);

	theFile->name = name; // Rename the file
	if (this->goToFolder(path) == "/") //Move to desired directory
	{
		// Invalid path of destination
		this->currentFolder = mem; //Return to starting directory
		return 0;
	}
	this->currentFolder->children.push_back(theFile); //Place chosen file in this directory
	this->currentFolder = mem; //Return to starting directory
	return 1;
}

/* Please insert your code */
