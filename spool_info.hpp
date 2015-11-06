#ifndef __SPOOL_INFO_HPP__
#define __SPOOL_INFO_HPP__
#include <ctime>
#include <unistd.h>
#include <string>
#include <vector>

class spool_info{
	private:
	struct timestamp{
		int month;
		int day;
		int year;
		int hour;
		int minute;
		int second;
	};

	class file{
		public:
		// some arbitrary name (*not* the original file name,
		// but a number is fine)
		std::string name;

		// unique identifier (may or may not be the same as file name)
		std::string identifier;

		// the owner (the userid who made the request)
		uid_t owner;

		// the date and time
		timestamp ts;

		file(std::string filename, std::string id, uid_t uid);
		file(std::string spool_info_line);
		std::string to_string(bool pretty=false);
	};

	std::vector<file> files;
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
