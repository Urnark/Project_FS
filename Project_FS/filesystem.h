#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <vector>
#include <string>

#include "memblockdevice.h"

class FileSystem
{
private:
	MemBlockDevice mMemblockDevice;
	bool blocksEmpty[250];

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
		std::vector<Node*> children{};
		Folder(std::string name, Folder* parent) : Node(name) { this->parent = parent; };
		~Folder() {};
		void addFolder(std::string name, Folder* theFolder) { children.push_back(new Folder(name, theFolder)); };
		Node* addFile(std::string name, char startBlock)
		{
			children.push_back(new File(name, startBlock, this)); 
			return children.back();
		};
	};

	class File : public Node
	{
	public:
		char startBlock;
		File(std::string name, char startBlock, Folder* parent = nullptr) : Node(name)
		{ 
			this->startBlock = startBlock; 
			this->parent = parent;
		};
		~File() {};
	};
	Folder* root;
	Folder* currentFolder;
	char freeBlock;
public:
	FileSystem();
	~FileSystem();

	/* Egna funktioner */
	void addFolder(std::string name); // using currentFolder
	void addFile(std::string name, std::string data); // using currentFolder
	Folder* unmountFolder(std::string name); // using currentFolder
	void deleteFolder(Folder* folder); // using currentFolder
	File* removeFile(std::string name); // using currentFolder
	void deleteFile(File* file); // using currentFolder
	int changeFolder(std::string path);
	// 0 = failed. 1 = success

	/* These API functions need to be implemented
	You are free to specify parameter lists and return values
	*/

	/* This function creates a file in the filesystem */
	// createFile(...)

	/* Creates a folder in the filesystem */
	// createFolderi(...);

	/* Removes a file in the filesystem */
	// removeFile(...);

	/* Removes a folder in the filesystem */
	// removeFolder(...);

	/* Function will move the current location to a specified location in the filesystem */
	// goToFolder(...);

	/* This function will get all the files and folders in the specified folder */
	// listDir(...);

	/* Add your own member-functions if needed */
};

#endif // FILESYSTEM_H
