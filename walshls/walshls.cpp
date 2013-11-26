#include <iostream>

#include <vector>
#include <list>

#include <cassert>

const bool SHOW_STEPS = true;

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

inline unsigned long long pow2(unsigned int exp) {
	return 1 << static_cast<long long>(exp);
}

void walsh_recursive(std::vector<double>& from, std::vector<double>& to, unsigned int start, unsigned int end) {
	unsigned int size = end-start;
	if( size == 1 )
		return;

	for( unsigned int i=start; i<start+size/2; ++i ) {
		to[i]        = from[i] + from[size/2+i];
		to[size/2+i] = from[i] - from[size/2+i];
	}
	walsh_recursive(to, from, start, start+size/2);
	walsh_recursive(to, from, start+size/2, end);
}

/**
 * Calculate the walsh coefficient for a vector of 2^k entries.
 */
inline std::vector<double> walsh_transform(const std::vector<double>& fitness_evaluations) {
	std::vector<double> walsh1(fitness_evaluations);
	std::vector<double> walsh2(pow2(k));
	walsh_recursive(walsh1, walsh2, 0, pow2(k));

	// The recursive functions swaps the vector to read to and
	// write from every recursion, there are k recursions deep.
	// Return the correct vector.
	if( k%2==0 )
		return walsh1;
	else
		return walsh2;
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

		// Read the evaluations in
		for( unsigned int j=0; j<pow2(k); ++j ) {
			std::cin >> subfunctions[i].second[j];
		}
	}

	// Transform the subfunctions into the walsh coefficients
	// and add them in wprime
	Wprime.resize(pow2(n));
	for( int i=0; i<n; ++i ) {
		// Calculate the subfunction walsh coefficients
		std::vector<double> walsh_coefficients = walsh_transform(subfunctions[i].second);

		if( SHOW_STEPS ) {
			std::cout << "Walsh coefficients subfunction " << i << ": ";
			for( double coef : walsh_coefficients )
				std::cout << coef << " ";
			std::cout << std::endl;
		}
	}

	// Setup the S vector
	S.resize(n);
	for( int i=0; i<n; ++i ) {
		
	}

	// Initialize buffer B
	for( int i=0; i<n; ++i ) {
		if( S[i].value > 0 )
			B.push_back(&S[i]);
	}
}
