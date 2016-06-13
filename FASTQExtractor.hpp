#ifndef FASTQEXTRACTOR_HPP
#define FASTQEXTRACTOR_HPP

#include "../FASTQParser/Parser.hpp"
#include "../HybridLargeDataStorage/Key.hpp"

#include <cassert>

template<typename Key>
class FASTQExtractor{
	FASTQ::Parser parser;
	const size_t keySize;

	std::vector<FASTQ::Common::Nucleotide> buffer;
	std::vector<FASTQ::Common::Nucleotide>::const_iterator bufferPos;

	static constexpr size_t shrinkOnMargin = 1000;

public:
	FASTQExtractor(FASTQ::Parser parser, const size_t keySize): parser(std::move(parser)), keySize(keySize), buffer(), bufferPos(this->buffer.cbegin()){}

private:
	size_t itemsInBufferCount() const{
		return this->buffer.cend() - this->bufferPos;
	}

	void readUntilHasNext(){
		const size_t currentIndex = this->bufferPos - this->buffer.cbegin();

		while(this->itemsInBufferCount() < this->keySize){
			if(this->parser.hasNext() == false){
				throw std::underflow_error("No more keys available");
			}

			const FASTQ::Block &currentBlock = this->parser.next();

			for(const auto item : currentBlock.getSequence()){
				assert(item.first <= Key::value_type::max_value);
				this->buffer.push_back(item.first);
			}
		}

		this->bufferPos = this->buffer.cbegin() + currentIndex;

		if(currentIndex > FASTQExtractor::shrinkOnMargin){
			this->buffer.erase(this->buffer.cbegin(), this->bufferPos);
			this->bufferPos = this->buffer.cbegin();
		}
	}
public:

	bool hasNext(){
		try{
			this->readUntilHasNext();
			return true;
		}
		catch(std::underflow_error &){
			return false;
		}
	}

	Key next(){
		this->readUntilHasNext();

		Key result;
		result.reserve(this->keySize);

		const auto end = this->bufferPos + this->keySize;
		while(this->bufferPos != end){
			assert(this->bufferPos != this->buffer.cend());
			result.push_back(*this->bufferPos);
			++this->bufferPos;
		}

		return result;
	}

	double parserProgress() const{
		return this->parser.progress();
	}

};

#endif // FASTQEXTRACTOR_HPP

