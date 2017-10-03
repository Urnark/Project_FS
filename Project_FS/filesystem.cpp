#include "filesystem.h"

FileSystem::FileSystem() {
	this->root = new Folder("root", this->root);
	this->currentFolder = this->root;
}

FileSystem::~FileSystem() {

}

void FileSystem::addFolder(std::string name) {
	this->currentFolder->addFolder(name, this->currentFolder);
}

void FileSystem::addFile(std::string name, char startBlock) {
	//Vi behöver skriva om addFile i Folder-klassen
}

FileSystem::Folder* FileSystem::removeFolder(std::string fileName) {
	Folder* result;
	if (this->currentFolder->children.size() != 0) {
		int location = -1;
		Folder* child = nullptr;
		for (int i = 0; i < this->currentFolder->children.size(); i++) {
			child = dynamic_cast<Folder*>(this->currentFolder->children[i]);
			if (fileName == child->name) {
				location = i;
			}

			if (child->children.size() == 0) {
				result = child;
				this->currentFolder->children.erase[i];
			}
			else {

			}

		}
	}
	return result;
}

FileSystem::File* FileSystem::removeFile(std::string name) {

}

int FileSystem::changeFolder(std::string path) {
	if (path == "..") {
		this->currentFolder = dynamic_cast<Folder*>(this->currentFolder->parent);
	}
	else if (path == ".") {

	}
	else if (path == "") {

	}
}



/* Please insert your code */