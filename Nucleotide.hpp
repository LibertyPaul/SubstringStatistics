#ifndef NUCLEOTIDE_HPP
#define NUCLEOTIDE_HPP

#include "NucleotideBasis.hpp"

#include <string>


class Nucleotide{
	NucleotideBasis basis;
	
public:
	Nucleotide(const NucleotideBasis basis);
	
	NucleotideBasis getBasis() const;
	bool operator==(const Nucleotide &nucleotide) const;
	bool operator!=(const Nucleotide &nucleotide) const;
	operator uint8_t()() const;
};

#endif // NUCLEOTIDE_HPP
