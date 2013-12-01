#include <iostream>

#include <vector>
#include <list>

#include <cassert>

const bool SHOW_STEPS = true;
const bool DEBUG      = true;

// Global variables (I know, I am lazy :))
unsigned int n, k;
// The format of the file in memory
std::vector<std::pair<std::vector<int>,std::vector<double>>> subfunctions(n);

// Prototype so the S vector can refer to it
struct walsh_vector_entry;

struct s_vector_entry {
	double value;
	std::list<walsh_vector_entry*> walsh_coefficients;
	std::list<s_vector_entry*>::iterator buffer_entry;
};
std::vector<s_vector_entry> S;

struct walsh_vector_entry {
	double value;
	std::list<s_vector_entry*> influenced_sums;
};
std::vector<walsh_vector_entry> Wprime;

std::list<s_vector_entry*> B;

inline unsigned long long pow2(unsigned int exp) {
	return 1UL << exp;
}

void walsh_recursive(std::vector<double>& from, std::vector<double>& to, unsigned int start, unsigned int end) {
	unsigned int size = end-start;
	if( size == 1 )
		return;

	for( unsigned int i=start; i<start+size/2; ++i ) {
		to[i]        = (from[i] + from[size/2+i])/2;
		to[size/2+i] = (from[i] - from[size/2+i])/2;
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

		// Read the evaluations in
		for( unsigned int j=0; j<pow2(k); ++j ) {
			std::cin >> subfunctions[i].second[j];
		}
	}

	// Transform the subfunctions into the walsh coefficients
	// and initialize the Wprime and S datastructures
	Wprime.resize(pow2(n));
	S.resize(n);
	for( unsigned int i=0; i<n; ++i ) {
		// Calculate the subfunction walsh coefficients
		std::vector<double> walsh_coefficients = walsh_transform(subfunctions[i].second);

		if( DEBUG ) {
			std::cout << "Walsh coefficients of subfunction " << i << ": ";
			for( double coef : walsh_coefficients )
				std::cout << coef << " ";
			std::cout << std::endl;
		}

		// Loop over all possible 2^K walsh coefficients and add them to
		// the Wprime vector. Set up the pointers from Wprime to S and
		// vice versa.
		for( unsigned long long walsh_index=0; walsh_index<pow2(k); ++walsh_index ) {
			// Skip this walsh coefficient if it is zero
			if( walsh_coefficients[walsh_index] == 0 ) { // TODO epsilon compare
				continue;
			}

			// Loop over all bits in walsh_index; if a bit is
			// set, set the correct bit in the wprime_index
			unsigned long long wprime_index = 0;
			for( unsigned int j=0; j<k; ++j ) {
				if( walsh_index & pow2(j) ) {
					wprime_index += pow2(subfunctions[i].first[k-j-1]);
				}
			}

			// Calculate the correct sign
			// TODO This implementation is O(n), it can probably be done smarter
			unsigned long long mask=1;
			int bitcount = 0;
			for( unsigned int j=0; j<n; ++j ) {
				bitcount += ((bitstring[j]&&(wprime_index&mask)) ? 1 : 0 );
				mask = mask << 1;
			}

			// Add the walsh coefficient with the right sign to Wprime
			if( bitcount%2 == 0 ) {
				Wprime[wprime_index].value += walsh_coefficients[walsh_index];
			} else {
				Wprime[wprime_index].value -= walsh_coefficients[walsh_index];
			}

			// Look at what bits are set in this walsh coefficient number
			// which translates to what bits in the bitstring and thus
			// the S vector are influenced by this Wprime entry.
			for( unsigned int j=0; j<k; ++j ) {
				if( walsh_index & pow2(j) ) {
					Wprime[wprime_index].influenced_sums.push_front(&S[subfunctions[i].first[j]]);
					S[subfunctions[i].first[j]].walsh_coefficients.push_front(&Wprime[wprime_index]);
				}
			}
		}

		// Initialize the buffer iterator to the past the end iterator
		S[i].buffer_entry = B.end();
	}

	if( DEBUG ) {
		std::cout << std::endl << "Non-zero entries of Wprime vector:" << std::endl;
		for( unsigned long long i=0; i<pow2(n); ++i ) {
			if( Wprime[i].value != 0 ) { // TODO epsilon comparison
				std::cout << "Wprime[";
				for( unsigned int j=0; j<n; ++j ) {
					std::cout << ((i & pow2(n-j-1)) ? "1" : "0") ;
				}
				std::cout << "] = " << Wprime[i].value << std::endl;
			}
		}
	}

	// Calculate the initial S vector value by summing all walsh
	// coefficients that influence this sum.
	for( unsigned int i=0; i<n; ++i ) {
		S[i].value = 0;
		for( struct walsh_vector_entry* coef_index : S[i].walsh_coefficients ) {
			S[i].value += coef_index->value;
		}
	}

	if( DEBUG ) {
		std::cout << std::endl << "The S vector" << std::endl;
		for( unsigned int i=0; i<n; ++i ) {
			std::cout << "S[" << i << "] = " << S[i].value << " with " << S[i].walsh_coefficients.size() << " walsh coefficients" << std::endl;
		}
	}

	// Initialize buffer B
	for( unsigned int i=0; i<n; ++i ) {
		if( S[i].value > 0 ) {
			B.push_front(&S[i]);
			S[i].buffer_entry = B.begin();
		}
	}
}
