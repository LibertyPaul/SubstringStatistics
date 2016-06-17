#include "../HybridLargeDataStorage/HybridLargeDataStorage.hpp"
#include "../FASTQParser/Common.hpp"
#include "HLDSFeeder.hpp"
#include "HLDSDumpMultiMerger.hpp"
#include "Filesystem.hpp"

#include <iostream>
#include <fstream>
#include <functional>
#include <istream>

using namespace FASTQ;

constexpr Common::NucleotideBasis Common::Nucleotide::min_value;
constexpr Common::NucleotideBasis Common::Nucleotide::max_value;

using Key = Key_<Common::Nucleotide>;
typedef size_t Value;

void man(const std::string &selfName){
	std::cout << "Usage:" << std::endl;
	std::cout << selfName << "<FASTQ-file-path> <K-size> <Memory-available-MB> <result-file-path>" << std::endl;
}

int main(const int argc, const char **argv){
	if(argc != 5){
		man(std::string(argv[0]));
		return -1;
	}

	std::ifstream src(argv[1]);
	const size_t K = std::stoull(argv[2]);
	const size_t memoryAvailable = std::stoull(argv[3]) * 1024 * 1024;
	const std::string resultFilePath(argv[4]);

	const size_t headMaxSize = 10;

	size_t headSize, tailSize;
	if(K < headMaxSize){
		headSize = K;
		tailSize = 0;
	}
	else{
		headSize = headMaxSize;
		tailSize = K - headMaxSize;
	}


	HLDSFeeder<Key, Value> feeder(src, headSize, tailSize, memoryAvailable);

	std::vector<std::string> dumps;


	feeder.setOnProgress([](const double progress){
		static double lastProgress = 0.0;
		const double progressStep = 0.01;

		if(progress - lastProgress > progressStep){
			lastProgress = progress;
			std::cout << progress * 100 << std::endl;
		}
	}).setOnDumpCreated([&dumps](const std::string &dumpPath){
		std::cout << "Dump '" << dumpPath << "' was created." << std::endl;
		dumps.push_back(dumpPath);
	}).run();

	std::string resultPath;
	std::vector<std::string> tmpFiles;

	HLDSDumpMultiMerger<Key, Value> merger(dumps);
	merger.setOnStartMerging([](const size_t size1, const std::string &path1, const size_t size2, const std::string &path2){
		std::cout
			<< "Starting to merge "
			<< path1 << "(" << size1 << ")"
			<< " and "
			<< path2 << "(" << size2 << ")"
			<< std::endl;
	}).setOnEndMerging([&tmpFiles](const size_t size, const std::string &path){
		tmpFiles.push_back(path);
		std::cout << "Merge was completed. Result file: " << path << "(" << size << ")" << std::endl;
	}).setOnEnd([&resultPath](const std::string &path){
		resultPath = path;
	}).run();

	try{
		Filesystem::rename(resultPath, resultFilePath);
	}
	catch(std::exception &e){
		std::cerr << e.what() << std::endl;
	}


	for(const std::string &path : dumps){
		try{
			Filesystem::remove(path);
		}
		catch(std::exception &e){
			std::cerr << e.what() << std::endl;
		}
	}

	for(const std::string &path : tmpFiles){
		try{
			Filesystem::remove(path);
		}
		catch(std::exception &e){
			std::cerr << e.what() << std::endl;
		}
	}
}























