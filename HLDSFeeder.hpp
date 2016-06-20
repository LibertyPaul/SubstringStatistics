#ifndef HLDSFEEDER_HPP
#define HLDSFEEDER_HPP

#include "../HybridLargeDataStorage/HybridLargeDataStorage.hpp"
#include "../HybridLargeDataStorage/HLDSDump.hpp"
#include "../FASTQParser/Parser.hpp"
#include "FASTQExtractor.hpp"
#include "Filesystem.hpp"

#include <istream>
#include <fstream>
#include <string>
#include <functional>

template<typename Key, typename Value>
class HLDSFeeder{
	FASTQExtractor<Key> extractor;

	HybridLargeDataStorage<Key, Value> hlds;
	const size_t hldsMaxRAM;

	size_t dumpCount = 0;

	std::function<void(double)> onProgressCallback;
	std::function<void(void)> onStartFeedingCallback;
	std::function<void(void)> onStopFeedingCallback;
	std::function<void(const std::string &)> onDumpCreatedCallback;


public:
	HLDSFeeder(std::istream &FASTQSource, const size_t headSize, const size_t tailSize, const size_t hldsMaxRAM):
		extractor(FASTQ::Parser(FASTQSource), headSize + tailSize),
		hlds(headSize, tailSize),
		hldsMaxRAM(hldsMaxRAM){}

	HLDSFeeder &setOnProgress(const std::function<void(double)> &onProgressCallback){
		this->onProgressCallback = onProgressCallback;
		return *this;
	}

	HLDSFeeder &setOnDumpCreated(const std::function<void(const std::string &)> &onDumpCreatedCallback){
		this->onDumpCreatedCallback = onDumpCreatedCallback;
		return *this;
	}

	HLDSFeeder &setOnStartFeeding(const std::function<void(void)> &onStartFeedingCallback){
		this->onStartFeedingCallback = onStartFeedingCallback;
		return *this;
	}

	HLDSFeeder &setOnStopFeeding(const std::function<void()> &onStopFeedingCallback){
		this->onStopFeedingCallback = onStopFeedingCallback;
		return *this;
	}

private:
	void dump(){
		const std::string dumpPath = Filesystem::tmpnam();
		std::ofstream dst(dumpPath, std::ios_base::trunc);
		if(dst.good() == false){
			throw std::runtime_error("Dump dst file opening error");
		}
		HLDSDumpWriter<Key, Value> writer(dst);

		writer.dumpAll(this->hlds);

		++this->dumpCount;

		if(this->onDumpCreatedCallback){
			this->onDumpCreatedCallback(dumpPath);
		}
	}

public:
	HLDSFeeder &run(){
		this->hlds.clear();

		if(this->onStartFeedingCallback){
			this->onStartFeedingCallback();
		}

		while(this->extractor.hasNext()){
			Key currentKey = this->extractor.next();

			if(this->onProgressCallback){
				this->onProgressCallback(this->extractor.parserProgress());
			}

			auto it = this->hlds.find(currentKey);
			if(it == this->hlds.end()){
				this->hlds.insert(currentKey, 1);
			}
			else{
				*it += 1;
			}

			if(this->onStopFeedingCallback){
				this->onStopFeedingCallback();
			}

			if(this->hlds.getApproximateRAMUsage() > this->hldsMaxRAM){
				this->dump();
				this->hlds.clear();
			}

			if(this->onStartFeedingCallback){
				this->onStartFeedingCallback();
			}
		}

		if(this->onStopFeedingCallback){
			this->onStopFeedingCallback();
		}

		this->dump();
		this->hlds.clear();

		return *this;
	}

	size_t getDumpCount() const{
		return this->dumpCount;
	}

	size_t getDumpId() const{
		return this->hlds.getId();
	}


};

#endif // HLDSFEEDER_HPP

