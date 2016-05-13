#include "HybridLargeDataStorage/HybridLargeDataStorage.hpp"
#include "FASTQParser/Parser.hpp"
#include "FASTQParser/Common.hpp"

#include <iostream>
#include <fstream>
#include <cassert>

using namespace FASTQ;

typedef Common::Nucleotide KeyItem;

class Key : public std::vector<KeyItem>{
public:
	static constexpr size_t alphabetSize = 5;

	Key(){}
	Key(std::initializer_list<KeyItem> il): std::vector<KeyItem>(il){}


	KeyItem at(const size_t index) const{
		assert(index < this->size());
		return static_cast<const std::vector<KeyItem> *>(this)->at(index);
	}

	void pop_front(){
		this->erase(this->begin());
	}

	size_t size() const{
		return static_cast<const std::vector<KeyItem> *>(this)->size();
	}

	bool empty() const{
		return static_cast<const std::vector<KeyItem> *>(this)->empty();
	}
};

typedef uint64_t Value;

int main(const int argc, const char **argv){
	assert(argc > 1);

	std::ifstream src(argv[1]);
	Parser parser(src);


	const size_t headSize = 10;
	const size_t tailSize = 10;
	const size_t substringSize = headSize + tailSize;

	HybridLargeDataStorage<Key, Value> hlds(headSize, tailSize);


	std::vector<std::pair<Common::Nucleotide, Common::Quality>> sequence;
	size_t seqPos = 0;
	while(parser.hasNext()){
		if(sequence.size() - seqPos < substringSize){
			Block current = parser.next();
			sequence.insert(sequence.end(), current.getSequence().begin(), current.getSequence().end());
		}

		while(seqPos < sequence.size() - substringSize){
			Key key;
			key.reserve(substringSize);

			for(size_t pos = 0; pos < substringSize; ++pos){
				key.push_back(sequence[pos + seqPos].first);
			}

			auto count = hlds.getValue(key);
			if(count.is_initialized()){
				count.get() += 1;
			}
			else{
				hlds.insert(key, 1);
			}

			++seqPos;
		}

	}
}

