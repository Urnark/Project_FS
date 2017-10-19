#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "memblockdevice.h"

/*
	root 1 1 2 1
	aa 1 1 1 0
	bb 1 1 0 1
	test 1 1 
	data
	cc 1 1 0 1
	test2 1 1 
	data2
	test3 1 1
	data3
*/

class FileSystem
{
public:
	static enum Ret { FAILURE, SUCCESS, NR, NW };
	static const std::string SAVE_TO_FILE;
	
	class Node
	{
	public:
		Node* parent = nullptr;
		std::string name;
		bool readable;
		bool writable;
		Node(std::string name = "?")
		{
			this->name = name;
			this->readable = true;
			this->writable = true;
		};
		virtual ~Node() {};
	};

	class Folder : public Node
	{
	public:
		std::vector<Node*> children;
		Folder(std::string name, Folder* parent) : Node(name) { this->parent = parent; };
		~Folder() {};
		void addFolder(std::string name, Folder* parentFolder) { children.push_back(new Folder(name, parentFolder)); };
		Node* addFile(std::string name)
		{
			children.push_back(new File(name, this));
			return children.back();
		};
	};

	class File : public Node
	{
	public:
		std::vector<int> blocks;
		File(std::string name, Folder* parent = nullptr) : Node(name) { this->parent = parent; };
		~File() {};
	};
private:
	MemBlockDevice mMemblockDevice;
	std::vector<int> emptyBlocks;
	enum Type { FILE, FOLDER, NONE};
	// Data structure

	Folder* root;
	Folder* currentFolder;

	void addFile(std::string name, std::string data);
	Ret deleteFolder(Folder* folder, bool ignoreRW = false, bool start = true);
	Ret deleteFile(File* file, Folder *parent = nullptr, bool erase = true, bool ignoreRW = false);
	File* detachFile(std::string name);

	int findFolder(std::string name) const;
	int findFile(std::string name) const;
	void parsePath(std::string &temp, std::string &path);
	Ret freeFile(File *file, bool ignoreRW = false);
	int posOfLastNameInPath(std::string path, Type type);
	std::string getPath(Folder* folder);
	File* getFile(const std::string &path);
	Folder* getFolder(const std::string &path);
	void save(std::ofstream &os, Folder* parent = nullptr, Node* node = nullptr);
	void load(std::ifstream &is, int index, Folder* parent = nullptr, Node* node = nullptr);
public:
	FileSystem();
	~FileSystem();

	/* Own functions */
	std::string getPathToParent(std::string path, Type type = Type::NONE);
	std::string getNameFromPath(std::string path, Type type = Type::NONE);
	bool isReadable(std::string path);
	bool isWritable(std::string path);
	bool isFile(const std::string &path);
	bool isFolder(const std::string &path);
	int fileSize(const std::string &path);
	std::string absolutePathfromPath(const std::string &path);
	bool pathExists(std::string path);
	std::string nameToPath(const std::string &name);

	/* These API functions need to be implemented
	You are free to specify parameter lists and return values
	*/

	/* This function creates a file in the filesystem */
	bool createFile(const std::string &filepath, const std::string &data);// createFile(...)

	/* Creates a folder in the filesystem */
	bool createFolder(const std::string &dirpath);// createFolderi(...);

	/* Removes a file in the filesystem */
	Ret removeFile(std::string filepath);// removeFile(...);

	/* Removes a folder in the filesystem */
	Ret removeFolder(std::string path);

	/* Function will move the current location to a specified location in the filesystem*/
	std::string goToFolder(std::string path);// return the last name in the path

	/* This function will get all the files and folders in the specified folder */
	std::vector<Node> listDir(const std::string &path);// listDir(...);

	/* Returns the string a file has written to its memory block */
	std::string getblockString(std::string path, FileSystem::Ret &ret);

	/* Returns a string representing the filepath to currentFolder*/
	std::string getCurrentPath();

	/* Changes read/write rights for a file*/
	bool chmod(int val, std::string path);

	void createImage();
	void restoreImage();
	/* Add your own member-functions if needed */
};

#endif // FILESYSTEM_H
