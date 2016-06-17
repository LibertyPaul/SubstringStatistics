#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <cstdio>
#include <string>
#include <stdexcept>

class Filesystem{
public:
	static std::string tmpnam(){
		return std::string(std::tmpnam(nullptr));
	}

	static void remove(const std::string &filename){
		const int result = std::remove(filename.c_str());
		if(result != 0){
			throw std::runtime_error("std::remove error. code: " + std::to_string(result));
		}
	}

	static void rename(const std::string &from, const std::string &to){
		const int result = std::rename(from.c_str(), to.c_str());
		if(result != 0){
			throw std::runtime_error("std::rename error. code: " + std::to_string(result));
		}
	}

};

#endif // FILESYSTEM_HPP

