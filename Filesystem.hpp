#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <cstdio>
#include <string>
#include <stdexcept>

class Filesystem{
public:
	static std::string tmpnam(){
		const char * const result = std::tmpnam(nullptr);
		if(result == nullptr){
			throw std::runtime_error("Failed to create tmp file name");
		}

		return std::string(result);
	}

	static void remove(const std::string &filename){
		const int result = std::remove(filename.c_str());
		if(result != 0){
			throw std::runtime_error("std::remove error. code: " + std::to_string(result) + "\nFilename: " + filename);
		}
	}

	static void rename(const std::string &from, const std::string &to){
		const int result = std::rename(from.c_str(), to.c_str());
		if(result != 0){
			throw std::runtime_error("std::rename error. code: " + std::to_string(result) + "\nArguments: from:" + from + " to:" + to);
		}
	}

};

#endif // FILESYSTEM_HPP

