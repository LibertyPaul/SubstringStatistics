#ifndef NUCLEOTIDEBASIS_HPP
#define NUCLEOTIDEBASIS_HPP

enum class NucleotideBasis{
	A = 0,
	T = 1,
	G = 2,
	C = 3
};

operator std::string()(const NucleotideBasis &basis){
	switch(this->basis){
	case NucleotideBasis::A:
		return "A";
	case NucleotideBasis::T:
		return "T";
	case NucleotideBasis::G:
		return "G";
	case NucleotideBasis::C:
		return "C";
	default:
		throw std::runtime_error("Unknown basis");
	}
}

#endif // NUCLEOTIDEBASIS_HPP

