#include <iostream>
#include <fstream>
#include <filesystem>
#include <regex>



typedef uint8_t Byte;

void PrintUsage(char ** argv)
{
	std::cout << "USAGE:\n"
		<< "  " << argv[0] << " <dir> <name>\n\n";
	std::cout << "  dir    - Dir to search.\n"
		<< "  name   - Name to find\n\n";
}


int main(int argc, char** argv)
{
	if (argc < 3) {
		PrintUsage(argv);
	}

	std::string searchDir = argv[1];
	std::string nameArg = argv[2];
	std::string regexString = "";
	
	if (nameArg.front() == '*') {
		nameArg.erase(0, 1);
		regexString += "(.*)";
	}

	if (nameArg.back() == '*') {
		size_t len = strlen(nameArg.c_str());
		nameArg.erase((len - 1), 1);
		regexString += "(" + nameArg + ")";
		regexString += "(.*)";
	}
	else {
		regexString += "(" + nameArg + ")";
	}

	std::regex rgx(regexString);

	std::filesystem::path searchPath = searchDir;
	if (std::filesystem::exists(searchPath)) {
		if (std::filesystem::is_directory(searchPath)) {
			for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(searchPath, std::filesystem::directory_options::skip_permission_denied)) {
				std::string filename = dirEntry.path().filename().string();
				std::string filenameLower;
				filenameLower.resize(filename.size());
				std::transform(filename.begin(), filename.end(), filenameLower.begin(), ::tolower);
				if (std::regex_match(filenameLower, rgx)) {
					std::cout << dirEntry.path().string() << std::endl;
				}
			}
		}
	}
	else {
		std::cout << searchDir << " not found!" << std::endl;
		return 1;
	}

	return 0;
}