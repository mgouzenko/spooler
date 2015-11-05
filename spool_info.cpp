#include "spool_info.hpp"
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

spool_info::file::file(std::string filename, std::string id, uid_t uid){
	name = filename;
	owner = uid;
	identifier = id;
	time_t t = time(0);
	struct tm* now = localtime(&t);

	ts.month = now->tm_mon + 1;
	ts.day = now->tm_mday;
	ts.year = now->tm_year + 1900;
	ts.hour = now->tm_hour;
	ts.minute = now->tm_min;
	ts.second = now->tm_sec;
}

std::string spool_info::ls_files(){
	std::ostringstream out;
	out << "Name" << "\t"
		<< "ID" << "\t"
		<< "Owner" << "\t"
		<< "Timestamp"
		<< std::endl;
	for(auto f: files) out << f.to_string(true);
	return out.str();
}

spool_info::file::file(std::string spool_info_line){
	std::istringstream line(spool_info_line);
	std::string next;

	std::getline(line, next, '\t');
	name = next;

	std::getline(line, next, '\t');
	identifier = next;

	std::getline(line, next, '\t');
	owner = std::stoull(next);

	std::getline(line, next, '\t');
	ts.month = std::stoi(next);

	std::getline(line, next, '\t');
	ts.day = std::stoi(next);

	std::getline(line, next, '\t');
	ts.year = std::stoi(next);

	std::getline(line, next, '\t');
	ts.hour = std::stoi(next);

	std::getline(line, next, '\t');
	ts.minute = std::stoi(next);

	std::getline(line, next, '\t');
	ts.second = std::stoi(next);
}

std::string spool_info::file::to_string(bool pretty){
	std::ostringstream file_string;
	file_string << name << "\t"
				<< identifier << "\t"
				<< owner << "\t"
				<< ts.month << (pretty ? "/":"\t")
				<< ts.day << (pretty ? "/":"\t")
				<< ts.year << (pretty ? " ":"\t")
				<< ts.hour << (pretty ? ":":"\t")
				<< ts.minute << (pretty ? ":":"\t")
				<< ts.second << std::endl;
	return file_string.str();
}

std::string spool_info::add_file(std::string filename, uid_t uid){
	int id = ++num_files;
	if(!free_ids.empty()){
		id = free_ids.back();
		free_ids.pop_back();
	}
	auto unique_filename = filename + std::to_string(id);
	files.push_back(spool_info::file(unique_filename, std::to_string(id), uid));
	return unique_filename;
}

bool spool_info::rm_file(std::string id, uid_t uid){
	for(auto f = files.begin(); f != files.end(); ++f){
		if(f->identifier == id){
			if(f->owner == uid){
				files.erase(f);
				free_ids.push_back(std::stoi(id));
				num_files--;
				return true;
			}
			else{
				break;
			}
		}
	}
	return false;
}

spool_info::spool_info(std::string spool_info_file){
	info_file = spool_info_file;
	std::ifstream infile(spool_info_file);

	if(infile){
		std::string line;

		// The first line is the number of entries
		std::getline(infile, line);
		num_files = stoi(line);

		// The second line is a tab separated list of free ids
		std::getline(infile, line);
		std::istringstream available_ids(line);

		while(std::getline(available_ids, line, '\t'))
			free_ids.push_back(std::stoi(line));

		// The remaining lines are file entries.
		while(std::getline(infile, line)) files.push_back(spool_info::file(line));
	} else {
		num_files = 0;
	}
}

spool_info::~spool_info(){
	std::ofstream outfile(info_file);
	outfile << std::to_string(num_files) << std::endl;

	for(auto id: free_ids) outfile << id << "\t";
	outfile << std::endl;

	for(auto file: files) outfile << file.to_string();
}
