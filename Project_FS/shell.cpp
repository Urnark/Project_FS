#include <iostream>
#include <sstream>
#include "filesystem.h"

const int MAXCOMMANDS = 8;
const int NUMAVAILABLECOMMANDS = 16;

std::string availableCommands[NUMAVAILABLECOMMANDS] = {
    "quit","format","ls","create","cat","createImage","restoreImage",
    "rm","cp","append","mv","mkdir","cd","pwd","chmod","help"
};

/* Takes usercommand from input and returns number of commands, commands are stored in strArr[] */
int parseCommandString(const std::string &userCommand, std::string strArr[]);
int findCommand(std::string &command);
bool quit();
std::string help();

/* More functions ... */
void errSyntax(std::string cmdArr[]);
void errFS(std::string cmdArr[]);
void errNR(std::string file);
void errNW(std::string file);
void format(FileSystem* &fs);
void ls(FileSystem* &fs, std::string cmdArr[], int nrOfCommands);
void create(FileSystem* &fs, std::string cmdArr[], int nrOfCommands);
void cat(FileSystem* &fs, std::string cmdArr[], int nrOfCommands);
void createImage(FileSystem* &fs, std::string cmdArr[], int nrOfCommands);
void restoreImage(FileSystem* &fs, std::string cmdArr[], int nrOfCommands);
void rm(FileSystem* &fs, std::string cmdArr[], std::string &currentDir, int nrOfCommands);
void cp(FileSystem* &fs, std::string cmdArr[], int nrOfCommands);
void append(FileSystem* &fs, std::string cmdArr[], int nrOfCommands);
void mv(FileSystem* &fs, std::string cmdArr[], int nrOfCommands);
void mkdir(FileSystem* &fs, std::string cmdArr[], int nrOfCommands);
void cd(FileSystem* &fs, std::string cmdArr[], std::string &currentDir, int nrOfCommands);
void pwd(FileSystem* &fs, std::string cmdArr[], int nrOfCommands);
void chmod(FileSystem* &fs, std::string cmdArr[], int nrOfCommands);
std::string setw(unsigned int val);

int main(void) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	std::string userCommand, commandArr[MAXCOMMANDS];
	std::string user = "user@DV1492";    // Change this if you want another user to be displayed
	std::string currentDir = "/";    // current directory, used for output

    bool bRun = true;

	FileSystem* fs = nullptr;

    do {
        std::cout << user << ":" << currentDir << "$ ";
        getline(std::cin, userCommand);

        int nrOfCommands = parseCommandString(userCommand, commandArr);
        if (nrOfCommands > 0) {
            int cIndex = findCommand(commandArr[0]);
            switch(cIndex) {

			case 0: //quit
				bRun = quit();
                break;
            case 1: // format
				format(fs);
                break;
            case 2: // ls
				ls(fs, commandArr, nrOfCommands);
                break;
            case 3: // create
				create(fs, commandArr, nrOfCommands);
                break;
            case 4: // cat
				cat(fs, commandArr, nrOfCommands);
                break;
            case 5: // createImage
				createImage(fs, commandArr, nrOfCommands);
                break;
            case 6: // restoreImage
				restoreImage(fs, commandArr, nrOfCommands);
                break;
            case 7: // rm
				rm(fs, commandArr, currentDir, nrOfCommands);
                break;
            case 8: // cp
				cp(fs, commandArr, nrOfCommands);
                break;
            case 9: // append
				append(fs, commandArr, nrOfCommands);
                break;
            case 10: // mv
				mv(fs, commandArr, nrOfCommands);
                break;
            case 11: // mkdir
				mkdir(fs, commandArr, nrOfCommands);
                break;
            case 12: // cd
				cd(fs, commandArr, currentDir, nrOfCommands);
                break;
            case 13: // pwd
				pwd(fs, commandArr, nrOfCommands);
                break;
            case 14: // chmod
                chmod(fs, commandArr, nrOfCommands);
                break;
            case 15: // help
                std::cout << help() << std::endl;
                break;
            default:
                std::cout << "Unknown command: " << commandArr[0] << std::endl;
            }
        }
    } while (bRun == true);
	delete fs;
    return 0;
}

int parseCommandString(const std::string &userCommand, std::string strArr[]) {
    std::stringstream ssin(userCommand);
    int counter = 0;
    while (ssin.good() && counter < MAXCOMMANDS) {
        ssin >> strArr[counter];
        counter++;
    }
    if (strArr[0] == "") {
        counter = 0;
    }
    return counter;
}
int findCommand(std::string &command) {
    int index = -1;
    for (int i = 0; i < NUMAVAILABLECOMMANDS && index == -1; ++i) {
        if (command == availableCommands[i]) {
            index = i;
        }
    }
    return index;
}

bool quit() {
	std::cout << "Exiting\n";
	return false;
}

std::string help() {
    std::string helpStr;
    helpStr += "OSD Disk Tool .oO Help Screen Oo.\n";
    helpStr += "-----------------------------------------------------------------------------------\n" ;
    helpStr += "* quit:                             Quit OSD Disk Tool\n";
    helpStr += "* format;                           Formats disk\n";
    helpStr += "* ls     <path>:                    Lists contents of <path>.\n";
    helpStr += "* create <path>:                    Creates a file and stores contents in <path>\n";
    helpStr += "* cat    <path>:                    Dumps contents of <file>.\n";
    helpStr += "* createImage  <real-file>:         Saves disk to <real-file>\n";
    helpStr += "* restoreImage <real-file>:         Reads <real-file> onto disk\n";
    helpStr += "* rm     <file>:                    Removes <file>\n";
    helpStr += "* cp     <source> <destination>:    Copy <source> to <destination>\n";
    helpStr += "* append <source> <destination>:    Appends contents of <source> to <destination>\n";
    helpStr += "* mv     <old-file> <new-file>:     Renames <old-file> to <new-file>\n";
    helpStr += "* mkdir  <directory>:               Creates a new directory called <directory>\n";
    helpStr += "* cd     <directory>:               Changes current working directory to <directory>\n";
    helpStr += "* pwd:                              Get current working directory\n";
    helpStr += "* chmod <permission> <file>:        Change access rights for a file to <permission>\n";
    helpStr += "* help:                             Prints this help screen\n";
    return helpStr;
}

/* Insert code for your shell functions and call them from the switch-case */
void errSyntax(std::string cmdArr[])
{
	std::cout << "Wrong syntax, use the <help> command to see the syntax for <" << cmdArr[0] << ">" << std::endl;
}

void errFS(std::string cmdArr[])
{
	std::cout << "Format disk first before calling commando <" << cmdArr[0] << ">" << std::endl;
}

void errNR(std::string file)
{
	std::cout << "The file '" << file << "' is not readable!" << std::endl;
}

void errNW(std::string file)
{
	std::cout << "The file '" << file << "' is not writable!" << std::endl;
}

void format(FileSystem* &fs)
{
	if (fs != nullptr)
		delete fs;
	fs = new FileSystem();
}

void ls(FileSystem* &fs, std::string cmdArr[], int nrOfCommands)
{
	if (fs == nullptr)
	{
		errFS(cmdArr);
		return;
	}
	if (nrOfCommands > 2)
	{
		errSyntax(cmdArr);
		return;
	}
	std::string path = "./";
	if (nrOfCommands > 1) path = fs->nameToPath(cmdArr[1]);
	if ((path == "./"? true : fs->isFolder(path)))
	{
		
		std::vector<FileSystem::Node> list = fs->listDir(path);
		path = fs->absolutePathFromPath(path);
		std::string result = "";
		FileSystem::Node node;
		int mNameSize = 10;
		for (int i = 0; i < list.size(); i++)
		{
			node = list[i];
			if (node.name.size() >= mNameSize)
				mNameSize = node.name.size() + 1;
		}
		result += "Type" + setw(10 - 4) + "Name" + setw(mNameSize - 4) +
			"Permissions" + setw(15 - 11) + "Size" + setw(10 - 4) + "\n";
		std::string type = "DIR";
		for (int i = 0; i < list.size(); i++)
		{
			node = list[i];
			int size = 0;
			type = "DIR";

			if (dynamic_cast<FileSystem::File*>(&node) != nullptr)
			{
				size = fs->fileSize((path == "/" ? "" : path) + "/" + node.name);
				type = "FILE";
			}
			std::string rw = "";
			if (fs->isReadable((path == "/" ? "" : path) + "/" + node.name)) rw += "R";
			if (fs->isWritable((path == "/" ? "" : path) + "/" + node.name)) rw += "W";

			result += type + setw(10 - type.size()) +
				node.name + setw(mNameSize - node.name.size()) +
				rw + setw(15 - rw.size()) + std::to_string(size) + "\n";
		}
		
		std::cout << "Listing directory" << std::endl;
		std::cout << result << std::endl;
	}
	else
	{
		errSyntax(cmdArr);
	}
}

void create(FileSystem* & fs, std::string cmdArr[], int nrOfCommands)
{
	if (fs == nullptr)
	{
		errFS(cmdArr);
		return;
	}
	if (nrOfCommands != 2)
	{
		errSyntax(cmdArr);
		return;
	}
	std::cout << "Enter data: ";
	std::string data = "";
	getline(std::cin, data);

	if (data == "mb1") // For testing. make so data = block.size + 1, 
	{						// so the file take up two block for the data
		data = "";
		for (int i = 0; i < 512; i++) data += std::to_string(i % 10);
		data += "#";
	}

	// create a new file
	std::string path = fs->nameToPath(cmdArr[1]);
	// Check if the path to the directory that the file shall be in exist
	if (!fs->createFile(path, data))
	{
		errSyntax(cmdArr);
	}
}

void cat(FileSystem* & fs, std::string cmdArr[], int nrOfCommands)
{
	if (fs == nullptr)
	{
		errFS(cmdArr);
		return;
	}
	if (nrOfCommands != 2)
	{
		errSyntax(cmdArr);
		return;
	}
	std::string path = fs->nameToPath(cmdArr[1]);
	if (fs->pathExists(path))
	{
		FileSystem::Ret ret = FileSystem::Ret::FAILURE;
		std::string data = fs->getblockString(path, ret);
		if (ret == FileSystem::Ret::SUCCESS)
			std::cout << data << std::endl;
		else if (ret == FileSystem::Ret::NR)
			errNR(fs->getNameFromPath(path));
		else
			errSyntax(cmdArr);
	}
	else
	{
		errSyntax(cmdArr);
	}
}

void createImage(FileSystem *& fs, std::string cmdArr[], int nrOfCommands)
{
	if (nrOfCommands == 1)
		fs->createImage();
	else
		errSyntax(cmdArr);
}

void restoreImage(FileSystem *& fs, std::string cmdArr[], int nrOfCommands)
{
	if (nrOfCommands == 1)
	{
		format(fs);
		fs->restoreImage();
	}
	else
		errSyntax(cmdArr);
}

void rm(FileSystem* & fs, std::string cmdArr[], std::string &currentDir, int nrOfCommands)
{
	if (fs == nullptr)
	{
		errFS(cmdArr);
		return;
	}
	if (nrOfCommands != 2)
	{
		errSyntax(cmdArr);
		return;
	}
	FileSystem::Ret ret = FileSystem::Ret::FAILURE;
	std::string path = fs->nameToPath(cmdArr[1]);
	if (fs->pathExists(path))
	{
		if (fs->isFile(path)) // is a file
		{
			ret = fs->removeFile(path);
		}
		else // is a folder
		{
			ret = fs->removeFolder(path);
		}
		currentDir = fs->getCurrentPath();
	}

	if (ret == FileSystem::Ret::FAILURE)
	{
		errSyntax(cmdArr);
	}
	else if (ret == FileSystem::Ret::NW)
	{
		errNW(fs->getNameFromPath(path));
	}
}

void cp(FileSystem *& fs, std::string cmdArr[], int nrOfCommands)
{
	if (fs == nullptr)
	{
		errFS(cmdArr);
		return;
	}
	if (nrOfCommands != 3)
	{
		errSyntax(cmdArr);
		return;
	}
	FileSystem::Ret ret = FileSystem::Ret::FAILURE;
	std::string oldPath = fs->nameToPath(cmdArr[1]);
	std::string newPath = fs->nameToPath(cmdArr[2]);
	bool result = false;
	if (fs->pathExists(oldPath))// Check if the old path exist
	{
		// Check if the new path to the folder of the new file exist
		std::string pathToParentOfNewPath = fs->getPathToParent(newPath);
		if (fs->pathExists(pathToParentOfNewPath) && cmdArr[2] != "")
		{
			std::string data = fs->getblockString(oldPath, ret);
			if (ret == FileSystem::Ret::SUCCESS) // Check if the old path is readable
			{
				fs->createFile(newPath, data);
				result = true;
			}
		}
	}

	if (!result)
	{
		errSyntax(cmdArr);
	}
	if (ret == FileSystem::Ret::NR)
	{
		errNR(fs->getNameFromPath(oldPath));
	}
}

void append(FileSystem *& fs, std::string cmdArr[], int nrOfCommands)
{
	if (fs == nullptr)
	{
		errFS(cmdArr);
		return;
	}
	if (nrOfCommands != 3)
	{
		errSyntax(cmdArr);
		return;
	}
	FileSystem::Ret retSource = FileSystem::Ret::FAILURE;
	FileSystem::Ret retDest = FileSystem::Ret::FAILURE;
	std::string source = fs->nameToPath(cmdArr[1]);
	std::string dest = fs->nameToPath(cmdArr[2]);
	if (fs->pathExists(source) && fs->pathExists(dest))// Check if the paths exists
	{
		std::string dataSource = fs->getblockString(source, retSource);
		std::string dataDest = fs->getblockString(dest, retDest);
		//Check reading and writing privileges
		if (retSource == FileSystem::Ret::SUCCESS && retDest == FileSystem::Ret::SUCCESS)
		{
			fs->removeFile(dest);
			std::string newData = dataDest + dataSource;
			fs->createFile(dest, newData);
		}
	}
	else
	{
		errSyntax(cmdArr);
	}

	if (retSource == FileSystem::Ret::NR)
		errNR(fs->getNameFromPath(source));
	if (retDest == FileSystem::Ret::NR)
		errNR(fs->getNameFromPath(dest));
}

void mv(FileSystem* & fs, std::string cmdArr[], int nrOfCommands)
{
	if (fs == nullptr)
	{
		errFS(cmdArr);
		return;
	}
	if (nrOfCommands != 3)
	{
		errSyntax(cmdArr);
		return;
	}
	FileSystem::Ret retOld = FileSystem::Ret::FAILURE;
	std::string oldPath = fs->nameToPath(cmdArr[1]);
	std::string newPath = fs->nameToPath(cmdArr[2]);
	bool result = false;
	if (fs->pathExists(oldPath)) // Check if the old path exist and is writable
	{
		// Check if the new path to the folder of the new file exist
		std::string pathToParentOfNewPath = fs->getPathToParent(newPath);
		if (fs->pathExists(pathToParentOfNewPath) && cmdArr[2] != "")
		{
			std::string data = fs->getblockString(oldPath, retOld);
			if (retOld == FileSystem::Ret::SUCCESS)
			{
				fs->createFile(newPath, data);
				retOld = fs->removeFile(oldPath);
				result = true;
			}
		}
	}
	if (retOld == FileSystem::Ret::NR)
		errNR(fs->getNameFromPath(oldPath));
	if (retOld == FileSystem::Ret::NW)
		errNW(fs->getNameFromPath(oldPath));
	if (!result)
		errSyntax(cmdArr);
}

void mkdir(FileSystem* & fs, std::string cmdArr[], int nrOfCommands)
{
	if (fs == nullptr)
	{
		errFS(cmdArr);
		return;
	}
	if (nrOfCommands != 2 || cmdArr[1] == "")
	{
		errSyntax(cmdArr);
		return;
	}
	if (!fs->createFolder(fs->nameToPath(cmdArr[1])))
	{
		errSyntax(cmdArr);
	}
}

void cd(FileSystem* & fs, std::string cmdArr[], std::string &currentDir, int nrOfCommands)
{
	if (fs == nullptr)
	{
		errFS(cmdArr);
		return;
	}
	if (nrOfCommands != 2 || cmdArr[1] == "")
	{
		errSyntax(cmdArr);
		return;
	}
	// Change directory
	std::string path = fs->nameToPath(cmdArr[1]);
	if (fs->pathExists(fs->getPathToParent(path)))
	{
		fs->goToFolder(path);
		currentDir = fs->getCurrentPath();
	}else{
		std::cout << "The path does not exist in the filesystem" << std::endl;
	}
}

void pwd(FileSystem* & fs, std::string cmdArr[], int nrOfCommands)
{
	if (fs == nullptr)
	{
		errFS(cmdArr);
		return;
	}
	if (nrOfCommands != 1)
	{
		errSyntax(cmdArr);
		return;
	}
	std::cout << fs->getCurrentPath() << std::endl;
}

void chmod(FileSystem* & fs, std::string cmdArr[], int nrOfCommands)
{
	if (fs == nullptr)
	{
		errFS(cmdArr);
		return;
	}
	if (nrOfCommands != 3 || cmdArr[1] == "" || cmdArr[2] == "")
	{
		errSyntax(cmdArr);
		return;
	}
	int result;
	if (std::stringstream(cmdArr[1]) >> result)
	{
		if (!fs->chmod(result, fs->nameToPath(cmdArr[2])))
		{
			errSyntax(cmdArr);
		}
	}
	else
	{
		errSyntax(cmdArr);
	}
}

std::string setw(unsigned int val)
{
	std::string str = "";
	for (int i = 0; i < val; i++)
	{
		str += " ";
	}
	return str;
}
