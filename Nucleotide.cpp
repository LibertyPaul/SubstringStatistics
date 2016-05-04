#include "Nucleotide.hpp"

#include <stdexcept>

Nucleotide::Nucleotide(const NucleotideBasis basis): basis(basis){

}

NucleotideBasis Nucleotide::getBasis() const{
	return this->basis;
}

bool Nucleotide::operator==(const Nucleotide &nucleotide) const{
	return
}
bool Nucleotide::operator!=(const Nucleotide &nucleotide) const;
Nucleotide::operator uint8_t()() const;
