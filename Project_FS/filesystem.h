#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <vector>
#include <string>
#include <iostream>

#include "memblockdevice.h"

class FileSystem
{
private:
	MemBlockDevice mMemblockDevice;
	std::vector<int> emptyBlocks;
	enum Type { FILE, FOLDER, NONE};
	// Data structure

	class Node
	{
	public:
		Node* parent = nullptr;
		std::string name;
		int size;
		Node(std::string name = "?") { this->name = name;};
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
	Folder* root;
	Folder* currentFolder;

	void addFile(std::string name, std::string data);
	void deleteFolder(Folder* folder);
	void deleteFile(File* file, Folder *parent = nullptr);
    File* detachFile(std::string name);

	int findFolder(std::string name) const;
	int findFile(std::string name) const;
	void parsePath(std::string &temp, std::string &path);
	void freeFile(File *file);
	/* return the position of the last name in the path, need to know if the last name is a file or folder
	   Ex: if path = "/aa/b/testfile" return 6*/
	int posOfLastNameInPath(std::string path, Type type); // 0 = filepath, 1 = folderpath
	std::string getPathToParent(std::string path, Type type);
	std::string getNameFromPath(std::string path, Type type);
public:
	FileSystem();
	~FileSystem();

	/* Egna funktioner */
	Folder* unmountFolder(std::string name); // using currentFolder
	bool pathExists(std::string path);

	bool create(const std::string &filepath, const std::string &data);
	bool mkdir(std::string name);
	bool cd(std::string path, std::string &currentDir);
	// 0 = failed. 1 = success

	/* These API functions need to be implemented
	You are free to specify parameter lists and return values
	*/

	/* This function creates a file in the filesystem */
	void createFile(const std::string &filepath, const std::string &data);// createFile(...)

	/* Creates a folder in the filesystem */
	void createFolder(const std::string &dirpath);// createFolderi(...);

	/* Removes a file in the filesystem */
	void removeFile(std::string filepath);// removeFile(...);

	/* Removes a folder in the filesystem */
	void removeFolder(std::string path);

	/* Function will move the current location to a specified location in the filesystem*/
	std::string goToFolder(std::string path);// return the last name in the path

	/* This function will get all the files and folders in the specified folder */
	// listDir(...);

	/* Returns a string containing all children of currentFolder */
	std::string displayChildren(std::string path = "./");

	/* Returns the string a file has written to its memory block */
	std::string getblockString(std::string path);

	/* Returns a string representing the filepath to currentFolder*/
	std::string getCurrentFilePath();

	/* Renames a file and moves it from source to dest */
	bool move(std::string source, std::string dest);

	/* Add your own member-functions if needed */
};

#endif // FILESYSTEM_H
