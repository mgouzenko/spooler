#include "spool_info.hpp"
#include "spool_controller.hpp"
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <sys/stat.h>

spool_info::file::file(std::string filename, uid_t uid) {
  name = filename;
  owner = uid;
}

std::string spool_info::ls_files() {
  std::ostringstream out;
  out << "Name"
      << "\t"
      << "ID"
      << "\t"
      << "Owner" << std::endl;
  for (auto f : files) {
    auto file_path = SPOOL_DIR + "/" + f.second.name;
    struct stat buf;
    if (stat(file_path.c_str(), &buf) == -1) {
      std::cout << "Stat failed\n";
      exit(1);
    }
    time_t creation_secs = buf.st_ctime;
    struct tm ts = *localtime(&creation_secs);

    out << f.second.name << "\t" << f.first << "\t" << f.second.owner << "\t"
        << ts.tm_mon + 1 << "/" << ts.tm_mday << "/" << ts.tm_year + 1900 << " "
        << ts.tm_hour << ":" << ts.tm_min << ":" << ts.tm_sec << std::endl;
  }

  return out.str();
}

std::string spool_info::add_file(std::string filename, uid_t uid) {
  int id = ++num_files;
  if (!free_ids.empty()) {
    id = free_ids.back();
    free_ids.pop_back();
  }
  auto unique_filename = filename + std::to_string(id);
  files.emplace(std::to_string(id), spool_info::file(unique_filename, uid));
  return unique_filename;
}

std::string spool_info::rm_file(std::string id, uid_t uid) {
  auto f = files.find(id);
  if (f == files.end())
    return "";
  if (f->second.owner == uid) {
    auto fname = f->second.name;
    files.erase(f);
    free_ids.push_back(std::stoi(id));
    num_files--;
    return fname;
  } else {
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

      files.emplace(identifier, spool_info::file(name, uid));
    }
  } else {
    num_files = 0;
  }
}

spool_info::~spool_info() {
  std::ofstream outfile(info_file);
  outfile << std::to_string(num_files) << std::endl;

  for (auto id : free_ids)
    outfile << id << "\t";
  outfile << std::endl;

  for (auto file : files)
    outfile << file.second.name << "\t" << file.first << "\t"
            << file.second.owner << std::endl;
}
