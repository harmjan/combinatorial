#include <iostream>
#include <vector>
#include <utility>

// Global variables (I know, I am lazy :))
int n, k;
// The format of the file in memory
std::vector<std::pair<std::vector<int>,std::vector<double>>> subfunctions(n);

const bool SHOW_STEPS = true;

double calculateFitness(std::vector<bool>& bitstring) {
	double sum = 0;

	for( int i=0; i<n; ++i ) {
		int subbitstring = 0;
		for( int j=0; j<k; ++j ) {
			if( bitstring[subfunctions[i].first[j]] ) {
				subbitstring += 1<<j;
			}
		}
		sum += subfunctions[i].second[subbitstring];
	}

	return sum;
}

int main() {
	std::cin >> n >> k;

	// Read in the start bitstring
	std::vector<bool> bitstring(n);
	for( int i=0; i<n; ++i ) {
		int tmp;
		std::cin >> tmp;
		bitstring[i] = tmp==1;
	}

	// Read in the subfunctions
	subfunctions.resize(n);
	for( int i=0; i<n; ++i ) {
		subfunctions[i].first.resize(k);
		subfunctions[i].second.resize(1<<k);

		// Read the mapping in
		for( int j=0; j<k; ++j ) {
			std::cin >> subfunctions[i].first[j];
		}

		// Read the evaluatio)ns in
		for( int j=0; j<(1<<k); ++j ) {
			std::cin >> subfunctions[i].second[j];
		}
	}

	// Iterate as long as there is an improving move
	double fitness = calculateFitness(bitstring);
	std::cout << "Starting with bitstring ";
	for( bool bit : bitstring )
		std::cout << bit;
	std::cout << " with initial fitness " << fitness << std::endl;

	int iteration = 0;
	while(true) {
		int bit = -1;
		for( int i=0; i<n; ++i ) {
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

	std::cout << "Found answer in " << iteration << " iterations" << std::endl;
	std::cout << "Local optimum: ";
	for( bool bit : bitstring )
		std::cout << bit;
	std::cout << " with fitness: " << fitness << std::endl;
	std::cout << std::flush;
}
