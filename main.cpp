#include "../HybridLargeDataStorage/HybridLargeDataStorage.hpp"
#include "../FASTQParser/Common.hpp"
#include "HLDSFeeder.hpp"


#include <iostream>
#include <fstream>

using namespace FASTQ;

constexpr Common::NucleotideBasis Common::Nucleotide::min_value;
constexpr Common::NucleotideBasis Common::Nucleotide::max_value;

using Key = Key_<Common::Nucleotide>;
typedef size_t Value;

int main(const int argc, const char **argv){
	assert(argc > 1);

	std::ifstream src(argv[1]);

	const size_t headSize = 10;
	const size_t tailSize = 50;

	const std::string dumpPathTemplate("/tmp/HLDSDump/#DUMP_INDEX.dmp");

	HLDSBinaryDumper<Key, Value> dumper;

	HLDSFeeder<Key, Value> feeder(src, headSize, tailSize, dumpPathTemplate, 10000000000, dumper, 0.01, [](const double progress){
		std::cout << progress * 100 << std::endl;
	});

	feeder.run();

}

