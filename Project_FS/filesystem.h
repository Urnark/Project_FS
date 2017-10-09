#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <vector>
#include <string>

#include "memblockdevice.h"

class FileSystem
{
private:
	MemBlockDevice mMemblockDevice;
	std::vector<int> emptyBlocks;
	// Data structure
	std::string rootName = "/";

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

	int findFolder(std::string name) const;
	int findFile(std::string name) const;
	void parsePath(std::string &temp, std::string &path);
	void freeFile(File *file);
public:
	FileSystem();
	~FileSystem();

	/* Egna funktioner */
	Folder* unmountFolder(std::string name); // using currentFolder
	File* detachFile(std::string name); // using currentFolder
	
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
	// removeFolder(...);

	/* Function will move the current location to a specified location in the filesystem*/
	std::string goToFolder(std::string path);// goToFolder(...);

	/* This function will get all the files and folders in the specified folder */
	// listDir(...);

	/* Add your own member-functions if needed */
};

#endif // FILESYSTEM_H
