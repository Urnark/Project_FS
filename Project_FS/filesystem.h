#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <vector>
#include <string>

#include "memblockdevice.h"

class FileSystem
{
private:
	MemBlockDevice mMemblockDevice;

	// Data structure
	std::string rootName = "/";

	class Node
	{
	public:
		Node* parent = nullptr;
		std::string name;
		int size;
		Node(std::string name = "?") { this->name = name; this->parent = nullptr; };
		virtual ~Node() {};
	};
	
	class Folder : public Node
	{
	public:
		std::vector<Node*> children{};
		Folder(std::string name, Folder* parent) : Node(name) { this->parent = parent; };
		~Folder() {};
		void addFolder(std::string name, Folder* theFolder) { children.push_back(new Folder(name, theFolder)); };
		void addFile(std::string name, char startBlock) { children.push_back(new File(name, startBlock, this)); }; //Försöker lägga en File i en Folder-array // tror detta är fixat
	};

	class File : public Node
	{
	public:
		char startBlock;
		File(std::string name, char startBlock, Folder* parent = nullptr) : Node(name) { this->startBlock = startBlock; this->parent = parent; };
		~File() {};
	};

	Folder* root;
	Folder* currentFolder;
public:
	FileSystem();
	~FileSystem();

	/* Egna funktioner */
	void addFolder(std::string name);
	void addFile(std::string name, char startBlock);
	Folder* removeFolder(std::string name);
	File* removeFile(std::string name);
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
