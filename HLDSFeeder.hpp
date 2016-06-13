#ifndef HLDSFEEDER_HPP
#define HLDSFEEDER_HPP

#include "../HybridLargeDataStorage/HybridLargeDataStorage.hpp"
#include "../HybridLargeDataStorage/HLDSDumper.hpp"
#include "../FASTQParser/Parser.hpp"
#include "FASTQExtractor.hpp"

#include <istream>
#include <fstream>
#include <string>
#include <regex>
#include <functional>

template<typename Key, typename Value>
class HLDSFeeder{
	FASTQExtractor<Key> extractor;

	HybridLargeDataStorage<Key, Value> hlds;
	const std::string dumpPathTemplate;

	const size_t hldsMaxRAM;

	const HLDSDumperBase<Key, Value> &dumper;

	const std::regex dumpPathReplaceToken = std::regex("#DUMP_INDEX");
	size_t dumpIndex = 1;

	const double progressStep;
	const std::function<void(double)> onProgressCallback;


public:
	HLDSFeeder(std::istream &FASTQSource, const size_t headSize, const size_t tailSize, const std::string &dumpPathTemplate, const size_t hldsMaxRAM, const HLDSDumperBase<Key, Value> &dumper, const double progressStep, const std::function<void(double)> &onProgressCallback):
		extractor(FASTQ::Parser(FASTQSource), headSize + tailSize),
		hlds(headSize, tailSize),
		dumpPathTemplate(dumpPathTemplate),
		hldsMaxRAM(hldsMaxRAM),
		dumper(dumper),
		progressStep(progressStep),
		onProgressCallback(onProgressCallback)
	{
		const size_t replaceTokenPos = this->dumpPathTemplate.find("#DUMP_INDEX");
		if(replaceTokenPos == std::string::npos){
			throw std::invalid_argument("dumpPathTemplateIndexToken wasn't found in dumpPathTemplate");
		}
	}

private:
	void dump(){
		const std::string dumpPath = std::regex_replace(dumpPathTemplate, HLDSFeeder::dumpPathReplaceToken, std::to_string(dumpIndex++));
		std::ofstream dst(dumpPath, std::ios_base::trunc);
		if(dst.good() == false){
			throw std::runtime_error("Dump dst file opening error");
		}

		dumper.dump(this->hlds, dst);
	}

public:
	void run(){
		double lastProgress = 0.0;

		this->hlds.clear();

		while(this->extractor.hasNext()){
			Key currentKey = this->extractor.next();

			const double currentProgress = this->extractor.parserProgress();
			if(currentProgress - lastProgress > this->progressStep){
				lastProgress = currentProgress;
				this->onProgressCallback(lastProgress);
			}

			auto it = this->hlds.find(currentKey);
			if(it == this->hlds.end()){
				this->hlds.insert(currentKey, 1);
			}
			else{
				*it += 1;
			}

			if(this->hlds.getApproximateRAMUsage() > this->hldsMaxRAM){
				this->dump();
				this->hlds.clear();
			}
		}

		this->dump();
		this->hlds.clear();
	}


};

#endif // HLDSFEEDER_HPP

