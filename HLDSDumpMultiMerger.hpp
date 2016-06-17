#ifndef HLDSDUMPMULTIMERGER_HPP
#define HLDSDUMPMULTIMERGER_HPP

#include "../HybridLargeDataStorage/HLDSBinaryDumpMerger.hpp"
#include "Filesystem.hpp"

#include <vector>
#include <map>
#include <istream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <functional>

template<typename Key, typename Value>
class HLDSDumpMultiMerger{
	std::vector<std::pair<size_t, std::string>> dumpParts;

	std::function<void(const size_t, const std::string &, const size_t, const std::string &)> onStartMerging;
	std::function<void(const size_t, const std::string &)> onEndMerging;
	std::function<void(const std::string &)> onEnd;

public:
	HLDSDumpMultiMerger(const std::vector<std::string> &dumpParts){
		assert(dumpParts.empty() == false);

		for(const auto path : dumpParts){
			this->dumpParts.emplace_back(HLDSDumpMultiMerger<Key, Value>::getStreamSize(path), path);
		}
	}

	HLDSDumpMultiMerger &setOnStartMerging(const std::function<void(const size_t, const std::string &, const size_t, const std::string &)> &callback){
		this->onStartMerging = callback;
		return *this;
	}

	HLDSDumpMultiMerger &setOnEndMerging(const std::function<void(const size_t, const std::string &)> &callback){
		this->onEndMerging = callback;
		return *this;
	}

	HLDSDumpMultiMerger &setOnEnd(const std::function<void(const std::string &)> &callback){
		this->onEnd = callback;
		return *this;
	}

private:
	static size_t getStreamSize(std::istream &i){
		const size_t lastPos = i.tellg();
		i.seekg(0, std::ios_base::end);
		const size_t result = i.tellg();
		i.seekg(lastPos);

		return result;
	}

	static size_t getStreamSize(const std::string &path){
		std::ifstream i(path);
		return HLDSDumpMultiMerger<Key, Value>::getStreamSize(i);
	}

	static bool comp(const std::pair<size_t, std::string> &lhs, const std::pair<size_t, std::string> &rhs){
		return lhs.first < rhs.first;
	}

public:
	void run(){
		assert(this->dumpParts.size() > 0);

		while(this->dumpParts.size() > 1){
			std::nth_element(this->dumpParts.begin(), this->dumpParts.begin(), this->dumpParts.end(), HLDSDumpMultiMerger<Key, Value>::comp);
			std::nth_element(this->dumpParts.begin(), this->dumpParts.begin() + 1, this->dumpParts.end(), HLDSDumpMultiMerger<Key, Value>::comp);

			std::ifstream lhs(this->dumpParts.at(0).second);
			std::ifstream rhs(this->dumpParts.at(1).second);

			const std::string tmpFileName = Filesystem::tmpnam();
			std::fstream result(tmpFileName, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);

			if(this->onStartMerging){
				this->onStartMerging(this->dumpParts.at(0).first, this->dumpParts.at(0).second, this->dumpParts.at(1).first, this->dumpParts.at(1).second);
			}

			HLDSBinaryDumpMerger<Key, Value> merger(lhs, rhs, result);
			merger.run();

			const size_t newFileSize = HLDSDumpMultiMerger<Key, Value>::getStreamSize(result);

			this->dumpParts.erase(this->dumpParts.begin(), this->dumpParts.begin() + 2);
			this->dumpParts.emplace_back(newFileSize, tmpFileName);

			if(this->onEndMerging){
				this->onEndMerging(newFileSize, tmpFileName);
			}
		}

		if(this->onEnd){
			this->onEnd(this->dumpParts.front().second);
		}

		this->dumpParts.clear();
	}
};

#endif // HLDSDUMPMULTIMERGER_HPP

