#ifndef __SPOOLER_HPP__
#define __SPOOLER_HPP__
#include <string>
#include <unistd.h>
#include <vector>
#include "spool_info.hpp"

static auto ADD = "addqueue";
static auto RM = "rmqueue";
static auto LS = "showqueue";
static std::string SPOOL_DIR = "/Users/mgouzenko/Desktop/spool_dir";
static std::string SPOOL_INFO = "spool_info.txt";

class spool_controller{
	uid_t current_uid;
	spool_info info;

	public:
	spool_controller(uid_t uid);
	void add_files(std::vector<std::string> files);
	void rm_files(std::vector<std::string> files);
	void ls_files();
};

#endif
