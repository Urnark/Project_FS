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

bool FileSystem::createFolder(const std::string &dirpath)
{
	bool done = false;
	if (this->pathExists(this->getPathToParent(dirpath)))
	{
		Folder* folder = this->currentFolder;
		this->goToFolder(this->getPathToParent(dirpath));
		this->currentFolder->addFolder(this->getNameFromPath(dirpath), this->currentFolder);
		this->currentFolder = folder;
		done = true;
	}
	return done;
}

bool FileSystem::removeFile(std::string filepath)
{
	bool done = false;
	std::string path = this->getPathToParent(filepath);
	if (this->pathExists(path))
	{
		Folder* folder = this->currentFolder;
		this->goToFolder(path);
		this->deleteFile(getFile(filepath));
		this->currentFolder = folder;
		done = true;
	}
	return done;
}

bool FileSystem::removeFolder(std::string path)
{
	bool done = false;
	if (this->pathExists(path))
	{
		this->goToFolder(path);
		if (this->getPathToParent(path) != this->getCurrentPath() && this->root != this->currentFolder)
		{
			Folder* parent = dynamic_cast<Folder*>(this->currentFolder->parent);
			this->deleteFolder(this->currentFolder);
			this->currentFolder = parent;
			done = true;
		}
	}
	return done;
}

void FileSystem::addFile(std::string name, std::string data)
{
	File* newFile = dynamic_cast<File*>(this->currentFolder->addFile(name));
	newFile->size = data.size();

	int nrOfNeededBlocks = 1;
	/*std::string tempData = data;
	while (tempData.size() > this->mMemblockDevice[0].size())
	{
		nrOfNeededBlocks++;
		tempData = tempData.substr(0,
			(tempData.size() > this->mMemblockDevice[0].size()? tempData.size() - this->mMemblockDevice[0].size(): tempData.size()));
	}*/
	int dsize = data.size();
	while (dsize > this->mMemblockDevice[0].size())
	{
		nrOfNeededBlocks++;
		dsize -=  this->mMemblockDevice[0].size();
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

		remainingData = remainingData.substr((remainingData.size() > this->mMemblockDevice[0].size()  ?this->mMemblockDevice[0].size() : remainingData.size()),
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

bool FileSystem::pathExists(std::string path)
{
	bool exist = true;
	std::string temp = "";
	bool done = true;
	for (int i = 0; i < path.size() && done; i++)
	{
		if (path.at(i) == '/')
			done = false;
	}
	if (!done)
	{
		Folder* mem = this->currentFolder;
		if (path.at(0) == '/')
		{
			this->currentFolder = this->root;
		}
		while (path.size() > 1 && exist)
		{
			this->parsePath(temp, path);
			if (temp != ".." && temp != ".")
			{
				int locationFolder = this->findFolder(temp);
				int locationFile = this->findFile(temp);
				if (locationFolder != -1 && locationFile == -1)
				{
					this->currentFolder = dynamic_cast<Folder*>(this->currentFolder->children[locationFolder]);
				}
				else if (locationFolder == -1 && locationFile == -1)
				{
					exist = false;
				}
			}
			else if (temp == "..")
			{
				this->currentFolder = dynamic_cast<Folder*>(this->currentFolder->parent);
			}
		}
		this->currentFolder = mem;
	}
	else if (path == "")
	{
		exist = false;
	}
	return exist;
}

std::string FileSystem::nameToPath(const std::string & name)
{
	bool done = false;
	std::string path = name;
	for (int i = 0; i < path.size() && !done; i++)
	{
		if (path.at(i) == '/')
			done = true;
	}
	if (!done)
	{
		if (this->getCurrentPath() == "/")
			path = "/" + name;
		else
			path = this->getCurrentPath() + "/" + name;

	}
	return path;
}

/*bool FileSystem::create(const std::string & filepath, const std::string & data)
{
	bool done = false;
	if (this->pathExists(this->getPathToParent(filepath, Type::NONE)))
	{
		this->createFile(filepath, data);
		done = true;
	}
	return done;
}*/

/*bool FileSystem::cd(std::string path, std::string &currentDir)
{
	bool done = false;
	if (this->pathExists(this->getPathToParent(path, Type::NONE)))
	{
		this->goToFolder(path);
		currentDir = this->getCurrentPath();
		done = true;
	}
	return done;
}*/

/*bool FileSystem::cp(std::string oldFilepath, std::string newFilepath)
{
	Folder* mem = this->currentFolder; //Remember starting directory

	bool result = false;
	if (this->pathExists(oldFilepath))// Check if the source exist
	{
		// the old file
		this->goToFolder(this->getPathToParent(oldFilepath, Type::NONE));
		File* thFile = dynamic_cast<File*>(this->currentFolder->children[this->findFile(this->getNameFromPath(oldFilepath, Type::NONE))]);

		// Check if the new path to the folder of the new file exist
		std::string pathToParentOfNewPath = this->getPathToParent(newFilepath, Type::NONE);
		if (this->pathExists(pathToParentOfNewPath))
		{
			this->create(newFilepath, this->getblockString(oldFilepath));
			result = true;
		}
	}

	this->currentFolder = mem; //Return to starting directory
	return result;
}*/

/*bool FileSystem::append(std::string source, std::string dest)
{
	Folder* mem = this->currentFolder; //Remember starting directory

	bool result = false;
	if (this->pathExists(source) && this->pathExists(dest))// Check if the paths exists
	{
		std::string dataDest = this->getblockString(dest);
		this->removeFile(dest);
		std::string newData = dataDest + this->getblockString(source);
		this->create(dest, newData);
		result = true;
	}

	this->currentFolder = mem; //Return to starting directory
	return result;
}*/

/*Delete a folder and its children(inclusive the files in it)*/
void FileSystem::deleteFolder(Folder * folder, bool start)
{
	if (folder->children.size() != 0)
	{
		Folder* child = nullptr;
		int size = folder->children.size();
		for (int i = 0; i < size; i++) {
			child = dynamic_cast<Folder*>(folder->children[i]);
			if (child != nullptr) // is a folder
			{
				deleteFolder(child, false);
			}
		}
		size = folder->children.size();
		for (int i = 0; i < size; i++) {
			child = dynamic_cast<Folder*>(folder->children[i]);
			if (child != nullptr) // is a folder
			{
				delete folder->children[i];
			}
			else // is a file
			{
				this->deleteFile(dynamic_cast<File*>(folder->children[i]), folder, false);
			}
		}
	}
	folder->children.clear();
	if (start)
	{
		if (folder != this->root)
		{
			Folder* pFolder = dynamic_cast<Folder*>(this->currentFolder->parent);
			this->goToFolder(this->getPath(pFolder));
			int location = this->findFolder(folder->name);
			pFolder->children.erase(pFolder->children.begin() + location);
		}
		delete folder;
	}
}

/*Remove a file from the current folder and return that file*/
FileSystem::File* FileSystem::detachFile(std::string name)
{
	File* file = nullptr;
	// Search for right file
	int location = this->findFile(name);
	if (location == -1)
	{
		file = nullptr;
	}
	else
	{
		file = dynamic_cast<File*>(this->currentFolder->children[location]);
		// Remove file from its parents list of children
		this->currentFolder->children.erase(this->currentFolder->children.begin() + location);
	}

	return file;
}

/*Delete a file in the current folder*/
void FileSystem::deleteFile(File * file, Folder *parent, bool erase)
{
	if (file != nullptr)
	{
		if (parent == nullptr) parent = this->currentFolder;
		Folder* mem = this->currentFolder;
		if (erase)
		{
			this->currentFolder = parent;
			int location = this->findFile(file->name);
			parent->children.erase(parent->children.begin() + location);
		}
		this->freeFile(file);
		delete file;

		this->currentFolder = mem;
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
	if (err) temp = "/";
	return temp;
}

/*Create a new file in the FS*/
bool FileSystem::createFile(const std::string &filepath, const std::string &data)
{
	bool done = false;
	if (this->pathExists(this->getPathToParent(filepath)))
	{
		Folder* folder = this->currentFolder;
		this->goToFolder(this->getPathToParent(filepath));
		this->addFile(this->getNameFromPath(filepath), data);
		this->currentFolder = folder;
		done = true;
	}
	return done;
}

int FileSystem::findFolder(std::string name) const
{
	int location = -1;
	Folder* folder = nullptr;
	for (int i = 0; i < this->currentFolder->children.size() && location == -1; i++) {
		folder = dynamic_cast<Folder*>(this->currentFolder->children[i]);
		if (folder != nullptr)
		{
			if (name == folder->name) {
				location = i;
			}
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
		if (file != nullptr)
		{
			if (name == file->name) {
				location = i;
			}
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

int FileSystem::posOfLastNameInPath(std::string path, Type type)
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
	if (i == -1) i = 0;
	if (path == "..") i = 2;
	if (type == Type::FILE)
	{
		if (this->findFile(path.substr(i, path.size() - i)) == -1)
			i = -1;
	}
	else if (type == Type::FOLDER)
	{
		if (this->findFolder(path.substr(i, path.size() - i)) == -1)
			i = -1;
	}

	return i;
}

std::string FileSystem::getPathToParent(std::string path, Type type)
{
	int i = this->posOfLastNameInPath(path, type);
	int nrOfSlash = 0;
	for (int i = 0; i < path.size(); i++)
	{
		if (path.at(i) == '/')
			nrOfSlash++;
	}
	if (nrOfSlash > 1)
	{
		i--;
	}
	return path.substr(0, i);
}

std::string FileSystem::getNameFromPath(std::string path, Type type)
{
	int i = this->posOfLastNameInPath(path, type);
	return path.substr(i, path.size() - i);;
}

FileSystem::File* FileSystem::getFile(const std::string & path)
{
	Folder* mem = this->currentFolder; //Remember starting directory
	File* file = nullptr;
	if (this->pathExists(path))// Check if the source exist
	{
		this->goToFolder(this->getPathToParent(path));
		if (this->findFile(this->getNameFromPath(path)) != -1)
		{
			file = dynamic_cast<File*>(this->currentFolder->children[this->findFile(this->getNameFromPath(path))]);
		}
	}
	this->currentFolder = mem; // Return to starting directory
	return file;
}

std::string FileSystem::getPath(Folder * folder)
{
	std::string path = (folder != this->root ? folder->name : "");
	if (folder != this->root)
	{
		Folder* temp = folder;
		while (dynamic_cast<Folder*>(temp->parent) != this->root)
		{
			path = dynamic_cast<Folder*>(temp->parent)->name + "/" + path;
			temp = dynamic_cast<Folder*>(temp->parent);
		}
	}
	path = "/" + path;
	return path;
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
	std::string result = "?";
	Folder* mem = this->currentFolder;

	if (this->pathExists(path))
	{
		this->goToFolder(this->getPathToParent(path, Type::FILE));
		int location = this->findFile(this->getNameFromPath(path, Type::FILE));
		File* file = dynamic_cast<File*>(this->currentFolder->children[location]);

		if (file->readable)
		{
			result = "";
			for (int i = 0; i < file->blocks.size(); i++)
			{
				std::string data = mMemblockDevice.readBlock(dynamic_cast<File*>(this->currentFolder->children[location])->blocks[i]).toString();
				bool done = false;
				for (int j = data.size() - 1; j >= 0 && !done; j--)
				{
					if (data.at(j) == ' ')
						data = data.substr(0, data.size() - 1);
					else
						done = true;
				}
				result += data;
			}
		}
	}
	this->currentFolder = mem;
	return result;
}

std::string FileSystem::getCurrentPath()
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

/*bool FileSystem::move(std::string source, std::string dest) //source includes a file name, dest is a directory (and the name of the new file)
{
	Folder* mem = this->currentFolder; //Remember starting directory

	bool result = false;
	if (this->pathExists(source))
	{
		//Detach the file from th FS
		this->goToFolder(this->getPathToParent(source, Type::NONE));
		File* theFile = this->detachFile(this->getNameFromPath(source, Type::NONE));

		theFile->name = this->getNameFromPath(dest, Type::NONE);
		if (pathExists(this->getPathToParent(dest, Type::NONE)))
		{
			//Place chosen file in this directory
			this->goToFolder(this->getPathToParent(dest, Type::NONE));
			this->currentFolder->children.push_back(theFile);
			result = true;
		}
	}

	this->currentFolder = mem; //Return to starting directory
	return result;
}*/

bool FileSystem::chmod(int val, std::string path)
{
    Folder* mem = this->currentFolder;
	File* theFile = this->getFile(path);

    if (val == 1) {
        theFile->readable = true;
        theFile->writable = true;
    }
    else if (val == 2) {
        theFile->readable = true;
        theFile->writable = false;
    }
    else if (val == 3) {
        theFile->readable = false;
        theFile->writable = true;
    }
    else if (val == 4) {
        theFile->readable = false;
        theFile->writable = false;
    }
    else {
			  this->currentFolder = mem;
        return false;
    }
    this->currentFolder = mem;
	return true;
}

/* Please insert your code */
