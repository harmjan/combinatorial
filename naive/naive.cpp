#include <iostream>
#include <vector>
#include <utility>

#include <chrono>

#include <cassert>

// Global variables
unsigned int n, k;
// The format of the file in memory
std::vector<std::pair<std::vector<int>,std::vector<double>>> subfunctions(n);

const bool TIMING     = true;
const bool SHOW_STEPS = true && !TIMING;

inline unsigned long long pow2(unsigned int exp) {
	return 1UL << exp;
}

double calculateFitness(std::vector<bool>& bitstring) {
	double sum = 0;

	for( unsigned int i=0; i<n; ++i ) {
		unsigned long long subbitstring = 0;
		for( unsigned int j=0; j<k; ++j ) {
			if( bitstring[subfunctions[i].first[j]] ) {
				subbitstring += pow2(k-j-1);
			}
		}
		sum += subfunctions[i].second[subbitstring];
	}

	return sum;
}

int main() {
	std::cin >> n >> k;

	// Make sure that 2^k fits in a unsigned long long
	assert(k<sizeof(unsigned long long)*8);

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

	// Iterate as long as there is an improving move
	double fitness = calculateFitness(bitstring);
	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	if( TIMING ) {
		start = std::chrono::high_resolution_clock::now();
	} else {
		std::cout << "Starting with bitstring ";
		for( bool bit : bitstring )
			std::cout << bit;
		std::cout << " with initial fitness " << fitness << std::endl;
	}

	int iteration = 0;
	while(true) {
		int bit = -1;
		for( unsigned int i=0; i<n; ++i ) {
			bitstring[i] = !bitstring[i];
			double flippedFitness = calculateFitness(bitstring);
			if( fitness < flippedFitness ) {
				fitness = flippedFitness;
				bit = i;
			}
			bitstring[i] = !bitstring[i];
		}

		if( bit == -1 )
			break;

		bitstring[bit] = !bitstring[bit];
		++iteration;

		if( SHOW_STEPS ) {
			std::cout << "\tIteration " << iteration << ": ";
			for( bool bit : bitstring )
				std::cout << bit;
			std::cout << " fitness: " << fitness << std::endl;
		}
	}

	if( TIMING ) {
		end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;

		// Output the runtime and number of iterations space separated
		std::cout << elapsed_seconds.count() << " " << iteration << " " << fitness;
	} else {
		std::cout << "Found answer in " << iteration << " iterations" << std::endl;
		std::cout << "Local optimum: ";
		for( bool bit : bitstring )
			std::cout << bit;
		std::cout << " with total fitness: " << fitness << std::endl;
	}

	// Force a flush
	std::cout << std::flush;
}
