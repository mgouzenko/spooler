#include <iostream>
#include <unistd.h>
#include "spool_controller.hpp"

void print_usage(){
	std::cout << "Usage: spooler CMD\n"
			  << "CMD:\n"
			  << "\t" << ADD << " [filenames]\n"
			  << "\t" << RM  << " [filenames]\n"
			  << "\t" << LS  << " [filenames]\n";
}

int main(int argc, char** argv)
{
	if(argc < 2){
		print_usage();
		return 1;
	}
	std::string cmd = argv[1];
	std::vector<std::string> files(argv+2, argv+argc);

	auto print_spooler = spool_controller(getuid());
	if(cmd == ADD) print_spooler.add_files(files);
	else if(cmd == RM) print_spooler.rm_files(files);
	else if(cmd == LS) print_spooler.ls_files();
	else print_usage();
	return 0;
}
