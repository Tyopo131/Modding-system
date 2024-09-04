#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <algorithm>
#include <thread>


int main(int argc, char* argv[]) {

	// Early error handling + setup


	namespace fs = std::filesystem;
	if (argc != 4) {
		std::cerr << "Invalid command-line arguments.\nUsage: [app].exe [dll]";
		std::cin.get();
		return 1;
	}

	fs::path dllPath = argv[1];
	if (!fs::exists(dllPath)) {
		std::cerr << "DLL file (specified in argument 1) doesn't exist.";
		std::cin.get();
		return 2;
	}
	

	fs::path jsonPath = argv[2];
	if (!fs::exists(jsonPath))
	{
		std::cerr << "JSON file (specified in argument 2) doesn't exist.";
		std::cin.get();
		return 3;
	}
	std::string modFilename = std::string(argv[3]) + ".msys";
	if (fs::exists(std::string(argv[3]) + ".msys")) {
		using namespace std::chrono_literals;
		std::cerr << "File already exists. Would you like to delete its contents and overwrite it? (Y/N)\n";
		char choice;
		std::cin >> choice;
		if (choice == 'Y' || choice == 'y');
		else if (choice == 'N' || choice == 'n') return -1;
		else {
			std::cerr << "Dude, you need to either enter Y (yes) or N (no). I thought you already knew this.\nThis window will close after 9 seconds";
			std::this_thread::sleep_for(9s);
			return -2;
		}
	}

	// Part one: Grabbing the DLL data
	// ___________________________________________
	

	std::ifstream readFile(dllPath, std::ios::binary);
	readFile.setstate(std::ios::goodbit);
	size_t fileSize = fs::file_size(dllPath);
	
	char* dllData = new char[fileSize + 1];
	readFile.read(dllData, fileSize);
	dllData[fileSize] = '\0';
	std::cout << dllData << '\n';

	readFile.close();
	// ___________________________________________

	// Part two: Grabbing the JSON data

	readFile.open(jsonPath);
	
	std::stringstream jsonData;
	
	readFile.seekg(0);
	readFile.setstate(std::ios::goodbit);
	std::string line;
	while (std::getline(readFile, line)) {
		jsonData << line << '\n';
	}
	
	readFile.close();
	
	std::cout << jsonData.str();

	//____________________________________________
	

	// Part three: Creating the mod file

	// Part 3.1: Calculate sizes and positions
	constexpr uint64_t identifierStartPos = 0;

	constexpr uint64_t identifierSize = 8;
	constexpr uint64_t headerStartPos = 8;

	constexpr uint64_t headerEndPos = 8 + (8 * 4);

	constexpr uint64_t headerSize = (8 * 4);
	constexpr uint64_t dllStartPos = headerEndPos;

	const uint64_t jsonStartPos = headerEndPos + fileSize;
	const uint64_t jsonSize = jsonData.str().size();
	
	
	// ___________________________________________

	std::ofstream outputFile(argv[3], std::ios::binary);
	outputFile.write("msys-mod", identifierSize);
	outputFile.write(reinterpret_cast<const char*>(&dllStartPos), 8);
	outputFile.write(reinterpret_cast<const char*>(&fileSize), 8);
	outputFile.write(reinterpret_cast<const char*>(&jsonStartPos), 8);
	outputFile.write(reinterpret_cast<const char*>(&jsonSize), 8);
	outputFile.write(dllData, fileSize);
	outputFile.write(jsonData.str().c_str(), jsonData.str().size());
	
}


// Stored here for safekeeping: modfile.write(reinterpret_cast<const char*>(&data), 4);