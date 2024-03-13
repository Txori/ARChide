/*
    ARChide, by Etienne Bégué - www.txori.com
    Easily retrieve and gather the games you have hidden in ARC Browser!

    Compile using w64devkit (https://github.com/skeeto/w64devkit):
    g++ -O3 -std=c++17 -o ARChide.exe ARChide.cpp -lstdc++fs
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#endif

// Version of the program
const std::string VERSION = "0.2.0";

// Files and folder names
const std::string LIST_FILE = "games.csv";
const std::string CONFIG_FILE = "config.txt";
const std::string DELETE_FOLDER = "_delete";

// Definitions for text colors
const char* RESET_COLOR = "\033[0m";
const char* RED_COLOR = "\033[31m";
const char* GREEN_COLOR = "\033[32m";
const char* YELLOW_COLOR = "\033[33m";

namespace fs = std::filesystem;

// Struct that follows the ARC Browser database naming convention
struct GameInfo {
    std::string system;
    std::string filename;
    std::string name;
    std::string scraperDatabase;
    std::string scraperDatabaseID;
    std::string confidence;
    std::string hidden;
    std::string favorite;
};

GameInfo extractGameInfo(const std::string& row) {
    GameInfo gameInfo;
    std::istringstream rowStream(row);
    std::getline(rowStream, gameInfo.system, ',');
    std::getline(rowStream, gameInfo.filename, ',');
    std::getline(rowStream, gameInfo.name, ',');
    std::getline(rowStream, gameInfo.scraperDatabase, ',');
    std::getline(rowStream, gameInfo.scraperDatabaseID, ',');
    std::getline(rowStream, gameInfo.confidence, ',');
    std::getline(rowStream, gameInfo.hidden, ',');
    std::getline(rowStream, gameInfo.favorite, ',');
    return gameInfo;
}


// Function to enable ANSI colors mode for Windows terminal (ex: conhost.exe)
void setupConsoleMode() {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    
    // Get the current console mode
    GetConsoleMode(hConsole, &mode);

    // Enable ANSI escape codes in the console mode
    SetConsoleMode(hConsole, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}


// Function to print the program header
void printHeader(const std::string& version) {
    std::cout << "   _   ___  ___ _    _    _" << std::endl;
    std::cout << "  /_\\ | _ \\/ __| |_ (_)__| |___" << std::endl;
    std::cout << " / _ \\|   / (__| ' \\| / _` / -_)" << std::endl;
    std::cout << "/_/ \\_\\_|_\\\\___|_||_|_\\__,_\\___|" << std::endl;
    std::cout << "                           " << version << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
}


// Function to display an exit message and wait for user input
void exitMessage() {
    std::cout << std::endl;
    std::cout << "Press any key to exit" << std::endl;
#ifdef _WIN32
    system("pause >nul");
#else
    system("read -n 1 -s -r -p \"\"");
#endif
    std::exit(EXIT_FAILURE);
}


// Function to check if necessary files LIST_FILE and CONFIG_FILE exists
void checkFilesExist(const std::string& listFile, const std::string& configFile) {
    if (!fs::exists(listFile) || !fs::exists(configFile)) {
        std::cerr << RED_COLOR << "Error: " << (fs::exists(listFile) ? configFile : listFile) << " not found" << std::endl;
        exitMessage();
    }
}


// Function to read the configuration file CONFIG_FILE into a map
std::map<std::string, std::string> readConfigFile(const std::string& configFile) {
    std::map<std::string, std::string> systems;
    std::ifstream configStream(configFile);
    std::string line;
    while (std::getline(configStream, line)) {
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            systems[key] = value;
        }
    }

    // Print contents of the 'systems' map
    std::cout << "Systems found in " << configFile << ":" << std::endl;
    std::cout << std::endl;
    for (const auto& entry : systems) {
        std::cout << entry.first << " [" << entry.second << "]" << std::endl;
    }
    std::cout << std::endl;

    return systems;
}


// Function to create the delete folder if it doesn't exist
void createDeleteFolder(const std::string& deleteFolder) {
    if (!fs::exists(deleteFolder)) {
        fs::create_directories(deleteFolder);
    }
}


// Function to process the input file - First pass to check missing systems
std::set<std::string> checkMissingSystems(const std::string& listFile, const std::map<std::string, std::string>& systems) {
    std::set<std::string> missingSystems;
    std::ifstream listFileStream(listFile, std::ios::in | std::ios::binary);
    if (listFileStream) {
        std::string row;
        std::getline(listFileStream, row); // Skip header
        while (std::getline(listFileStream, row)) {
            GameInfo gameInfo = extractGameInfo(row);
            if (gameInfo.hidden == "HIDDEN" && systems.find(gameInfo.system) == systems.end()) {
                missingSystems.insert(gameInfo.system);
            }
        }
    }
    listFileStream.close();
    return missingSystems;
}


// Function to display missing systems and exit if any
void displayMissingSystems(const std::set<std::string>& missingSystems, const std::string& configFile) {
    if (!missingSystems.empty()) {
        std::cout << std::endl;
        std::cerr << RED_COLOR << "Error: Missing systems in " << configFile << ":" << std::endl;
        for (const auto& missingSystem : missingSystems) {
            std::cerr << " - " << missingSystem << std::endl;
        }
        exitMessage();
    }
}


// Function to move hidden games listed in LIST_FILE to DELETE_FOLDER
void moveHiddenGames(const std::string& listFile, const std::map<std::string, std::string>& systems, const std::string& deleteFolder) {
    int successCount = 0;
    int warningCount = 0;
    int errorCount = 0;

    std::set<std::string> missingSystems = checkMissingSystems(listFile, systems);
    displayMissingSystems(missingSystems, CONFIG_FILE);

    createDeleteFolder(deleteFolder);

    std::cout << std::endl;
    std::cout << "Moving hidden games to " << deleteFolder << " folder:" << std::endl;
    std::cout << std::endl;

    std::ifstream listFileStream(listFile, std::ios::in | std::ios::binary);
    if (listFileStream) {
        std::string row;
        std::getline(listFileStream, row); // Skip header
        while (std::getline(listFileStream, row)) {
            try {
                GameInfo gameInfo = extractGameInfo(row);

                if (gameInfo.hidden == "HIDDEN") {
                    std::string systemFolder = systems.at(gameInfo.system);
                    if (!systemFolder.empty()) {
                        fs::path filePath = fs::path(systemFolder) / gameInfo.filename;
                        fs::path destinationPath = fs::path(deleteFolder) / systemFolder / gameInfo.filename;

                        if (fs::exists(filePath)) {
                            fs::path systemFolderPath = fs::path(deleteFolder) / systemFolder;
                            if (!fs::exists(systemFolderPath)) {
                                fs::create_directories(systemFolderPath);
                            }

                            fs::rename(filePath, destinationPath);
                            std::cout << GREEN_COLOR << "+ " << fs::relative(filePath).string() << std::endl;
                            successCount++;
                        } else {
                            if (fs::exists(destinationPath)) {
                                std::cerr << YELLOW_COLOR << "- " << fs::relative(filePath).string() << std::endl;
                                warningCount++;
                            } else {
                                std::cerr << RED_COLOR << "! " << fs::relative(filePath).string() << std::endl;
                                errorCount++;
                            }
                        }
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << RED_COLOR <<  "Error processing row: " << e.what() << std::endl;
                errorCount++;
            }
        }
    }

    listFileStream.close();

    std::cout << std::endl;
    std::cout << RESET_COLOR << "Results:" << std::endl;
    std::cout << std::endl;
    if (successCount > 0) {
        std::cout << GREEN_COLOR << "+ " << RESET_COLOR << successCount << " games moved successfully" << std::endl;
    }
    if (warningCount > 0) {
        std::cout << YELLOW_COLOR << "- " << RESET_COLOR << warningCount << " games already moved" << std::endl;
    }
    if (errorCount > 0) {
        std::cout << RED_COLOR << "! " << RESET_COLOR << errorCount << " games not found" << std::endl;
    }

    exitMessage();
}


int main() {
    setupConsoleMode();
    printHeader(VERSION);
    checkFilesExist(LIST_FILE, CONFIG_FILE);
    moveHiddenGames(LIST_FILE, readConfigFile(CONFIG_FILE), DELETE_FOLDER);
    return 0;
}
