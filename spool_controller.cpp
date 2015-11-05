#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "spool_controller.hpp"

spool_controller::spool_controller(uid_t uid):
info(SPOOL_DIR + "/" + SPOOL_INFO){
	current_uid = uid;

	// Check if the spool directory exists
	std::ifstream spool_dir(SPOOL_DIR);

	// If not, make it
	if(!spool_dir) mkdir(SPOOL_DIR.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
	else spool_dir.close();

	// Check permissions
	struct stat statbuf;
	if(stat(SPOOL_DIR.c_str(), &statbuf) == -1){
		std::cout << "Error getting permissions for the spool directory\n";
		exit(1);
	} else if((statbuf.st_mode & S_IFDIR) == 0){
		std::cout << "Spool directory is actually a file.";
		exit(1);
	} else if((statbuf.st_mode & S_IRUSR) == 0){
		std::cout << "Spool directory is not readable.";
		exit(1);
	} else if((statbuf.st_mode & S_IWUSR) == 0){
		std::cout << "Spool directory is not writable.";
		exit(1);
	}
}

void spool_controller::add_files(std::vector<std::string> files){
	for(auto fname: files){
		std::ifstream file(fname);
		if(!file) continue;
		auto destination_filename = info.add_file(fname, current_uid);
		std::ofstream outfile(SPOOL_DIR + "/" + destination_filename);
		outfile << file.rdbuf();
		outfile.close();
		file.close();
	}
}

void spool_controller::rm_files(std::vector<std::string> ids){
	for(auto id: ids) info.rm_file(id, current_uid);
}

void spool_controller::ls_files(){
	std::cout << info.ls_files();
}
