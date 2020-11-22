#include <iostream>
#include <fstream>
#include <filesystem>

#include <md5.h>
#include <sha1.h>
#include <sha2_256.h>
#include <sha2_512.h>
#include <unordered_map>
#include <unordered_set>
#include "argparse.h"

typedef uint8_t Byte;

void PrintUsage(char** argv)
{
	std::cout << "USAGE:\n"
		<< "  " << argv[0] << " -in <dir> -out <dir> [-report]\n\n";
}

size_t FileData(std::filesystem::path p, Byte*& buf)
{
	std::streampos size;
	std::ifstream file(p, std::ios::in | std::ios::binary | std::ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		buf = new Byte[size];
		file.seekg(0, std::ios::beg);
		file.read(reinterpret_cast<char*>(buf), size);
		file.close();
	}
	return size;
}

std::string GenerateHash(std::filesystem::path p, std::string hashType)
{
	std::string returnString;
	Byte* buf = nullptr;
	size_t sz = FileData(p, buf);
	if (hashType.compare("md5") == 0) {
		Chocobo1::MD5 md5;
		md5.addData(buf, sz);
		md5.finalize();
		returnString = md5.toString();
	}
	else if (hashType.compare("sha1") == 0) {
		Chocobo1::SHA1 sha1;
		sha1.addData(buf, sz);
		sha1.finalize();
		returnString = sha1.toString();
	}
	else if (hashType.compare("sha256") == 0) {
		Chocobo1::SHA2_256 sha256;
		sha256.addData(buf, sz);
		sha256.finalize();
		returnString = sha256.toString();
	}
	else if (hashType.compare("sha512") == 0) {
		Chocobo1::SHA2_512 sha512;
		sha512.addData(buf, sz);
		sha512.finalize();
		returnString = sha512.toString();
	}
	else {
		delete[] buf;
		std::cout << "Hash type not supported!\n\n";
		throw std::runtime_error("Hash type not supported!");
	}
	delete[] buf;
	return returnString;
}



int main(int argc, char* argv[])
{
	ArgParser parser = ArgParser();
	parser.AddArg("-in", "Directory to search for duplicates.", true);
	parser.AddArg("-out", "Directory where to place duplicates.", true);
	parser.AddSwitch("-r", "Report switch will only report and not move any files.");
	Args args;
	try {
		args = parser.ParseArgs(argc, argv);
	}
	catch (std::invalid_argument ex) {
		PrintUsage(argv);
		std::cout << parser.GetHelpString() << std::endl;
		return 1;
	}

	std::unordered_set<std::string> seenFiles;

	std::filesystem::path rootPath = args["-in"];
	std::filesystem::path outPath = args["-out"];
	std::filesystem::path reportPath = outPath;
	reportPath.append("DeDupe_report.txt");

	std::ofstream reportOut(reportPath.string(), std::ios::out | std::ios::binary);
	if (std::filesystem::exists(rootPath)) {
		if (std::filesystem::is_directory(rootPath)) {
			for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(rootPath)) {
				if (!dirEntry.is_directory()) {
					std::string fileHash = GenerateHash(dirEntry.path(), "sha1");
					auto it = seenFiles.find(fileHash);
					if (it == seenFiles.end()) {
						seenFiles.insert(fileHash);
					}
					else {
						auto newfilepath = outPath;
						newfilepath.append(dirEntry.path().filename().string());
						reportOut << dirEntry.path().string() << " --> " << newfilepath.string() << "\n";
						if (args["-r"].empty()) {
							try {
								int n = 0;
								std::string stemname = newfilepath.stem().string();
								while (std::filesystem::exists(newfilepath)) {
									std::stringstream ss;
									ss << "_" << n++;
									newfilepath = newfilepath.replace_filename(stemname + ss.str() + newfilepath.extension().string());
								}

								std::filesystem::rename(dirEntry.path(), newfilepath);
								std::cout << dirEntry.path().string() << " --> " << newfilepath.string() << "\n";
							}
							catch (std::filesystem::filesystem_error& e) {
								std::cout << e.what() << '\n';
							}
						}
					}
				}
			}
		}
		else {
			std::cout << GenerateHash(rootPath, "sha1") << " " << rootPath << "\n";
		}
	}
	else {
		std::cout << "Path " << rootPath << " does not exist.\n\n";
		return 1;
	}

	return 0;
}
