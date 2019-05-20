#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <experimental/filesystem>

#include <wtypes.h>
#include <winnt.h>

int check_PEHeader(const std::experimental::filesystem::path& dllfile) {
	FILE* file = fopen(dllfile.string().c_str(), "rb");
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	if (size < sizeof(IMAGE_DOS_HEADER)) return -1;
	fseek(file, 0, SEEK_SET);
	IMAGE_DOS_HEADER idh;
	fread(&idh, 1, sizeof(IMAGE_DOS_HEADER), file);
	if (idh.e_magic != 0x5A4D) return -1;
	if (size < idh.e_lfanew + sizeof(IMAGE_NT_HEADERS)) return -1;
	fseek(file, idh.e_lfanew, SEEK_SET);
	IMAGE_NT_HEADERS inh;
	fread(&inh, 1, sizeof(IMAGE_NT_HEADERS), file);
	if (inh.Signature != 0x4550) return -1;
	switch (inh.FileHeader.Machine) {
	case 0x8664:
		return 1;
	case 0x014C:
		return 0;
	default:
		return -1;
	}
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cout << "libgen4dll\nA simple tool for making lib files from MSVC dlls\n\nUsage:\nlibgen4dll <dll-name>\n\n";
		system("pause");
		return 0;
	}
	for (int i = 1; i < argc; ++i) {
		int machine = -1;
		std::experimental::filesystem::path dllfile = argv[i];
		switch (check_PEHeader(dllfile)) {
		case 0:
			machine = 0;
		case 1:
			machine = 1;
		case -1:
			std::cerr << "Invalid file\n" << dllfile << "\n";
			continue;
		}
		std::experimental::filesystem::path exportfile = dllfile;
		exportfile.replace_extension(".exports");
		std::string command = "dumpbin -exports \"" + dllfile.string() + "\" > \"" + exportfile.string()+"\"";
		std::cout << "Executing:\n" << command << '\n' << std::endl;
		if (system(command.c_str())) {
			switch (errno) {
			case E2BIG:
				std::cerr << "The argument list(which is system - dependent) is too big.\n" << std::endl;
				break;
			case ENOENT:
				std::cerr << "The command interpreter cannot be found.\n" << std::endl;
				break;
			case ENOEXEC:
				std::cerr << "The command-interpreter file cannot be executed because the format is not valid.\n" << std::endl;
				break;
			case ENOMEM:
				std::cerr << "Not enough memory is available to execute command; or available memory has been corrupted; or a non-valid block exists, which indicates that the process that's making the call was not allocated correctly.\n" << std::endl;
				break;
			default:
				std::cerr << "Unknown errror executing \"" << command << "\"\n" << std::endl;
			}
		}
		std::experimental::filesystem::path outfile = dllfile;
		outfile.replace_extension(".def");
		std::cout << "Creating module definition file\n" << outfile << '\n' << std::endl;
		std::ifstream ifs;
		std::ofstream ofs;
		ifs.open(exportfile);
		ofs.open(outfile);
		ofs << "EXPORTS\n\n";
		bool found = false;
		std::string s;
		while (!found) {
			s.clear();
			getline(ifs, s);
			std::istringstream iss(s);
			std::string s2;
			iss >> s2;
			if (s2 == "ordinal") found = true;
		}
		getline(ifs, s);
		while (ifs) {
			getline(ifs, s);
			std::istringstream iss(s);
			int ordinal;
			if (!(iss >> ordinal)) break;
			std::string s2;
			iss >> s2;
			iss >> s2;
			s2.clear();
			iss >> s2;
			ofs << '\t' << s2 << "\t@" << ordinal << '\n';
		}
		ifs.close();
		ofs.close();
		std::experimental::filesystem::remove(exportfile);
		std::experimental::filesystem::path libfile = outfile;
		libfile.replace_extension(".lib");
		command = std::string("lib /def:\"") + outfile.string() + "\" /out:\"" + libfile.string() + "\" /machine:";
		switch (machine) {
		case 0:
			command += "x86";
		case 1:
			command += "x64";
		}
		command += " > NUL";
		std::cout << "Executing:\n" << command << '\n' << std::endl;
		if (system(command.c_str())) {
			switch (errno) {
			case E2BIG:
				std::cerr << "The argument list(which is system - dependent) is too big.\n" << std::endl;
				break;
			case ENOENT:
				std::cerr << "The command interpreter cannot be found.\n" << std::endl;
				break;
			case ENOEXEC:
				std::cerr << "The command-interpreter file cannot be executed because the format is not valid.\n" << std::endl;
				break;
			case ENOMEM:
				std::cerr << "Not enough memory is available to execute command; or available memory has been corrupted; or a non-valid block exists, which indicates that the process that's making the call was not allocated correctly.\n" << std::endl;
				break;
			default:
				std::cerr << "Unknown errror executing \"" << command << "\"\n" << std::endl;
			}
		}
		std::cout << "Done generating lib file for " << dllfile.string() << '\n' << std::endl;
	}
	return 0;
}
