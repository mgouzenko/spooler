#ifndef __SPOOL_INFO_HPP__
#define __SPOOL_INFO_HPP__
#include <ctime>
#include <unistd.h>
#include <string>
#include <vector>
#include <unordered_map>

class spool_info {
private:
  /* Inner class, used to represent a particular file */
  struct file {
    /* The name of the file in the spool directory */
    std::string name;

    /* The owner (the userid who made the request) */
    uid_t owner;

    /* Constructor */
    file(std::string filename, uid_t uid);
  };

  /* A hashmap of id -> file */
  std::unordered_map<std::string, file> files;

  /* The absolute path to the info file (ie spool_info.txt) */
  std::string info_file;

  /* A vector of ids corresponding to files that were removed */
  std::vector<int> free_ids;

  /* The number of files currently in the spool */
  int num_files;

public:
  /* Constructor
   * Args:
   * 	spool_info_file: The absolute path to the spool info file
   */
  spool_info(std::string spool_info_file);

  /* Destructor */
  ~spool_info();

  /* Adds a file
   * Args:
   * 	filename: The file's ORIGINAL name.
   *    uid: The uid of the user adding the file.
   *
   * Returns: The new name of the file. This is the name of
   * 	the file inside the spool directory.
   */
  std::pair<std::string, std::string> add_file(std::string filename, uid_t uid);

  /* Removes a file
   * Args:
   * 	id: The id associated with the file to be removed
   *	uid: The user id of the user who issued the request to remove the file
   *
   * Returns: The filename of the removed file.
   */
  std::string rm_file(std::string id, uid_t uid);

  /* Lists all of the files, along with their timestamps and ids
   * Args:
   * 	spool_dir: The spool directory where the files are located. */
  std::string ls_files(std::string spool_dir);
};

#endif
