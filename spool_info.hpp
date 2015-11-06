#ifndef __SPOOL_INFO_HPP__
#define __SPOOL_INFO_HPP__
#include <ctime>
#include <unistd.h>
#include <string>
#include <vector>
#include <unordered_map>

class spool_info {
private:
  class file {
  public:
    // some arbitrary name (*not* the original file name,
    // but a number is fine)
    std::string name;

    // the owner (the userid who made the request)
    uid_t owner;

    file(std::string filename, uid_t uid);
  };

  std::unordered_map<std::string, file> files;
  std::string info_file;
  std::vector<int> free_ids;
  int num_files;

public:
  spool_info(std::string spool_info_file);
  ~spool_info();
  std::string add_file(std::string filename, uid_t uid);
  std::string rm_file(std::string id, uid_t uid);
  std::string ls_files();
};

#endif
