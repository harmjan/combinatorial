#include <iostream>
#include <vector>
#include <utility>

#include <cassert>

// Global variables (I know, I am lazy :))
unsigned int n, k;
// The format of the file in memory
std::vector<std::pair<std::vector<int>,std::vector<double>>> subfunctions(n);

const bool SHOW_STEPS = true;

inline unsigned long long pow2(unsigned int exp) {
	return 1UL << exp;
}

int main() {
	std::cin >> n >> k;

	// The bitstring is represented as a unsigned long long at some point
	// so the 
	assert(n<sizeof(unsigned long long)*8);

	// Read in the start bitstring
	std::vector<bool> bitstring(n);
	for( unsigned int i=0; i<n; ++i ) {
		int tmp;
		std::cin >> tmp;
		bitstring[i] = tmp==1;
	}

	// Read in the subfunctions
	subfunctions.resize(n);
	for( unsigned int i=0; i<n; ++i ) {
		subfunctions[i].first.resize(k);
		subfunctions[i].second.resize(pow2(k));

		// Read the mapping in
		for( unsigned int j=0; j<k; ++j ) {
			std::cin >> subfunctions[i].first[j];
		}

		// Read the evaluatio)ns in
		for( unsigned long long j=0; j<(pow2(k)); ++j ) {
			std::cin >> subfunctions[i].second[j];
		}
	}

	// Iterate over all bitstrings
	double max_fitness = 0;
	unsigned long long best = 0;
	for( unsigned long long bitstr=0; bitstr<pow2(n); ++bitstr ) {
		double sum = 0;
		for( unsigned int i=0; i<n; ++i ) {
			unsigned long long subbitstring = 0;
			for( unsigned int j=0; j<k; ++j ) {
				if( bitstr & pow2(subfunctions[i].first[j]) ) {
					subbitstring += pow2(k-j-1);
				}
			}
			sum += subfunctions[i].second[subbitstring];
		}

		if( max_fitness < sum ) {
			max_fitness = sum;
			best = bitstr;
		}
	}

	std::cout << "After exhaustive search is the best fitness: " << max_fitness << std::endl;
	std::cout << "With bitstring: ";
	for( unsigned int i=0; i<n; ++i )
		std::cout << ((best & pow2(i))? "1" : "0");
	std::cout << std::endl;
}
