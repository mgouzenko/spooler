#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <errno.h>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <cstdio>
#include <unistd.h>
#include "spool_controller.hpp"

spool_controller::spool_controller()
    : info(SPOOL_DIR + "/" + SPOOL_INFO) {
  current_uid = getuid();
  euid = geteuid();

  // Check if the spool directory exists
  std::ifstream spool_dir(SPOOL_DIR);

  // If not, create it
  if (!spool_dir){
	if(errno == ENOENT) mkdir(SPOOL_DIR.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
	else{
		perror("Error");	
		exit(1);
	}
}
  else
    spool_dir.close();
  seteuid(current_uid);

  // Check permissions
  struct stat statbuf;
  if (stat(SPOOL_DIR.c_str(), &statbuf) == -1) {
    std::cout << "Error getting permissions for the spool directory\n";
    exit(1);
  } else if ((statbuf.st_mode & S_IFDIR) == 0) {
    std::cout << "Spool directory is actually a file.";
    exit(1);
  } else if ((statbuf.st_mode & S_IRUSR) == 0) {
    std::cout << "Spool directory is not readable.";
    exit(1);
  } else if ((statbuf.st_mode & S_IWUSR) == 0) {
    std::cout << "Spool directory is not writable.";
    exit(1);
  } else if ((statbuf.st_mode & S_IRWXO & S_IRWXG) != 0) {
    std::cout << "Spool directory has incorrect permissions";
    exit(1);
  }
}

void spool_controller::add_files(std::vector<std::string> files) {
  for (auto fname : files) {
    std::ifstream file(fname);
    if (!file){
	perror(("Error for file \"" + fname + "\"").c_str());
	continue;
    }

    seteuid(euid);
	// Get the destination file name from the spool_info object
    auto destination_filename = info.add_file(fname, current_uid);
    std::ofstream outfile(SPOOL_DIR + "/" + destination_filename);
    seteuid(current_uid);

	// Do the transfer
    outfile << file.rdbuf();
    outfile.close();
    file.close();
  }
}

void spool_controller::rm_files(std::vector<std::string> ids) {
  for (auto id : ids) {
	// Remove the file associated with the id from the spool info, and get its name
    auto fname = info.rm_file(id, current_uid);

	// If such a file exists, remove it from the spool directory.
    if (!fname.empty()){
      seteuid(euid);
      remove((SPOOL_DIR + "/" + fname).c_str());
      seteuid(current_uid);
    } else {
      std::cout << "Could not delete: " <<  id << std::endl;
    }
  }
}

void spool_controller::ls_files() {
  seteuid(euid);
  std::cout << info.ls_files();
  seteuid(current_uid);
}
