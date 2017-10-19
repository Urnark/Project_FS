#include "filesystem.h"

const std::string FileSystem::SAVE_TO_FILE = "imageFS.txt";

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
	deleteFolder(this->root, true);
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

FileSystem::Ret FileSystem::removeFile(std::string filepath)
{
	FileSystem::Ret ret = FileSystem::Ret::FAILURE;
	std::string path = this->getPathToParent(filepath);
	if (this->pathExists(path))
	{
		ret = FileSystem::Ret::SUCCESS;
		Folder* folder = this->currentFolder;
		this->goToFolder(path);
		ret = this->deleteFile(this->getFile(filepath));
		this->currentFolder = folder;
		
	}
	return ret;
}

FileSystem::Ret FileSystem::removeFolder(std::string path)
{
	FileSystem::Ret ret = FileSystem::Ret::FAILURE;
	if (this->pathExists(path))
	{
		this->goToFolder(path);
		if (this->getPathToParent(path) != this->getCurrentPath() && this->root != this->currentFolder)
		{
			ret = FileSystem::Ret::SUCCESS;
			Folder* parent = dynamic_cast<Folder*>(this->currentFolder->parent);
			ret = this->deleteFolder(this->currentFolder);
			this->currentFolder = parent;
			
		}
	}
	return ret;
}

void FileSystem::addFile(std::string name, std::string data)
{
	File* newFile = dynamic_cast<File*>(this->currentFolder->addFile(name));

	int nrOfNeededBlocks = 1;
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

/*Delete a folder and its children(inclusive the files in it)*/
FileSystem::Ret FileSystem::deleteFolder(Folder * folder, bool ignoreRW, bool start)
{
	FileSystem::Ret ret = FileSystem::Ret::FAILURE;
	if (folder->children.size() != 0)
	{
		Node* child = nullptr;
		if (start && !ignoreRW)
		{
			if (this->isW(folder))
				ret = FileSystem::Ret::NW;
		}
		
		if (ret != FileSystem::Ret::NW || ignoreRW)
		{
			int size = folder->children.size();
			for (int i = 0; i < size; i++) {
				child = folder->children[i];
				if (dynamic_cast<Folder*>(child) != nullptr) // is a folder
				{
					deleteFolder(dynamic_cast<Folder*>(child), ignoreRW, false);
				}
			}
			size = folder->children.size();
			for (int i = 0; i < size; i++) {
				child = folder->children[i];
				if (dynamic_cast<Folder*>(child) != nullptr) // is a folder
				{
					delete folder->children[i];
				}
				else // is a file
				{
					this->deleteFile(dynamic_cast<File*>(folder->children[i]), folder, false, ignoreRW);
				}
			}
		}
	}
	if (ret != FileSystem::Ret::NW || ignoreRW)
	{
		ret = FileSystem::Ret::SUCCESS;
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
	return ret;
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

bool FileSystem::isW(Folder * folder)
{
	bool nw = false;
	for (int i = 0; i < folder->children.size() && !nw; i++)
	{
		if (!folder->children[i]->writable)
			nw = true;
		else if (dynamic_cast<Folder*>(folder->children[i]) != nullptr)
		{
			nw = isW(dynamic_cast<Folder*>(folder->children[i]));
		}
	}
	return nw;
}

/*Delete a file in the current folder*/
FileSystem::Ret FileSystem::deleteFile(File * file, Folder *parent, bool erase, bool ignoreRW)
{
	FileSystem::Ret ret = FileSystem::Ret::FAILURE;
	if (file != nullptr)
	{
		ret = FileSystem::Ret::NW;
		if (file->writable || ignoreRW)
		{
			if (parent == nullptr) parent = this->currentFolder;
			Folder* mem = this->currentFolder;
			if (erase)
			{
				this->currentFolder = parent;
				int location = this->findFile(file->name);
				parent->children.erase(parent->children.begin() + location);
			}
			ret = this->freeFile(file, ignoreRW);
			delete file;

			this->currentFolder = mem;
		}
	}
	return ret;
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

std::vector<FileSystem::Node> FileSystem::listDir(const std::string &path)
{
	std::vector<Node> list;
	Folder* mem = this->currentFolder;
	std::string p = (path == "./"? this->getCurrentPath(): path);
	if (this->pathExists(path) && this->isFolder(path))
	{
		this->goToFolder(path);
		for (int i = 0; i < this->currentFolder->children.size(); i++)
			list.push_back(*this->currentFolder->children[i]);
	}
	this->currentFolder = mem;
	return list;
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
FileSystem::Ret FileSystem::freeFile(File *file, bool ignoreRW)
{
	FileSystem::Ret ret = FileSystem::Ret::NW;
	if (file->writable || ignoreRW)
	{
		ret = FileSystem::Ret::SUCCESS;
		for (int i = 0; i < file->blocks.size(); i++)
		{
			this->mMemblockDevice[file->blocks[i]].reset();
			this->emptyBlocks.push_back(file->blocks[i]);
		}
		file->blocks.clear();
	}
	return ret;
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

std::string FileSystem::getPathToParent(std::string path)
{
	int i = this->posOfLastNameInPath(path, Type::NONE);
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

std::string FileSystem::getNameFromPath(std::string path)
{
	int i = this->posOfLastNameInPath(path, Type::NONE);
	return path.substr(i, path.size() - i);;
}

bool FileSystem::isReadable(std::string path)
{
	bool r = false;
	if (this->pathExists(path))
	{
		File* file = this->getFile(path);
		if (file != nullptr)
		{
			r = file->readable;
		}
		Folder* folder = this->getFolder(path);
		if (folder != nullptr)
		{
			r = folder->readable;
		}
	}
	return r;
}

bool FileSystem::isWritable(std::string path)
{
	bool w = false;
	if (this->pathExists(path))
	{
		File* file = this->getFile(path);
		if (file != nullptr)
		{
			w = file->writable;
		}
		Folder* folder = this->getFolder(path);
		if (folder != nullptr)
		{
			w = folder->writable;
		}
	}
	return w;
}

bool FileSystem::isFile(const std::string & path)
{
	bool is = false;
	if (this->pathExists(path))
	{
		if (this->getFile(path) != nullptr)
			is = true;
	}
	return is;
}

bool FileSystem::isFolder(const std::string & path)
{
	bool is = false;
	if (this->pathExists(path))
	{
		if (this->getFile(path) == nullptr)
			is = true;
	}
	return is;
}

int FileSystem::fileSize(const std::string & path)
{
	FileSystem::Ret ret = FileSystem::Ret::FAILURE;
	std::string temp = this->getblockString(path, ret);
	return temp.size();
}

std::string FileSystem::absolutePathFromPath(const std::string & path)
{
	std::string absPath = "/";
	if (this->pathExists(path))
	{
		Folder* mem = this->currentFolder;
		this->goToFolder(path);
		absPath = getCurrentPath();
		this->currentFolder = mem;
	}
	return absPath;
}

FileSystem::File* FileSystem::getFile(const std::string & path)
{
	Folder* mem = this->currentFolder; //Remember starting directory
	File* file = nullptr;
	if (this->pathExists(path))// Check if the source exist
	{
		this->goToFolder(this->getPathToParent(path));
		int location = this->findFile(this->getNameFromPath(path));
		if (location != -1)
		{
			file = dynamic_cast<File*>(this->currentFolder->children[location]);
		}
	}
	this->currentFolder = mem; // Return to starting directory
	return file;
}

FileSystem::Folder* FileSystem::getFolder(const std::string & path)
{
	Folder* mem = this->currentFolder; //Remember starting directory
	Folder* folder = nullptr;
	if (this->pathExists(path))// Check if the source exist
	{
		this->goToFolder(this->getPathToParent(path));
		int location = this->findFolder(this->getNameFromPath(path));
		if (location != -1)
		{
			folder = dynamic_cast<Folder*>(this->currentFolder->children[location]);
		}
	}
	this->currentFolder = mem; // Return to starting directory
	return folder;
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

std::string FileSystem::getblockString(std::string path, FileSystem::Ret &ret)
{
	std::string result = "";
	Folder* mem = this->currentFolder;

	ret = FileSystem::Ret::FAILURE;
	if (this->pathExists(path) && this->isFile(path))
	{
		this->goToFolder(this->getPathToParent(path));
		int location = this->findFile(this->getNameFromPath(path));
		File* file = dynamic_cast<File*>(this->currentFolder->children[location]);

		ret = FileSystem::Ret::NR;
		if (file->readable)
		{
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
			ret = FileSystem::Ret::SUCCESS;
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

bool FileSystem::chmod(int val, std::string path)
{
    Folder* mem = this->currentFolder;
	File* theFile = this->getFile(path);
	bool result = false;
	if (theFile != nullptr)
	{
		if (val == 1) { // 11
			theFile->readable = true;
			theFile->writable = true;
			result = true;
		}
		else if (val == 2) { // 10
			theFile->readable = true;
			theFile->writable = false;
			result = true;
		}
		else if (val == 3) { // 01
			theFile->readable = false;
			theFile->writable = true;
			result = true;
		}
		else if (val == 4) { // 00
			theFile->readable = false;
			theFile->writable = false;
			result = true;
		}
		else
		{
			this->currentFolder = mem;
			return false;
		}
	}
    this->currentFolder = mem;
	return result;
}

void FileSystem::save(std::ofstream &os, Folder* parent, Node* node)
{
	if (parent == nullptr)
	{
		node = this->root;
		parent = dynamic_cast<Folder*>(this->root);
	}

	os << node->name << " " << node->readable << " " << node->writable;
	if (dynamic_cast<Folder*>(node) != nullptr)
	{
		Folder* folder = dynamic_cast<Folder*>(node);
		parent = folder;
		int folders = 0;
		int files = 0;
		for (int i = 0; i < folder->children.size(); i++)
		{
			if (dynamic_cast<Folder*>(folder->children[i]))
				folders++;
			else
				files++;
		}
		os << " " << folders << " " << files << "\n";
		for (int i = 0; i < folder->children.size(); i++)
		{
			save(os, parent, folder->children[i]);
		}
	}
	else
	{
		File* file = dynamic_cast<File*>(node);
		FileSystem::Ret ret = FileSystem::Ret::FAILURE;
		std::string path = this->getPath(parent) + "/" + file->name;
		if (parent == this->root) path = "/" + file->name;
		os << "\n" << this->getblockString(path, ret) << "\n";
	}
}

void FileSystem::load(std::ifstream &is, int index, Folder* parent, Node* node)
{
	std::string line;
	getline(is, line);
	std::stringstream ssin(line);
	std::string word[5];
	int wordi = 0;
	while (ssin.good()) {
		ssin >> word[wordi];
		wordi++;
	}
	std::string path = this->getPath(parent) + "/" + std::to_string(index);
	if (parent == this->root) path = "/" + std::to_string(index);
	if (wordi == 5) // is a file
	{
		this->removeFolder(path);
		path = this->getPath(parent) + "/" + word[0];
		if (parent == this->root) path = "/" + word[0];
		this->createFolder(path);
		node = this->getFolder(path);
		node->readable = std::atoi(word[1].c_str());
		node->writable = std::atoi(word[2].c_str());
		int nrOfFolders = std::atoi(word[3].c_str());
		int nrOfFiles = std::atoi(word[4].c_str());
		parent = dynamic_cast<Folder*>(node);
		for (int i = 0; i < nrOfFolders; i++)
		{
			this->createFolder(path + "/" + std::to_string(i));
			load(is, i, parent, this->getFolder(path + "/" + std::to_string(i)));
		}
		for (int i = 0; i < nrOfFiles; i++)
		{
			this->createFile(path + "/" + std::to_string(nrOfFolders + i), "");
			load(is, nrOfFolders + i, parent, this->getFile(path + "/" + std::to_string(nrOfFolders + i)));
		}
	}
	else
	{
		this->removeFile(path);
		path = this->getPath(parent) + "/" + word[0];
		if (parent == this->root) path = "/" + word[0];
		getline(is, line);
		this->createFile(path, line);
		node = this->getFile(path);
		node->readable = std::atoi(word[1].c_str());
		node->writable = std::atoi(word[2].c_str());
	}

}

void FileSystem::createImage()
{
	std::ofstream os(FileSystem::SAVE_TO_FILE);
	if (os.is_open())
	{
		save(os);
		os.close();
	}
}

void FileSystem::restoreImage()
{
	std::ifstream is(FileSystem::SAVE_TO_FILE);
	if (is.is_open())
	{
		std::string line;
		getline(is, line);
		std::stringstream ssin(line);
		std::string word[5];
		int index = 0;
		while (ssin.good()) {
			ssin >> word[index];
			index++;
		}
		this->root->name = word[0];
		this->root->readable = std::atoi(word[1].c_str());
		this->root->writable = std::atoi(word[2].c_str());
		int nrOfFolders = std::atoi(word[3].c_str());
		int nrOfFiles = std::atoi(word[4].c_str());
		Folder* parent = dynamic_cast<Folder*>(this->root);
		for (int i = 0; i < nrOfFolders; i++)
		{
			this->createFolder("/" + std::to_string(i));
			load(is, i, parent, this->getFolder("/" + std::to_string(i)));
		}
		for (int i = 0; i < nrOfFiles; i++)
		{
			this->createFile("/" + std::to_string(nrOfFolders + i), "");
			load(is, nrOfFolders + i, parent, this->getFile("/" + std::to_string(nrOfFolders + i)));
		}
		is.close();
	}
	this->currentFolder = this->root;
}

/* Please insert your code */
