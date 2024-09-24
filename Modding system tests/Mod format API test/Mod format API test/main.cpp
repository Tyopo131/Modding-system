#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <algorithm>
#include <thread>

namespace formatPoses {
	constexpr uint64_t identifierStartPos = 0;

	constexpr uint64_t identifierSize = 8;
	constexpr uint64_t headerStartPos = 8;

	constexpr uint64_t headerEndPos = 8 + (8 * 4);

	constexpr uint64_t headerSize = (8 * 4);
	constexpr uint64_t dllStartPos = headerEndPos;
}

int main(int argc, char* argv[]) {

	// choice

	using namespace std::chrono_literals;
	std::cerr << "Enter 1 for write mode or 2 for read mode.\n";
	char choice;
	std::cin >> choice;
	if (choice == '1') {

		// write


		// Early error handling + setup


		namespace fs = std::filesystem;
		if (argc != 4) {
			std::cerr << "Invalid command-line arguments.\nUsage: [app].exe [dll] [json] [mod name]";
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
		
		size_t fileSize = fs::file_size(dllPath);

		char* dllData = new char[fileSize];
		readFile.read(dllData, fileSize);
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
		std::string temp = jsonData.str();
		temp.pop_back();
		jsonData.str(temp);
		readFile.close();

		std::cout << jsonData.str();

		//____________________________________________


		// Part three: Creating the mod file

		// Part 3.1: Calculate sizes and positions

		using namespace formatPoses;

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
		outputFile.seekp(jsonStartPos);
		outputFile.write(jsonData.str().c_str(), jsonData.str().size());

	}

	else if (choice == '2') {

		namespace fs = std::filesystem;

		// read

		// Early error handling + setup

		if (argc != 3) {
			std::cerr << "Invalid command-line arguments.\nUsage: [app].exe [mod] [folder-name]";
			std::cin.get();
			return 1;
		}

		fs::path folderPath = argv[2];

		fs::path modPath(argv[1]);

		if (!fs::exists(modPath)) {
			std::cerr << "Mod file (specified in argument 1) doesn't exist.";
			std::cin.get();
			return 2;
		}

		if (fs::exists(folderPath)) {

			std::cerr << "Folder already exists. Would you like to add files to it anyways? (Y/N)\n";
			char choice;
			std::cin >> choice;
			if (choice == 'Y' || choice == 'y');
			else if (choice == 'N' || choice == 'n') return -1;


			else {
				using namespace std::chrono_literals;
				std::cerr << "Dude, you need to either enter Y (yes) or N (no). I thought you already knew this.\nThis window will close after 9 seconds";
				std::this_thread::sleep_for(9s);
				return -2;
			}

		}


		std::ifstream readFile(modPath, std::ios::binary);
		size_t modSize = fs::file_size(modPath);

		// Part 1: Extracting the files

		// Part 1.1: Verifying the mod format is correct

		char start[9];

		readFile.read(start, 8);
		start[8] = '\0';
		if (strcmp(start, "msys-mod")) {

			std::cout << "The mod file is invalid. (Verification token at start not found)\nThe mod creator probably modified their mod in a way that broke it.";
			std::cin.get();
			return -10;

		}
		// Part 1.2: Reading the mod data:

		int64_t header[4];
		readFile.read(reinterpret_cast<char*>(header), 8 * 4);
		std::unique_ptr<char> modData(new char[header[1]]);
		std::unique_ptr<char> jsonData(new char[header[3]]);
		readFile.read(modData.get(), header[1]);
		readFile.read(jsonData.get(), header[3]);

		// Part 2: Creating the files

		fs::create_directory(folderPath);
		std::ofstream dllWrite(folderPath.string() + "/dll.dll", std::ios::binary);
		std::ofstream jsonWrite(folderPath.string() + "/json.json", std::ios::binary);

		dllWrite.write(modData.get(), header[1]); jsonWrite.write(jsonData.get(), header[3]);

	}

}