#include <iostream>
#include <sstream>
#include "filesystem.h"

const int MAXCOMMANDS = 8;
const int NUMAVAILABLECOMMANDS = 15;

std::string availableCommands[NUMAVAILABLECOMMANDS] = {
    "quit","format","ls","create","cat","createImage","restoreImage",
    "rm","cp","append","mv","mkdir","cd","pwd","help"
};

/* Takes usercommand from input and returns number of commands, commands are stored in strArr[] */
int parseCommandString(const std::string &userCommand, std::string strArr[]);
int findCommand(std::string &command);
bool quit();
std::string help();

/* More functions ... */
void errSyntax(std::string cmdArr[]);
void errFS(std::string cmdArr[]);
void format(FileSystem* &fs);
void ls(FileSystem* &fs, std::string cmdArr[], int nrOfCommands);
void create(FileSystem* &fs, std::string cmdArr[]);
void cat(FileSystem* &fs, std::string cmdArr[]);
void rm(FileSystem* &fs, std::string cmdArr[]);
void cp(FileSystem* &fs, std::string cmdArr[]);
void append(FileSystem* &fs, std::string cmdArr[]);
void mv(FileSystem* &fs, std::string cmdArr[]);
void mkdir(FileSystem* &fs, std::string cmdArr[]);
void cd(FileSystem* &fs, std::string cmdArr[], std::string &currentDir);
void pwd(FileSystem* &fs, std::string cmdArr[]);

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
				create(fs, commandArr);
                break;
            case 4: // cat
				cat(fs, commandArr);
                break;
            case 5: // createImage
                break;
            case 6: // restoreImage
                break;
            case 7: // rm
				rm(fs, commandArr);
                break;
            case 8: // cp
				cp(fs, commandArr);
                break;
            case 9: // append
				append(fs, commandArr);
                break;
            case 10: // mv
				mv(fs, commandArr);
                break;
            case 11: // mkdir
				mkdir(fs, commandArr);
                break;
            case 12: // cd
				cd(fs, commandArr, currentDir);
                break;
            case 13: // pwd
				pwd(fs, commandArr);
                break;
            case 14: // help
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

void format(FileSystem* &fs)
{
	if (fs != nullptr) delete fs;
	fs = new FileSystem(); // Tror detta borde fungera bättre i denn situation
	//fs.removeFolder("/");
}

void ls(FileSystem* &fs, std::string cmdArr[], int nrOfCommands)
{
	if (fs == nullptr)
	{
		errFS(cmdArr);
		return;
	}
	std::cout << "Listing directory" << std::endl;
	std::string temp = "";
	if (nrOfCommands > 1) temp = cmdArr[1];
	std::cout << fs->displayChildren(temp) << std::endl;
}

void create(FileSystem* & fs, std::string cmdArr[])
{
	if (fs == nullptr)
	{
		errFS(cmdArr);
		return;
	}
	std::cout << "Enter data: ";
	std::string data = "";
	getline(std::cin, data);
	if (!fs->create(fs->nameToPath(cmdArr[1]), data))
	{
		errSyntax(cmdArr);
	}
}

void cat(FileSystem* & fs, std::string cmdArr[])
{
	if (fs == nullptr)
	{
		errFS(cmdArr);
		return;
	}
	std::cout << fs->getblockString(fs->nameToPath(cmdArr[1])) << std::endl;
}

void rm(FileSystem* & fs, std::string cmdArr[])
{
	if (fs == nullptr)
	{
		errFS(cmdArr);
		return;
	}
	fs->removeFile(fs->nameToPath(cmdArr[1]));
}

void cp(FileSystem *& fs, std::string cmdArr[])
{
	if (fs == nullptr)
	{
		errFS(cmdArr);
		return;
	}
	fs->cp(fs->nameToPath(cmdArr[1]), fs->nameToPath(cmdArr[2]));
}

void append(FileSystem *& fs, std::string cmdArr[])
{
	if (fs == nullptr)
	{
		errFS(cmdArr);
		return;
	}
	fs->append(fs->nameToPath(cmdArr[1]), fs->nameToPath(cmdArr[2]));
}

void mv(FileSystem* & fs, std::string cmdArr[])
{
	if (fs == nullptr)
	{
		errFS(cmdArr);
		return;
	}
	fs->move(fs->nameToPath(cmdArr[1]), fs->nameToPath(cmdArr[2]));
}

void mkdir(FileSystem* & fs, std::string cmdArr[])
{
	if (fs == nullptr)
	{
		errFS(cmdArr);
		return;
	}
	fs->createFolder(fs->nameToPath(cmdArr[1]));
}

void cd(FileSystem* & fs, std::string cmdArr[], std::string &currentDir)
{
	if (fs == nullptr)
	{
		errFS(cmdArr);
		return;
	}
	if (!fs->cd(fs->nameToPath(cmdArr[1]), currentDir))
		std::cout << "The path do not exists in the filesystem" << std::endl;
}

void pwd(FileSystem* & fs, std::string cmdArr[])
{
	if (fs == nullptr)
	{
		errFS(cmdArr);
		return;
	}
	std::cout << fs->getCurrentFilePath() << std::endl;
}
