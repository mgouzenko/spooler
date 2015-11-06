#ifndef __SPOOLER_HPP__
#define __SPOOLER_HPP__
#include <string>
#include <unistd.h>
#include <vector>
#include "spool_info.hpp"

// Static globals
static auto ADD = "addqueue";
static auto RM = "rmqueue";
static auto LS = "showqueue";
static std::string SPOOL_DIR = "/home/tinyvm/spool_dir";
static std::string SPOOL_INFO = "spool_info.txt";

class spool_controller {
  /* The uid of the user who ran the spooler */
  uid_t current_uid;

  /* The initial effective user id */
  uid_t euid;

  /* Metadata about when the file was added to the spooler. */
  spool_info info;

public:
  /* Default constructor */
  spool_controller();

  /* Adds files to the spool
   *
   * Args:
   * 	files: a vector of strings, each representing a file to add
   */
  void add_files(std::vector<std::string> files);

  /* Removes files from the spool
   *
   * Args:
   * 	files: a vector of strings, each representing a file to remove
   */
  void rm_files(std::vector<std::string> files);

  /* Lists all the files currently in the spool */
  void ls_files();
};

#endif
