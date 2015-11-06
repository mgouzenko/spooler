#include <iostream>
#include <unistd.h>
#include "spool_controller.hpp"

// Static Globals
static auto ADD = "addqueue";
static auto RM = "rmqueue";
static auto LS = "showqueue";

void print_usage() {
  std::cout << "Usage: spooler CMD\n"
            << "CMD:\n"
            << "\t" << ADD << " [filenames]\n"
            << "\t" << RM << " [filenames]\n"
            << "\t" << LS << " [filenames]\n";
}

int main(int argc, char **argv) {
  if (argc < 2) {
    print_usage();
    return 1;
  }
  std::string cmd = argv[1];

  // If there are files specified as part of the command
  // collect them into a vector.
  std::vector<std::string> files(argv + 2, argv + argc);

  uid_t euid = geteuid();
  auto print_spooler = spool_controller();

  if (cmd == ADD)
    print_spooler.add_files(files);
  else if (cmd == RM)
    print_spooler.rm_files(files);
  else if (cmd == LS)
    print_spooler.ls_files();
  else
    print_usage();

  // We restore the euid because spool_info's destructor needs it so that it
  // can write the info file back into its place within the spool directory.
  seteuid(euid);
  return 0;
}
