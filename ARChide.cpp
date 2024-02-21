/*
    ARChide 0.1.0 by Etienne Bégué - www.txori.com
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

const std::string VERSION = "0.1.0";
const std::string INPUT_FILE = "games.csv";
const std::string CONFIG_FILE = "config.txt";
const std::string DELETE_FOLDER = "_delete";

namespace fs = std::filesystem;

void moveHiddenGames(const std::string& inputFile, const std::string& configFile, const std::string& deleteFolder) {
    // Check if necessary files exist
    if (!fs::exists(inputFile) || !fs::exists(configFile)) {
        std::cerr << "Error: " << (fs::exists(inputFile) ? configFile : inputFile) << " not found." << std::endl;
        std::cout << std::endl;
        std::cout << "Press Enter to exit" << std::endl;
        std::cin.get();
        std::exit(EXIT_FAILURE);
    }

    // Read the configuration file into a map
    std::map<std::string, std::string> system;
    std::ifstream configStream(configFile);
    std::string line;
    while (std::getline(configStream, line)) {
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            system[key] = value;
        }
    }

    // Debug output: Print contents of the 'system' map
    std::cout << "Systems found in " << configFile << ":" << std::endl;
    std::cout << std::endl;
    for (const auto& entry : system) {
        std::cout << "- " << entry.first << " [" << entry.second << "]" << std::endl;
    }
    std::cout << std::endl;

    // Create the delete folder if it doesn't exist
    if (!fs::exists(deleteFolder)) {
        fs::create_directories(deleteFolder);
    }

    // Process the input file - First pass to check missing systems
    std::set<std::string> missingSystems;
    std::ifstream inputFileStream(inputFile, std::ios::in | std::ios::binary);
    if (inputFileStream) {
        std::string row;
        // Assuming the first line is a header and we skip it
        std::getline(inputFileStream, row);
        while (std::getline(inputFileStream, row)) {
            std::istringstream rowStream(row);
            std::string systemName, filename, name, scraperDatabase, scraperDatabaseID, confidence, hidden, favorite;
            std::getline(rowStream, systemName, ',');
            std::getline(rowStream, filename, ',');
            std::getline(rowStream, name, ',');
            std::getline(rowStream, scraperDatabase, ',');
            std::getline(rowStream, scraperDatabaseID, ',');
            std::getline(rowStream, confidence, ',');
            std::getline(rowStream, hidden, ',');
            std::getline(rowStream, favorite, ',');

            if (hidden == "HIDDEN") {
                // Check if the system is configured
                if (system.find(systemName) == system.end()) {
                    missingSystems.insert(systemName);
                }
            }
        }
    }
    inputFileStream.close();

    // If missing systems are found, print and stop the script
    if (!missingSystems.empty()) {
        std::cout << std::endl;
        std::cerr << "Error: Missing systems in " << configFile << ":" << std::endl;
        for (const auto& missingSystem : missingSystems) {
            std::cerr << " - " << missingSystem << std::endl;
        }
        std::cout << std::endl;
        std::cout << "Press Enter to exit" << std::endl;
        std::cin.get();
        std::exit(EXIT_FAILURE);
    }

    // Initialize counters for successful moves and errors
    int successfulMoves = 0;
    int errorCount = 0;

    // Second pass to actually move hidden games
    std::cout << std::endl;
    std::cout << "Moving hidden games to " << deleteFolder << " folder:" << std::endl;
    std::cout << std::endl;
    inputFileStream.open(inputFile, std::ios::in | std::ios::binary);

    if (inputFileStream) {
        std::string row;
        // Assuming the first line is a header and we skip it
        std::getline(inputFileStream, row);
        while (std::getline(inputFileStream, row)) {
            try {
                std::istringstream rowStream(row);
                std::string systemName, filename, name, scraperDatabase, scraperDatabaseID, confidence, hidden, favorite;
                std::getline(rowStream, systemName, ',');
                std::getline(rowStream, filename, ',');
                std::getline(rowStream, name, ',');
                std::getline(rowStream, scraperDatabase, ',');
                std::getline(rowStream, scraperDatabaseID, ',');
                std::getline(rowStream, confidence, ',');
                std::getline(rowStream, hidden, ',');
                std::getline(rowStream, favorite, ',');

                if (hidden == "HIDDEN") {
                    // Get the folder name for the system
                    std::string systemFolder = system[systemName];
                    if (!systemFolder.empty()) {
                        // Build the full path to the file
                        fs::path filePath = fs::path(systemFolder) / filename;

                        // Build the destination path in the _hidden directory
                        fs::path destinationPath = fs::path(deleteFolder) / systemFolder / filename;

                        try {
                            // Create the system-specific folder in the delete_folder if it doesn't exist
                            fs::path systemFolderPath = fs::path(deleteFolder) / systemFolder;
                            if (!fs::exists(systemFolderPath)) {
                                fs::create_directories(systemFolderPath);
                            }

                            // Move the file to the destination path
                            fs::rename(filePath, destinationPath);
                            std::cout << "+ " << fs::relative(filePath).string() << std::endl;
                            successfulMoves++;
                        }
                        catch (const std::filesystem::filesystem_error& e) {
                            std::cerr << "! " << fs::relative(filePath).string() << std::endl;
                            errorCount++;
                        }
                    }
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Error processing row: " << e.what() << std::endl;
                errorCount++;
            }
        }
    }

    inputFileStream.close();

    // Display the results
    std::cout << std::endl;
    if (successfulMoves > 0) {
        std::cout << successfulMoves << " games moved successfully (+)" << std::endl;
    }
    if (errorCount > 0) {
        std::cout << errorCount << " errors encountered (!)" << std::endl;
    }
}

int main() {
    std::cout << "   _   ___  ___ _    _    _" << std::endl;
    std::cout << "  /_\\ | _ \\/ __| |_ (_)__| |___" << std::endl;
    std::cout << " / _ \\|   / (__| ' \\| / _` / -_)" << std::endl;
    std::cout << "/_/ \\_\\_|_\\\\___|_||_|_\\__,_\\___|" << std::endl;
    std::cout << "                           " << VERSION << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    moveHiddenGames(INPUT_FILE, CONFIG_FILE, DELETE_FOLDER);
    std::cout << std::endl;
    std::cout << "Press Enter to exit" << std::endl;
    std::cin.get();
    return 0;
}
