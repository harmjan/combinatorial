#include <iostream>

#include <vector>
#include <list>

#include <cassert>

// Global variables (I know, I am lazy :))
int n, k;
// The format of the file in memory
std::vector<std::pair<std::vector<int>,std::vector<double>>> subfunctions(n);

// Prototype so the S vector can refer to it
struct walsh_vector_entry;

struct s_vector_entry {
	double value;
	std::list<walsh_vector_entry*> walsh_coefficients;
	std::list<s_vector_entry*>::iterator buffer_entry;

	void flip();
};
std::vector<s_vector_entry> S;

struct walsh_vector_entry {
	double value;
	std::list<s_vector_entry*> influenced_sums;

	void flip() {
		for( auto it=influenced_sums.begin(); it!=influenced_sums.end(); ++it ) {
			(*it)->value -= 2*this->value;
		}
		this->value *= -1;
	}
};
std::vector<walsh_vector_entry> Wprime;

void s_vector_entry::flip() {
	for( auto it=walsh_coefficients.begin(); it!=walsh_coefficients.end(); ++it ) {
		(*it)->flip();
	}
}

std::list<s_vector_entry*> B;

unsigned long long pow2(unsigned int exp) {
	return 1 << static_cast<long long>(exp);
}

/**
 * Transform a vector of fitness evaluations to
 * a vector of walsh coefficient
 */
std::vector<double> walsh_transform(const std::vector<double>& fitness_evaluations) {
	if( fitness_evaluations.size() == 1 )
		return fitness_evaluations;

	// Make sure that the size of the given vector is a power of 2
	assert((fitness_evaluations.size()-1)&fitness_evaluations.size()==0);

	std::vector<double> walsh(fitness_evaluations);

	return walsh;
}

int main()
{
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
		subfunctions[i].second.resize(pow2(k));

		// Read the mapping in
		for( int j=0; j<k; ++j ) {
			std::cin >> subfunctions[i].first[j];
		}

		// Read the evaluatio)ns in
		for( int j=0; j<pow2(k); ++j ) {
			std::cin >> subfunctions[i].second[j];
		}
	}

	// Transform the subfunctions into the walsh coefficients
	// and add them in wprime
	Wprime.resize(pow2(n));
	for( int i=0; i<n; ++i ) {
		// TODO some magic
		walsh_transform(subfunctions[i].second);
	}

	// Setup the S vector
	for( int i=0; i<n; ++i ) {
		
	}

	// Initialize buffer B
	for( int i=0; i<n; ++i ) {
		if( S[i].value > 0 )
			B.push_back(&S[i]);
	}
}
