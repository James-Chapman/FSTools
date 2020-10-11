#include <iostream>
#include <fstream>
#include <filesystem>

#include <md5.h>
#include <sha1.h>
#include <sha2_256.h>
#include <sha2_512.h>

typedef uint8_t Byte;

void PrintUsage(char ** argv)
{
	std::cout << "USAGE:\n"
		<< "  " << argv[0] << " <hashType> <path>\n\n";
	std::cout << "  hashType - Options are: md5, sha1, sha256, sha512. MD5 will be generated for anything not supported.\n"
		<< "  path     - Path to a file or dir\n\n";
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
		file.read(reinterpret_cast<char *>(buf), size);
		file.close();
	}
	return size;
}

std::string GenerateHash(std::filesystem::path p, std::string hashType)
{
	std::string returnString;
	Byte* buf = nullptr;
	size_t sz = FileData(p, buf);
	if (hashType.compare("sha1") == 0) {
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
	else if (hashType.compare("sha512")) {
		Chocobo1::SHA2_512 sha512;
		sha512.addData(buf, sz);
		sha512.finalize();
		returnString = sha512.toString();
	}
	else {
		Chocobo1::MD5 md5;
		md5.addData(buf, sz);
		md5.finalize();
		returnString = md5.toString();
	}
	delete[] buf;
	return returnString;
}

int main(int argc, char ** argv)
{
	if (argc < 3) {
		PrintUsage(argv);
	}

	std::string hashType = argv[1];
	std::string hashTypeLower;
	hashTypeLower.resize(hashType.size());
	std::transform(hashType.begin(), hashType.end(), hashTypeLower.begin(), ::tolower);

	std::filesystem::path rootPath = argv[2];
	if (std::filesystem::exists(rootPath)) {
		if (std::filesystem::is_directory(rootPath)) {
			for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(rootPath)) {
				std::string md5hash = GenerateHash(dirEntry.path(), hashTypeLower);
				std::cout << md5hash << " " << dirEntry.path() << "\n";
			}
		}
		else {
			std::cout << GenerateHash(rootPath, hashTypeLower) << " " << rootPath << "\n";
		}
	}
	else {
		std::cout << "Path " << rootPath << " does not exist.\n\n";
		return 1;
	}

	return 0;
}