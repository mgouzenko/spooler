#include <iostream>
#include "spool_controller.hpp"

spool_controller::spool_controller(uid_t uid):
spool_dir(SPOOL_DIR),
info(SPOOL_INFO){
	current_uid = uid;
}

void spool_controller::add_files(std::vector<std::string> files){
	for(auto file: files) info.add_file(file, current_uid);
}

void spool_controller::rm_files(std::vector<std::string> ids){
	for(auto id: ids) info.rm_file(id, current_uid);
}

void spool_controller::ls_files(){
	std::cout << info.ls_files();
}
