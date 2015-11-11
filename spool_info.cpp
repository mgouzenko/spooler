#include "spool_info.hpp"
#include "spool_controller.hpp"
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

spool_info::file::file(std::string filename, uid_t uid) {
  name = filename;
  owner = uid;
}

std::string spool_info::ls_files(std::string spool_dir) {
  std::ostringstream out;

  std::multimap<time_t, std::string> info_map;

  for (auto f : files) {
    std::ostringstream info_line;
    // Get the file's absolute path
    auto file_path = spool_dir + "/" + f.second.name;
    struct stat buf;

    // Do a stat on the file to get its creation timestamp
    if (stat(file_path.c_str(), &buf) == -1) {
      std::cout << "Stat failed\n";
      exit(1);
    }
    time_t creation_secs = buf.st_ctime;
    struct tm ts = *localtime(&creation_secs);

    // Output the relevant info about the file
    info_line << f.second.name << " " << f.second.owner << " "
              << ts.tm_year + 1900 << "-" << ts.tm_mon + 1 << "-" << ts.tm_mday
              << "_" << ts.tm_hour << ":" << ts.tm_min << ":" << ts.tm_sec
              << " " << f.first << std::endl;

    info_map.insert(std::make_pair(creation_secs, info_line.str()));
  }

  for (auto line : info_map)
    out << line.second;
  return out.str();
}

std::pair<std::string, std::string> spool_info::add_file(std::string filename,
                                                         uid_t uid) {
  int id = ++num_files;
  if (!free_ids.empty()) {
    // If there's an id in the pool of free ids, use it.
    id = free_ids.back();
    free_ids.pop_back();
  }

  // Make a unique filename based on its id. Appending the id is fine.
  // That's because since ids are unique, two file names will never
  // end in the same character.

  if (filename == "/" || filename.empty()) {
    std::cout << filename << ": X Bad filename" << std::endl;
    return std::make_pair("", "");
  } else if (filename.back() == '/')
    filename.erase(filename.end() - 1);

  auto idx = filename.find_last_of('/');
  if (idx == std::string::npos)
    idx = 0;
  else
    idx++;
  auto unique_filename = filename.substr(idx) + "_" + std::to_string(id);
  files.insert(std::make_pair(std::to_string(id),
                              spool_info::file(unique_filename, uid)));
  return std::make_pair(std::to_string(id), unique_filename);
}

std::string spool_info::rm_file(std::string id, uid_t uid) {
  auto f = files.find(id);
  if (f == files.end()) {
    errno = ENOENT;
    return "";
  }

  // If the file exists and the uid matches, get rid of it and return its name
  if (f->second.owner == uid) {
    auto fname = f->second.name;
    files.erase(f);
    free_ids.push_back(std::stoi(id));
    num_files--;
    return fname;
  } else {
    errno = EACCES;
    return "";
  }
}

spool_info::spool_info(std::string spool_info_file) {
  info_file = spool_info_file;
  std::ifstream infile(spool_info_file);

  if (infile) {
    std::string line;

    // The first line is the number of entries
    std::getline(infile, line);
    num_files = stoi(line);

    // The second line is a tab separated list of free ids
    std::getline(infile, line);
    std::istringstream available_ids(line);

    while (std::getline(available_ids, line, '\t'))
      free_ids.push_back(std::stoi(line));

    // The remaining lines are file entries.
    std::string next;
    while (std::getline(infile, line)) {
      std::istringstream line_stream(line);

      std::string name;
      std::getline(line_stream, name, '\t');

      std::string identifier;
      std::getline(line_stream, identifier, '\t');

      std::string owner;
      std::getline(line_stream, owner, '\t');
      uid_t uid = std::stoull(owner);

      files.insert(std::make_pair(identifier, spool_info::file(name, uid)));
    }
  } else if (errno == ENOENT) {
    num_files = 0;
  } else {
    perror("Error");
    exit(1);
  }
}

spool_info::~spool_info() {
  // When the spool_info object is destroyed, write its contents to
  // the spool info file
  std::ofstream outfile(info_file);

  // Output the number of files as the first line
  outfile << std::to_string(num_files) << std::endl;

  // Output the free ids as the second line
  for (auto id : free_ids)
    outfile << id << "\t";
  outfile << std::endl;

  for (auto file : files)
    outfile << file.second.name << "\t" << file.first << "\t"
            << file.second.owner << std::endl;
}
